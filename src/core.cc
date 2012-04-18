
#include <dbglog.h>
#include <threadserver/threadserver.h>
#include <threadserver/error.h>
#include <mysqlwrapper/result.h>
#include <mysqlwrapper/util.h>
#include <mysqlwrapper/error.h>
#include "core.h"


Core_t::Core_t(ThreadServer::ThreadServer_t &threadServer)
  : threadServer(threadServer),
    config(threadServer),
    db(0),
    magic(0),
    teng(0),
    mailer(config.smtp.address, config.smtp.port)
{
    connectDb();
    checkDb();
    db.release();
}

Core_t::~Core_t()
{
}

void Core_t::threadCreate()
{
    magic.reset(new Magic_t());
    Teng::Teng_t::Settings_t tengSettings;
    teng.reset(new Teng::Teng_t("/tmp", tengSettings));
    connectDb();
}

void Core_t::threadDestroy()
{
    delete magic.release();
    delete teng.release();
    disconnectDb();
}

void Core_t::connectDb()
{
    db.reset(connectDb("database"));
}

void Core_t::disconnectDb()
{
    delete db.release();
}

MySQLWrapper::MySQLWrapper_t* Core_t::connectDb(const std::string &section)
{
    const std::string fullSection("spamserver::" + section);

    // create MySQL wrapper instance
    std::auto_ptr<MySQLWrapper::MySQLWrapper_t> result(
        new MySQLWrapper::MySQLWrapper_t(
            threadServer.configuration.get<std::string>(fullSection + "::master.Host"),
            threadServer.configuration.get<std::string>(fullSection + "::master.Socket", ""),
            threadServer.configuration.get<int>(fullSection + "::master.Port", 0),
            threadServer.configuration.get<std::string>(fullSection + "::master.User"),
            threadServer.configuration.get<std::string>(fullSection + "::master.Pass"),
            threadServer.configuration.get<std::string>(fullSection + "::master.Name"),
            threadServer.configuration.get<std::string>(fullSection + "::slave.Host", ""),
            threadServer.configuration.get<std::string>(fullSection + "::slave.Socket", ""),
            threadServer.configuration.get<int>(fullSection + "::slave.Port", 0),
            threadServer.configuration.get<std::string>(fullSection + "::slave.User", ""),
            threadServer.configuration.get<std::string>(fullSection + "::slave.Pass", ""),
            threadServer.configuration.get<std::string>(fullSection + "::slave.Name", "")));

    {
        MySQLWrapper::RWTransaction_t trans(result.get());
        result->query("SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED");
        trans.commit();
    }

    {
        MySQLWrapper::ROTransaction_t trans(result.get());
        result->query("SET SESSION TRANSACTION ISOLATION LEVEL READ COMMITTED");
        trans.commit();
    }

    return result.release();
}

void Core_t::checkDb()
{
    {
        MySQLWrapper::ROTransaction_t trans(db.get());
        db->query("SELECT 1");
        MySQLWrapper::Result_t result(db.get());
        trans.commit();
    }
    {
        MySQLWrapper::RWTransaction_t trans(db.get());
        db->query("SELECT 1");
        MySQLWrapper::Result_t result(db.get());
        trans.commit();
    }
}

void Core_t::send(const Mail_t &mail)
{
    Mailer_t::Message_t message;

    message.setSender(mail.from);

    for (std::vector<std::string>::const_iterator ito(mail.to.begin()) ;
         ito != mail.to.end() ;
         ++ito) {

        message.addRecipient(*ito);
    }

    for (std::vector<std::string>::const_iterator icc(mail.cc.begin()) ;
         icc != mail.cc.end() ;
         ++icc) {

        message.addCCRecipient(*icc);
    }

    for (std::vector<std::string>::const_iterator ibcc(mail.bcc.begin()) ;
         ibcc != mail.bcc.end() ;
         ++ibcc) {

        message.addBCCRecipient(*ibcc);
    }

    message.setSubject(mail.subject);

    message.setBody(mail.plainBody, mail.htmlBody);

    for (std::vector<Mail_t::Attachment_t>::const_iterator iattachments(mail.attachments.begin()) ;
         iattachments != mail.attachments.end() ;
         ++iattachments) {

        std::string contentType((*magic)(iattachments->data));
        message.addAttachment(
            iattachments->data,
            contentType,
            iattachments->filename,
            iattachments->description);
    }

    mailer.send(message);
}

