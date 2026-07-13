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
    std::string                         _upload;
    std::map<int, std::string>          _return;
    std::map<std::string, std::string>  _cgi_extension;

public:
    // ── Orthodox canonical form ─────────────────────────────────────────────
    LocationConfig();
    LocationConfig(const LocationConfig &ref);
    LocationConfig& operator=(const LocationConfig &ref);
    ~LocationConfig();

    // ── Getters ─────────────────────────────────────────────────────────────
    const std::string&                        getPath()        const;
    const std::vector<std::string>&           getMethods()     const;
    const std::string&                        getUpload()      const;
    const std::map<int, std::string>&         getReturn()      const;
    const std::map<std::string, std::string>& getMap()         const;

    // ── Setters ─────────────────────────────────────────────────────────────
    void setPath(const std::string &path);
    void addMethod(const std::string &method);
    void setUpload(const std::string &upload);
    void addReturn(int code, const std::string &uri);
    void addMap(const std::string &key, const std::string &value);
};

#endif