#include "AConfig.hpp"

/*
** ============================================================================
** AConfig - Constructors & Destructor
** ============================================================================
*/

// Note Delphine : une string, un vector et une map se contruisent vides
// automatiquement en CPP98, pas besoin de les initialiser explicitement !!
AConfig::AConfig() : _root(""), _autoindex(false), _client_max_body_size(0) {}

AConfig::~AConfig() {}

/*
** ============================================================================
** AConfig - Getters & Setter
** ============================================================================
*/

const std::string& AConfig::getRoot() const
{
    return _root;
}

const std::vector<std::string>& AConfig::getIndex() const
{
    return _index;
}

const std::map<int, std::string>& AConfig::getErrorPages() const
{
    return _error_pages;
}

bool AConfig::getAutoindex() const
{
    return _autoindex;
}

size_t AConfig::getClientMaxBodySize() const
{
    return _client_max_body_size;
}


void AConfig::setRoot(const std::string& root)
{
    _root = root;
}
void AConfig::addIndex(const std::string& index)
{
    _index.push_back(index);
}
void AConfig::setAutoindex(bool autoindex)
{
    _autoindex = autoindex;
}

void AConfig::setClientMaxBodySize(size_t size)
{
    _client_max_body_size = size;
}

void AConfig::addErrorPage(int code, const std::string& uri)
{
    _error_pages[code] = uri;
}