void Core_t::sendMail(const Mail_t &mail)
{
    try {
        send(mail);
    } catch (const std::exception &e) {
        ERROR(ERR1, FE_INTERNAL, "Couldn't send mail: %s", e.what());
    }
}

void Core_t::sendTemplatedMail(const TemplatedMail_t &templatedMail)
{
    MySQLWrapper::ROTransaction_t trans(db.get());

    std::stringstream query;
    query << "SELECT id, name, `from`, subject, body_plain, body_html "
               "FROM email_template "
              "WHERE name=" << db->escape(templatedMail.templateName);

    db->query(query.str());

    MySQLWrapper::Result_t result(db.get());

    if (!result) {
        ERROR(ERR1, FE_NEXISTS,
            "Template with name=%s doesn't exist",
            templatedMail.templateName.c_str());
    }

    Template_t templ;
    result >> templ.id
           >> templ.name
           >> templ.from
           >> templ.subject
           >> templ.plainBody
           >> templ.htmlBody;

    Mail_t mail;
    mail.from = templ.from;
    mail.to = templatedMail.to;
    mail.cc = templatedMail.cc;
    mail.bcc = templatedMail.bcc;
    mail.subject = templ.subject;
    mail.plainBody = render(templ.plainBody, "text/plain", templatedMail.data);
    mail.htmlBody = render(templ.htmlBody, "text/html", templatedMail.data);
    mail.attachments = templatedMail.attachments;

    try {
        send(mail);
    } catch (const std::exception &e) {
        ERROR(ERR1, FE_INTERNAL, "Couldn't send mail: %s", e.what());
    }
}

int Core_t::template_create(const Template_t &templ)
{
    MySQLWrapper::RWTransaction_t trans(db.get());

    std::stringstream query;
    query << "INSERT "
               "INTO email_template "
                "SET name=" << db->escape(templ.name) << ","
                    "`from`=" << db->escape(templ.from) << ","
                    "subject=" << db->escape(templ.subject) << ","
                    "body_plain=" << db->escape(templ.plainBody) << ","
                    "body_html=" << db->escape(templ.htmlBody);

    try {
        db->query(query.str());
    } catch (const MySQLWrapper::MySQLWrapperError_t &e) {
        switch (e.code()) {
        case ER_DUP_ENTRY:
            ERROR(ERR1, FE_EXISTS,
                "Template with name=%s already exists",
                templ.name.c_str());
        default:
            throw e;
        }
    }

    int id(db->lastInsertId());

    trans.commit();

    return id;
}

Template_t Core_t::template_getAttributes(const int id)
{
    MySQLWrapper::ROTransaction_t trans(db.get());

    std::stringstream query;
    query << "SELECT id, name, `from`, subject, body_plain, body_html "
               "FROM email_template "
              "WHERE id=" << id;

    db->query(query.str());

    MySQLWrapper::Result_t result(db.get());

    if (!result) {
        ERROR(ERR1, FE_NEXISTS, "Template with id=%d doesn't exist", id);
    }

    Template_t templ;
    result >> templ.id
           >> templ.name
           >> templ.from
           >> templ.subject
           >> templ.plainBody
           >> templ.htmlBody;

    trans.commit();

    return templ;
}

