#include "AConfig.hpp"

/*
** ============================================================================
** AConfig - Constructors & Destructor
** ============================================================================
*/

AConfig::AConfig() : _root(""), _autoindex(false), _client_max_body_size(0) {}

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

void AConfig::setRoot(const std::string& root)
{
    _root = root;
}

// ── index ───────────────────────────────────────────────────────────────────
const std::vector<std::string>& AConfig::getIndex() const
{
    return _index;
}

void AConfig::addIndex(const std::string& index)
{
    _index.push_back(index);
}

// ── error_pages ────────────────────────────────────────────────────────────
const std::map<int, std::string>& AConfig::getErrorPages() const
{
    return _error_pages;
}

void AConfig::addErrorPage(int code, const std::string& uri)
{
    _error_pages[code] = uri;
}

// ── autoindex ──────────────────────────────────────────────────────────────
bool AConfig::getAutoindex() const
{
    return _autoindex;
}

void AConfig::setAutoindex(bool autoindex)
{
    _autoindex = autoindex;
}

// ── client_max_body_size ──────────────────────────────────────────────────
size_t AConfig::getClientMaxBodySize() const
{
    return _client_max_body_size;
}

void AConfig::setClientMaxBodySize(size_t size)
{
    _client_max_body_size = size;
}