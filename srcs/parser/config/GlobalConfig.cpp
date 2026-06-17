#include "GlobalConfig.hpp"

/*
** ============================================================================
** GlobalConfig - Orthodox canonical form
** ============================================================================
*/

GlobalConfig::GlobalConfig() : AConfig() {}

GlobalConfig::GlobalConfig(const GlobalConfig &ref) : AConfig(ref),
    _servers(ref._servers) {}

GlobalConfig& GlobalConfig::operator=(const GlobalConfig &ref)
{
    if (this != &ref)
    {
        AConfig::operator=(ref);
        _servers = ref._servers;
    }
    return *this;
}

GlobalConfig::~GlobalConfig() {}

/*
** ============================================================================
** GlobalConfig - Getters & Setters
** ============================================================================
*/

// ── servers ─────────────────────────────────────────────────────────────────
const std::vector<ServerConfig>& GlobalConfig::getServers() const
{
    return _servers;
}

void GlobalConfig::addServer(const ServerConfig &server)
{
    _servers.push_back(server);
}