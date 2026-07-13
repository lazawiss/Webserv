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
    std::string                     _listen;
    std::vector<std::string>        _server_names; 
    std::vector<LocationConfig>     _locations;

public:
    // ── Orthodox canonical form ─────────────────────────────────────────────
    ServerConfig();
    ServerConfig(const ServerConfig &ref);
    ServerConfig& operator=(const ServerConfig &ref);
    ~ServerConfig();

    // ── Getters ─────────────────────────────────────────────────────────────
    const std::string&                  getListen()         const;
    const std::vector<std::string>&     getServerNames()    const;
    const std::vector<LocationConfig>&  getLocations()      const;
    std::vector<LocationConfig>&        getLocations();

    // ── Setters ─────────────────────────────────────────────────────────────
    void setListen(const std::string& listen);
    void addServerName(const std::string& name);
    void addLocation(const LocationConfig& location);
};

#endif