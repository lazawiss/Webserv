#include "GlobalConfig.hpp"

/*
** ============================================================================
** GlobalConfig - Constructors & Destructor
** ============================================================================
*/

GlobalConfig::GlobalConfig() : AConfig() {}

GlobalConfig::~GlobalConfig() {}

/*
** ============================================================================
** GlobalConfig - Getters & Setter
** ============================================================================
*/

const std::vector<ServerConfig>& GlobalConfig::getServers() const
{
    return _servers;
}

void GlobalConfig::addServer(const ServerConfig& server)
{
    _servers.push_back(server);
}