#include "ServerConfig.hpp"

/*
** ============================================================================
** ServerConfig - Constructors & Destructor
** ============================================================================
*/

ServerConfig::ServerConfig(): AConfig() {}

ServerConfig::~ServerConfig() {}

/*
** ============================================================================
** ServerConfig - Getters & Setter
** ============================================================================
*/

const std::vector<std::string>& ServerConfig::getListen() const
{
    return _listen;
}

const std::vector<std::string>& ServerConfig::getServerNames() const
{
    return _server_names;
}

const std::vector<LocationConfig>& ServerConfig::getLocations() const
{
    return _locations;
}


void ServerConfig::addListen(const std::string& listen)
{
    _listen.push_back(listen);
}

void ServerConfig::addServerName(const std::string& name)
{
    _server_names.push_back(name);
}

void ServerConfig::addLocation(const LocationConfig& location)
{
    _locations.push_back(location);
}