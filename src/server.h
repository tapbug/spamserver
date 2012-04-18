
#ifndef SERVER_H
#define SERVER_H

#include <boost/thread/shared_mutex.hpp>
#include <threadserver/handlers/cppfrpchandler/cppfrpchandler.h>
#include "core.h"
#include "frpcinterface.h"

class Server_t {
public:
    Server_t();
    ~Server_t();
    void threadCreate();
    void threadDestroy();

    std::auto_ptr<Core_t> core;
    std::auto_ptr<FrpcInterface_t> frpcInterface;
};

class FrpcServer_t : ThreadServer::CppFrpcHandler_t::Module_t {
public:
    FrpcServer_t(ThreadServer::CppFrpcHandler_t &handler);
    virtual ~FrpcServer_t();
    virtual void threadCreate();
    virtual void threadDestroy();

protected:
    ThreadServer::CppFrpcHandler_t &handler;
};

extern "C" {
    extern Server_t server;
    FrpcServer_t* frpc(ThreadServer::CppFrpcHandler_t *handler);
}

#endif // SERVER_H

