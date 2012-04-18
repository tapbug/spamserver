
#include <dbglog.h>
#include <threadserver/handlers/cppfrpchandler/cppfrpchandler.h>

#include "frpcinterface.h"
#include "types.h"

FrpcInterface_t::FrpcInterface_t(ThreadServer::CppFrpcHandler_t &handler, Core_t &core)
  : handler(handler),
    core(core)
{
}

void FrpcInterface_t::threadCreate()
{
    registerMethods();
}

void FrpcInterface_t::threadDestroy()
{
}

#define REGMET(name, method, signature) \
    handler.registerMethod( \
        name, \
        ThreadServer::CppFrpcHandler_t::boundMethod(&FrpcInterface_t::method, *this), \
        signature)

void FrpcInterface_t::registerMethods()
{
    REGMET("sendMail", sendMail, "S:S");
    REGMET("sendTemplatedMail", sendTemplatedMail, "S:S");

    REGMET("template.create", template_create, "S:S");
    REGMET("template.getAttributes", template_getAttributes, "S:i");
    REGMET("template.setAttributes", template_setAttributes, "S:iS");
    REGMET("template.remove", template_remove, "S:i");
    REGMET("listTemplates", listTemplates, "S:S");
    REGMET("template.resolve", template_resolve, "S:s");
}

#undef REGMET

#define METHOD(name) \
    FRPC::Value_t& FrpcInterface_t::name(FRPC::Pool_t &pool, FRPC::Array_t &params)

#define CONVERT(x) convert(pool, x)

template<typename T_t>
FRPC::Array_t& convert(FRPC::Pool_t &pool,
                       const T_t &array)
{
    typename T_t::const_iterator iarray;
    typename T_t::const_iterator arrayEnd = array.end();

    FRPC::Array_t &result(pool.Array());

    for (iarray = array.begin() ;
         iarray != arrayEnd ;
         ++iarray) {

        result.push_back(CONVERT(*iarray));
    }
    return result;
}

FRPC::Struct_t& convert(FRPC::Pool_t &pool,
                        const Template_t &templ)
{
    return pool.Struct(
        "id", pool.Int(templ.id),
        "name", pool.String(templ.name),
        "from", pool.String(templ.from),
        "subject", pool.String(templ.subject),
        "plainBody", pool.String(templ.plainBody)).append(
        "htmlBody", pool.String(templ.htmlBody));
}

Mail_t mailFromRpc(FRPC::Struct_t &attributes)
{
    Mail_t mail;
    mail.from = FRPC::String(attributes["from"]);

    FRPC::Array_t &toArray(FRPC::Array(attributes["to"]));
    for (FRPC::Array_t::const_iterator itoArray(toArray.begin()) ;
         itoArray != toArray.end() ;
         ++itoArray) {

        mail.to.push_back(FRPC::String(**itoArray));
    }

    if (attributes.has_key("cc")) {
        FRPC::Array_t &ccArray(FRPC::Array(attributes["cc"]));
        for (FRPC::Array_t::const_iterator iccArray(ccArray.begin()) ;
             iccArray != ccArray.end() ;
             ++iccArray) {

            mail.cc.push_back(FRPC::String(**iccArray));
        }
    }

    if (attributes.has_key("bcc")) {
        FRPC::Array_t &bccArray(FRPC::Array(attributes["bcc"]));
        for (FRPC::Array_t::const_iterator ibccArray(bccArray.begin()) ;
             ibccArray != bccArray.end() ;
             ++ibccArray) {

            mail.bcc.push_back(FRPC::String(**ibccArray));
        }
    }

    mail.subject = FRPC::String(attributes["subject"]);
    mail.plainBody = FRPC::String(attributes["plainBody"]);
    if (attributes.has_key("htmlBody")) {
        mail.htmlBody = FRPC::String(attributes["htmlBody"]);
    }

    if (attributes.has_key("attachments")) {
        FRPC::Array_t &attachmentArray(FRPC::Array(attributes["attachments"]));
        for (FRPC::Array_t::const_iterator iattachmentArray(attachmentArray.begin()) ;
             iattachmentArray != attachmentArray.end() ;
             ++iattachmentArray) {

            FRPC::Struct_t &attachmentStruct(FRPC::Struct(**iattachmentArray));

            Mail_t::Attachment_t attachment;

            attachment.data = FRPC::Binary(attachmentStruct["data"]);
            attachment.filename = FRPC::String(attachmentStruct["filename"]);
            if (attachmentStruct.has_key("description")) {
                attachment.description = FRPC::String(attachmentStruct["description"]);
            }

            mail.attachments.push_back(attachment);
        }
    }

    return mail;
}

