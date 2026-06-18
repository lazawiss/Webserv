#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include "AConfig.hpp"

# include <iostream>
# include <string>
# include <vector>

/*
** ============================================================================
** AConfig - Class
** ============================================================================
*/

class LocationConfig : public AConfig
{
private:
    std::string                         _path;
    std::vector<std::string>            _methods;
    int                                 _returnCode;
    std::string                         _returnValue;
    // for location, only requests matching this specific path 
    // gets this response

public:
    // ── Orthodox canonical form ─────────────────────────────────────────────
    LocationConfig();
    LocationConfig(const LocationConfig &ref);
    LocationConfig& operator=(const LocationConfig &ref);
    ~LocationConfig();

    // ── Getters ─────────────────────────────────────────────────────────────
    const std::string&                        getPath()        const;
    const std::vector<std::string>&           getMethods()     const;

    // ── Setters ─────────────────────────────────────────────────────────────
    void setPath(const std::string &path);
    void addMethod(const std::string &method);
};

#endif