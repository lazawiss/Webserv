#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

# include "AConfig.hpp"
# include "LocationConfig.hpp"

# include <iostream>
# include <string>
# include <map>

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
    std::vector<LocationConfig>     _locations;

public:
    ServerConfig();
    ~ServerConfig();

    // ── Getters ───────────────────────────────────────────
    const std::vector<std::string>&     getListen()          const;
    const std::vector<std::string>&     getServerNames()    const;
    const std::vector<LocationConfig>&  getLocations()      const;

    // ── Setters ───────────────────────────────────────────
    void addListen(const std::string& listen);
    void addServerName(const std::string& name);
    void addLocation(const LocationConfig& location);
};

#endif