void rpcToTeng(Teng::Fragment_t &fragment,
               const FRPC::Struct_t &value);

void rpcToTeng(Teng::Fragment_t &fragment,
               const std::string &name,
               const FRPC::Value_t &value)
{
    if (value.getType() == FRPC::Struct_t::TYPE) {
        rpcToTeng(
            fragment.addFragment(name),
            FRPC::Struct(value));
    } else if (value.getType() == FRPC::Array_t::TYPE) {
        const FRPC::Array_t &array(FRPC::Array(value));
        for (FRPC::Array_t::const_iterator iarray(array.begin()) ;
             iarray != array.end() ;
             ++iarray) {

            FRPC::Struct_t &item(FRPC::Struct(**iarray));
            rpcToTeng(
                fragment.addFragment(name),
                item);
        }
    } else if (value.getType() == FRPC::String_t::TYPE) {
        fragment.addVariable(name, FRPC::String(value));
    } else if (value.getType() == FRPC::Int_t::TYPE) {
        fragment.addVariable(name, long(FRPC::Int(value)));
    } else if (value.getType() == FRPC::Double_t::TYPE) {
        fragment.addVariable(name, FRPC::Double(value));
    } else if (value.getType() == FRPC::Bool_t::TYPE) {
        fragment.addVariable(name, FRPC::Bool(value) ? 1L : 0L);
    } else if (value.getType() == FRPC::DateTime_t::TYPE) {
        fragment.addVariable(name, long(FRPC::DateTime(value).getUnixTime()));
    } else {
        ERROR(ERR1, FE_ARGUMENT, "Can't send %s to Teng", value.getTypeName());
    }
}

void rpcToTeng(Teng::Fragment_t &fragment,
               const FRPC::Struct_t &value)
{
    for (FRPC::Struct_t::const_iterator ivalue(value.begin()) ;
         ivalue != value.end() ;
         ++ivalue) {

        rpcToTeng(fragment, ivalue->first, *ivalue->second);
    }
}

void templatedMailFromRpc(TemplatedMail_t &mail,
                          FRPC::Struct_t &attributes)
{
    mail.templateName = FRPC::String(attributes["templateName"]);

    FRPC::Array_t &toArray(FRPC::Array(attributes["to"]));
    for (FRPC::Array_t::const_iterator itoArray(toArray.begin()) ;
         itoArray != toArray.end() ;
         ++itoArray) {

        mail.to.push_back(FRPC::String(**itoArray));
    }

    if (attributes.has_key("cc")) {
        FRPC::Array_t &ccArray(FRPC::Array(attributes["cc"]));
        for (FRPC::Array_t::const_iterator iccArray(ccArray.begin()) ;
             iccArray != ccArray.end() ;
             ++iccArray) {

            mail.cc.push_back(FRPC::String(**iccArray));
        }
    }

    if (attributes.has_key("bcc")) {
        FRPC::Array_t &bccArray(FRPC::Array(attributes["bcc"]));
        for (FRPC::Array_t::const_iterator ibccArray(bccArray.begin()) ;
             ibccArray != bccArray.end() ;
             ++ibccArray) {

            mail.bcc.push_back(FRPC::String(**ibccArray));
        }
    }

    if (attributes.has_key("attachments")) {
        FRPC::Array_t &attachmentArray(FRPC::Array(attributes["attachments"]));
        for (FRPC::Array_t::const_iterator iattachmentArray(attachmentArray.begin()) ;
             iattachmentArray != attachmentArray.end() ;
             ++iattachmentArray) {

            FRPC::Struct_t &attachmentStruct(FRPC::Struct(**iattachmentArray));

            Mail_t::Attachment_t attachment;

            attachment.data = FRPC::Binary(attachmentStruct["data"]);
            attachment.filename = FRPC::String(attachmentStruct["filename"]);
            if (attachmentStruct.has_key("description")) {
                attachment.description = FRPC::String(attachmentStruct["description"]);
            }

            mail.attachments.push_back(attachment);
        }
    }

    rpcToTeng(mail.data, FRPC::Struct(attributes["data"]));
}