void Core_t::template_setAttributes(const int id,
                                    const Template_t &templ)
{
    MySQLWrapper::RWTransaction_t trans(db.get());

    std::stringstream query;
    query << "SELECT id "
               "FROM email_template "
              "WHERE id=" << id << " "
                "FOR UPDATE";

    db->query(query.str());

    if (!MySQLWrapper::Result_t(db.get())) {
        ERROR(ERR1, FE_NEXISTS, "Template with id=%d doesn't exist", id);
    }

    if (!templ.mask) {
        trans.commit();
        return;
    }

    query.str("");
    query << "UPDATE email_template "
                "SET ";

    std::string comma;
    if (templ.mask & Template_t::MASK_NAME) {
        query << comma << "name=" << db->escape(templ.name);
        comma = ",";
    }
    if (templ.mask & Template_t::MASK_FROM) {
        query << comma << "`from`=" << db->escape(templ.from);
        comma = ",";
    }
    if (templ.mask & Template_t::MASK_SUBJECT) {
        query << comma << "subject=" << db->escape(templ.subject);
        comma = ",";
    }
    if (templ.mask & Template_t::MASK_PLAINBODY) {
        query << comma << "body_plain=" << db->escape(templ.plainBody);
        comma = ",";
    }
    if (templ.mask & Template_t::MASK_HTMLBODY) {
        query << comma << "body_html=" << db->escape(templ.htmlBody);
        comma = ",";
    }

    query << " WHERE id=" << id;

    try {
        db->query(query.str());
    } catch (const MySQLWrapper::MySQLWrapperError_t &e) {
        switch (e.code()) {
        case ER_DUP_ENTRY:
            ERROR(ERR1, FE_EXISTS,
                "Template with name=%s already exists",
                templ.name.c_str());
        default:
            throw e;
        }
    }

    trans.commit();
}

void Core_t::template_remove(const int id)
{
    MySQLWrapper::RWTransaction_t trans(db.get());

    std::stringstream query;
    query << "SELECT id "
               "FROM email_template "
              "WHERE id=" << id << " "
                "FOR UPDATE";

    db->query(query.str());

    if (!MySQLWrapper::Result_t(db.get())) {
        ERROR(ERR1, FE_NEXISTS, "Template with id=%d doesn't exist", id);
    }

    query.str("");
    query << "DELETE "
               "FROM email_template "
              "WHERE id=" << id;

    db->query(query.str());

    trans.commit();
}

std::pair<TemplateList_t, size_t> Core_t::listTemplates(TemplateListingAttributes_t &attributes)
{
    MySQLWrapper::ROTransaction_t trans(db.get());

    std::stringstream query;
    query << "SELECT SQL_CALC_FOUND_ROWS "
                    "id, name, `from`, subject, body_plain, body_html "
               "FROM email_template "
              "ORDER BY name "
              "LIMIT " << (attributes.pageSize * attributes.page) << "," << attributes.pageSize;

    db->query(query.str());

    MySQLWrapper::Result_t result(db.get());

    TemplateList_t templs;
    while (result) {
        Template_t templ;
        result >> templ.id
               >> templ.name
               >> templ.from
               >> templ.subject
               >> templ.plainBody
               >> templ.htmlBody;

        templs.push_back(templ);
    }

    db->query("SELECT FOUND_ROWS()");

    size_t totalCount(0);
    MySQLWrapper::Result_t(db.get()) >> totalCount;

    trans.commit();

    return std::make_pair(templs, totalCount);
}


int Core_t::template_resolve(const std::string &name)
{
    MySQLWrapper::ROTransaction_t trans(db.get());

    std::stringstream query;
    query << "SELECT id "
               "FROM email_template "
              "WHERE name=" << db->escape(name);

    db->query(query.str());

    MySQLWrapper::Result_t result(db.get());

    if (!result) {
        ERROR(ERR1, FE_NEXISTS, "Template with name=%s doesn't exist", name.c_str());
    }

    int id;
    result >> id;

    trans.commit();

    return id;
}

std::string Core_t::render(const std::string &templateString,
                           const std::string &contentType,
                           const Teng::Fragment_t &data)
{
    std::string output;

    Teng::StringWriter_t writer(output);

    Teng::Error_t error;

    int res(teng->generatePage(
        templateString,
        "", // dict
        "", // lang
        "", // param
        contentType, // contentType
        "utf-8", // encoding
        data, // data
        writer, // writer
        error)); // error

    if (res) {
        std::stringstream errorStream;
        error.dump(errorStream);

        ERROR(ERR1, FE_TENG,
            "Can't render template: %s",
            errorStream.str().c_str());
    }

    return output;
}

