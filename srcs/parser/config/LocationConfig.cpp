#include "LocationConfig.hpp"

/*
** ============================================================================
** LocationConfig - Orthodox canonical form
** ============================================================================
*/

LocationConfig::LocationConfig() : AConfig() {}

LocationConfig::LocationConfig(const LocationConfig &ref) : AConfig(ref),
    _path(ref._path), _methods(ref._methods), _cgi_extension(ref._cgi_extension) {}

LocationConfig& LocationConfig::operator=(const LocationConfig &ref)
{
    if (this != &ref)
    {
        AConfig::operator=(ref);
        _path          = ref._path;
        _methods       = ref._methods;
        _cgi_extension = ref._cgi_extension;
    }
    return *this;
}

LocationConfig::~LocationConfig() {}


/*
** ============================================================================
** LocationConfig - Getters & Setter
** ============================================================================
*/

// ── path ────────────────────────────────────────────────────────────────────
const std::string& LocationConfig::getPath() const
{
    return _path;
}

void LocationConfig::setPath(const std::string &path)
{
    _path = path;
}

// ── methods ─────────────────────────────────────────────────────────────────
const std::vector<std::string>& LocationConfig::getMethods() const
{
    return _methods;
}

void LocationConfig::addMethod(const std::string &method) 
{
    _methods.push_back(method);
}

// ── CGI ────────────────────────────────────────────────────────────────────
const std::map<std::string, std::string>& LocationConfig::getMap() const
{
    return _cgi_extension;
}

void LocationConfig::addMap(const std::string &key, const std::string &value)
{
    _cgi_extension.insert(std::make_pair(key, value));
}
