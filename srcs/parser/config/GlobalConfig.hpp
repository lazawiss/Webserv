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
    GlobalConfig();
    ~GlobalConfig();

    // ── Getter ────────────────────────────────────────────
    const std::vector<ServerConfig>& getServers() const;

    // ── Setter ────────────────────────────────────────────
    void addServer(const ServerConfig& server);

};

#endif