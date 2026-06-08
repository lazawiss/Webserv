#ifndef GLOBALCONFIG_HPP
# define GLOBALCONFIG_HPP

# include "ServerConfig.hpp"

# include <vector>

class GlobalConfig
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