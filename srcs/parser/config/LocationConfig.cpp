#include "LocationConfig.hpp"

/*
** ============================================================================
** LocationConfig - Orthodox canonical form
** ============================================================================
*/

LocationConfig::LocationConfig() : AConfig() {}

LocationConfig::LocationConfig(const LocationConfig &ref) : AConfig(ref),
    _path(ref._path), _methods(ref._methods) {}

LocationConfig& LocationConfig::operator=(const LocationConfig &ref)
{
    if (this != &ref)
    {
        AConfig::operator=(ref);
        _path         = ref._path;
        _methods      = ref._methods;
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