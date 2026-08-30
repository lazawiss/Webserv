#include "AConfig.hpp"

/*
** ============================================================================
** AConfig - Orthodox canonical form
** ============================================================================
*/

// Sets max allowed size of the client request body 
// _client_max_body_size is 1m if never set
// this is why we hit 413 errors
// 0 disables the check entirely
AConfig::AConfig() : _root(""), _autoindex(""),
    _client_max_body_size("") {}

AConfig::AConfig(const AConfig &ref) : _root(ref._root), _index(ref._index),
    _error_pages(ref._error_pages), _autoindex(ref._autoindex),
    _client_max_body_size(ref._client_max_body_size) {}

AConfig& AConfig::operator=(const AConfig &ref)
{
    if (this != &ref)
    {
        _root = ref._root;
        _index = ref._index;
        _error_pages = ref._error_pages;
        _autoindex = ref._autoindex;
        _client_max_body_size = ref._client_max_body_size;
    }
    return *this;
}

AConfig::~AConfig() {}

/*
** ============================================================================
** AConfig - Getters & Setter
** ============================================================================
*/

// ── root ────────────────────────────────────────────────────────────────────
const std::string& AConfig::getRoot() const
{
    return _root;
}

void AConfig::setRoot(const std::string &root)
{
    _root = root;
}

// ── index ───────────────────────────────────────────────────────────────────
const std::vector<std::string>& AConfig::getIndex() const
{
    return _index;
}

void AConfig::addIndex(const std::string &index)
{
    _index.push_back(index);
}

// ── error_pages ────────────────────────────────────────────────────────────
const std::map<int, std::string>& AConfig::getErrorPages() const
{
    return _error_pages;
}

void AConfig::addErrorPage(int code, const std::string &uri)
{
    _error_pages[code] = uri;
}

// ── autoindex ──────────────────────────────────────────────────────────────
const std::string& AConfig::getAutoindex() const
{
    return _autoindex;
}

void AConfig::setAutoindex(const std::string &autoindex)
{
    _autoindex = autoindex;
}

// ── client_max_body_size ──────────────────────────────────────────────────
const std::string& AConfig::getClientMaxBodySize() const
{
    return _client_max_body_size;
}

void AConfig::setClientMaxBodySize(const std::string &size)
{
    _client_max_body_size = size;
}