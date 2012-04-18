
#ifndef FRPCINTERFACE_H
#define FRPCINTERFACE_H

#include "core.h"

#define METHOD(name) \
    FRPC::Value_t& name(FRPC::Pool_t &pool, FRPC::Array_t &params)

class FrpcInterface_t {
public:
    FrpcInterface_t(ThreadServer::CppFrpcHandler_t &handler, Core_t &core);
    void threadCreate();
    void threadDestroy();

    METHOD(sendMail);
    METHOD(sendTemplatedMail);

    METHOD(template_create);
    METHOD(template_getAttributes);
    METHOD(template_setAttributes);
    METHOD(template_remove);
    METHOD(listTemplates);
    METHOD(template_resolve);

private:
    void registerMethods();

    ThreadServer::CppFrpcHandler_t &handler;
    Core_t &core;
};

#undef METHOD

#endif // FRPCINTERFACE_H

