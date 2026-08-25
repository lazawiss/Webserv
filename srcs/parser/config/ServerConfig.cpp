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
const std::string& ServerConfig::getServerName() const
{
    return _server_name;
}

void ServerConfig::setServerName(const std::string& name)
{
    _server_name = name;
}

// ── locations ───────────────────────────────────────────────────────────────
const std::vector<LocationConfig>& ServerConfig::getLocations() const
{
    return _locations;
}

std::vector<LocationConfig>& ServerConfig::getLocations()
{
    return _locations;
}

void ServerConfig::addLocation(const LocationConfig& location)
{
    _locations.push_back(location);
}