Template_t templateFromRpc(FRPC::Struct_t &attributes,
                           const bool forCreate = false)
{
    Template_t templ;

    if (attributes.has_key("name") || forCreate) {
        templ.name = FRPC::String(attributes["name"]);
        templ.mask |= Template_t::MASK_NAME;
    }
    if (attributes.has_key("from") || forCreate) {
        templ.from = FRPC::String(attributes["from"]);
        templ.mask |= Template_t::MASK_FROM;
    }
    if (attributes.has_key("subject") || forCreate) {
        templ.subject = FRPC::String(attributes["subject"]);
        templ.mask |= Template_t::MASK_SUBJECT;
    }
    if (attributes.has_key("plainBody") || forCreate) {
        templ.plainBody = FRPC::String(attributes["plainBody"]);
        templ.mask |= Template_t::MASK_PLAINBODY;
    }
    if (attributes.has_key("htmlBody") || forCreate) {
        templ.htmlBody = FRPC::String(attributes["htmlBody"]);
        templ.mask |= Template_t::MASK_HTMLBODY;
    }

    return templ;
}

TemplateListingAttributes_t templateListingAttributesFromRpc(FRPC::Struct_t &attributes)
{
    TemplateListingAttributes_t result;

    if (attributes.has_key("pageSize")) {
        result.pageSize = FRPC::Int(attributes["pageSize"]);
    }
    if (attributes.has_key("page")) {
        result.page = FRPC::Int(attributes["page"]);
    }

    return result;
}

#define DONE return pool.Struct("status", pool.Int(200), "statusMessage", pool.String("OK"))

// FastRPC method bodies:

METHOD(sendMail)
{
    params.checkItems("S");
    Mail_t mail(mailFromRpc(FRPC::Struct(params[0])));
    core.sendMail(mail);
    DONE;
}

METHOD(sendTemplatedMail)
{
    params.checkItems("S");
    TemplatedMail_t mail;
    templatedMailFromRpc(mail, FRPC::Struct(params[0]));
    core.sendTemplatedMail(mail);
    DONE;
}

METHOD(template_create)
{
    params.checkItems("S");
    Template_t templ(templateFromRpc(FRPC::Struct(params[0]), true));
    int id(core.template_create(templ));
    DONE.append("id", pool.Int(id));
}

METHOD(template_getAttributes)
{
    params.checkItems("i");
    Template_t templ(core.template_getAttributes(FRPC::Int(params[0])));
    DONE.append("templateAttributes", CONVERT(templ));
}

METHOD(template_setAttributes)
{
    params.checkItems("iS");
    Template_t templ(templateFromRpc(FRPC::Struct(params[1])));
    core.template_setAttributes(FRPC::Int(params[0]), templ);
    DONE;
}

METHOD(template_remove)
{
    params.checkItems("i");
    core.template_remove(FRPC::Int(params[0]));
    DONE;
}

METHOD(listTemplates)
{
    params.checkItems("S");
    TemplateListingAttributes_t attributes(
        templateListingAttributesFromRpc(
            FRPC::Struct(params[0])));
    std::pair<TemplateList_t, size_t> templs(
        core.listTemplates(attributes));
    DONE.append("templates", CONVERT(templs.first))
        .append("totalCount", pool.Int(templs.second));
}

METHOD(template_resolve)
{
    params.checkItems("s");
    int id(core.template_resolve(FRPC::String(params[0])));
    DONE.append("id", pool.Int(id));
}

#undef CONVERT
#undef DONE
#undef METHOD

