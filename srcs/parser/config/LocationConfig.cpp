#include "LocationConfig.hpp"

/*
** ============================================================================
** Orthodox canonical form
** ============================================================================
*/

LocationConfig::LocationConfig() : AConfig() {}

LocationConfig::LocationConfig(const LocationConfig &ref) : AConfig(ref),
    _path(ref._path), _methods(ref._methods), _upload(ref._upload),
    _return(ref._return) {}

LocationConfig& LocationConfig::operator=(const LocationConfig &ref)
{
    if (this != &ref)
    {
        AConfig::operator=(ref);
        _path         = ref._path;
        _methods      = ref._methods;
        _upload       = ref._upload;
        _return       = ref._return;
    }
    return *this;
}

LocationConfig::~LocationConfig() {}


/*
** ============================================================================
** Getters & Setter
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

// ── upload ──────────────────────────────────────────────────────────────────
const std::string& LocationConfig::getUpload() const
{
    return _upload;
}

void LocationConfig::setUpload(const std::string &upload)
{
    _upload = upload;
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

// ── return ──────────────────────────────────────────────────────────────────
const std::map<int, std::string>& LocationConfig::getReturn() const
{
    return _return;
}

void LocationConfig::addReturn(int code, const std::string &uri)
{
    _return[code] = uri;
}