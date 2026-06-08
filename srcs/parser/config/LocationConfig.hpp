#ifndef LOCATIONCONFIG_HPP
# define LOCATIONCONFIG_HPP

# include "AConfig.hpp"

# include <iostream>
# include <string>
# include <map>

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
    std::string                         _upload_store;
    int                                 _return_code;
    std::string                         _return_uri;


public:
    LocationConfig();
    ~LocationConfig();

    // ── Getters ───────────────────────────────────────────
    const std::string&                        getPath()        const;
    const std::vector<std::string>&           getMethods()     const;
    const std::string&                        getUploadStore() const;
    int                                       getReturnCode()  const;
    const std::string&                        getReturnUri()   const;

    // ── Setters ───────────────────────────────────────────
    void setPath(const std::string& path);
    void addMethod(const std::string& method);
    void setUploadStore(const std::string& path);
    void setReturn(int code, const std::string& uri);
};

#endif