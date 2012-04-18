
#include "cfg.h"
#include "version.h"

Config_t::Config_t(ThreadServer::ThreadServer_t &threadServer)
  : threadServer(threadServer)
{
    smtp.address = get<std::string>("smtp.Address");
    smtp.port = get<uint16_t>("smtp.Port");
}

