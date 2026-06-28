#include "ServerConfig.hpp"

/*
** ============================================================================
** ServerConfig - Orthodox canonical form
** ============================================================================
*/

ServerConfig::ServerConfig(): AConfig() {}

ServerConfig::ServerConfig(const ServerConfig &ref) : AConfig(ref),
    _listen(ref._listen), _server_names(ref._server_names),
    _locations(ref._locations) {}

ServerConfig& ServerConfig::operator=(const ServerConfig &ref)
{
    if (this != &ref)
    {
        AConfig::operator=(ref);
        _listen = ref._listen;
        _server_names = ref._server_names;
        _locations = ref._locations;
    }
    return *this;
}

ServerConfig::~ServerConfig() {}

/*
** ============================================================================
** ServerConfig - Getters & Setter
** ============================================================================
*/

// ── listen ──────────────────────────────────────────────────────────────────
const std::string& ServerConfig::getListen() const
{
    return _listen;
}

void ServerConfig::setListen(const std::string& listen)
{
    _listen = listen;
}

// ── server names ────────────────────────────────────────────────────────────
const std::vector<std::string>& ServerConfig::getServerNames() const
{
    return _server_names;
}

void ServerConfig::addServerName(const std::string& name)
{
    _server_names.push_back(name);
}

// ── locations ───────────────────────────────────────────────────────────────
const std::vector<LocationConfig>& ServerConfig::getLocations() const
{
    return _locations;
}

void ServerConfig::addLocation(const LocationConfig& location)
{
    _locations.push_back(location);
}