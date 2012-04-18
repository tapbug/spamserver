
#include <dbglog.h>
#include "server.h"
#include "version.h"

Server_t::Server_t()
  : core(0),
    frpcInterface(0)
{
    LOG(INFO4, "Spamserver mailser server version %s starting", PACKAGE_VERSION);
}

Server_t::~Server_t()
{
    frpcInterface.reset();
    core.reset();
}

void Server_t::threadCreate()
{
    core->threadCreate();
}

void Server_t::threadDestroy()
{
    core->threadDestroy();
}

FrpcServer_t::FrpcServer_t(ThreadServer::CppFrpcHandler_t &handler)
  : ThreadServer::CppFrpcHandler_t::Module_t(&handler),
    handler(handler)
{
}

FrpcServer_t::~FrpcServer_t()
{
}

void FrpcServer_t::threadCreate()
{
    server.threadCreate();
    server.frpcInterface->threadCreate();
}

void FrpcServer_t::threadDestroy()
{
    server.frpcInterface->threadDestroy();
    server.threadDestroy();
}

extern "C" {
    Server_t server;

    FrpcServer_t* frpc(ThreadServer::CppFrpcHandler_t *handler)
    {
        if (!server.core.get()) {
            server.core.reset(new Core_t(*handler->threadServer));
        }
        server.frpcInterface.reset(new FrpcInterface_t(*handler, *server.core));
        FrpcServer_t *frpcServer = new FrpcServer_t(*handler);
        return frpcServer;
    }
}

