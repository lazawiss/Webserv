#ifndef GLOBALCONFIG_HPP
# define GLOBALCONFIG_HPP

# include "AConfig.hpp"
# include "ServerConfig.hpp"

# include <vector>

/*
** ============================================================================
** AConfig - Class
** ============================================================================
*/
class GlobalConfig : public AConfig
{
private:
    std::vector<ServerConfig> _servers;

public:
    // ── Orthodox canonical form ─────────────────────────────────────────────
    GlobalConfig();
    GlobalConfig(const GlobalConfig &ref);
    GlobalConfig& operator=(const GlobalConfig &ref);
    ~GlobalConfig();

    // ── Getters ─────────────────────────────────────────────────────────────
    const std::vector<ServerConfig>& getServers() const;

    // ── Setters ─────────────────────────────────────────────────────────────
    void addServer(const ServerConfig &server);

};

#endif