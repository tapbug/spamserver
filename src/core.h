
#ifndef CORE_H
#define CORE_H

#include <threadserver/threadserver.h>
#include <threadserver/handlers/cppfrpchandler/cppfrpchandler.h>
#include <mysqlwrapper/mysqlwrapper.h>
#include <teng.h>

#include "cfg.h"
#include "magic++.h"
#include "mailer.h"
#include "types.h"

#define FE_ARGUMENT 401
#define FE_TENG 402
#define FE_FORBIDDEN 403
#define FE_NEXISTS 404
#define FE_EXISTS 405
#define FE_CONSISTENCY 406
#define FE_INTERNAL 500

class Core_t {
public:
    Core_t(ThreadServer::ThreadServer_t &handler);

    ~Core_t();

    void threadCreate();

    void threadDestroy();

    void connectDb();

    void disconnectDb();

    MySQLWrapper::MySQLWrapper_t* connectDb(const std::string &section);

    void checkDb();

    void sendMail(const Mail_t &mail);

    void sendTemplatedMail(const TemplatedMail_t &templatedMail);

    int template_create(const Template_t &templ);

    Template_t template_getAttributes(const int id);

    void template_setAttributes(const int id,
                                const Template_t &templ);

    void template_remove(const int id);

    std::pair<TemplateList_t, size_t> listTemplates(TemplateListingAttributes_t &attributes);

    int template_resolve(const std::string &name);

    ThreadServer::ThreadServer_t &threadServer;

    Config_t config;

private:
    void send(const Mail_t &mail);

    std::string render(const std::string &templateString,
                       const std::string &contentType,
                       const Teng::Fragment_t &data);

    boost::thread_specific_ptr<MySQLWrapper::MySQLWrapper_t> db;
    boost::mutex mutex;
    boost::thread_specific_ptr<Magic_t> magic;
    boost::thread_specific_ptr<Teng::Teng_t> teng;
    Mailer_t mailer;
};

#define ERROR(level, code, ...) do { \
    LOG(level, __VA_ARGS__); \
    throw ThreadServer::CppFrpcHandler_t::RpcError_t(code, __VA_ARGS__); \
} while(0)

#endif // CORE_H

