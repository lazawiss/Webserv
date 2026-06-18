#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "AConfig.hpp"
# include "LocationConfig.hpp"

# include <iostream>
# include <string>
# include <map>
# include <vector>

/*
** ============================================================================
** ServerConfig - Class
** ============================================================================
*/

class ServerConfig : public AConfig
{
private:
    std::vector<std::string>        _listen;
    std::vector<std::string>        _server_names; 
    // have to make sure server names are individual, 
    // duplicate across blocks not allowed
    std::vector<LocationConfig>     _locations;
    int                             _returnCode;
    std::string                     _returnValue;
    // return lives in location and/or server: specifies value to send to client
    // value can contain text, variables, and their combination
    // return for server for this server_name (old-domain.com) 
    // means everything hitting this, no matter what path, 
    // send this response

public:
    // ── Orthodox canonical form ─────────────────────────────────────────────
    ServerConfig();
    ServerConfig(const ServerConfig &ref);
    ServerConfig& operator=(const ServerConfig &ref);
    ~ServerConfig();

    // ── Getters ─────────────────────────────────────────────────────────────
    const std::vector<std::string>&     getListen()         const;
    const std::vector<std::string>&     getServerNames()    const;
    const std::vector<LocationConfig>&  getLocations()      const;

    // ── Setters ─────────────────────────────────────────────────────────────
    void addListen(const std::string& listen);
    void addServerName(const std::string& name);
    void addLocation(const LocationConfig& location);
};

#endif