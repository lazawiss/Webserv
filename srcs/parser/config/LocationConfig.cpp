#include "LocationConfig.hpp"

/*
** ============================================================================
** LocationConfig - Constructors & Destructor
** ============================================================================
*/

// LocationConfig::LocationConfig() : AConfig() , _return_code(0) {}
LocationConfig::LocationConfig() : AConfig() {}

LocationConfig::~LocationConfig() {}


/*
** ============================================================================
** LocationConfig - Getters & Setter
** ============================================================================
*/

const std::string& LocationConfig::getPath() const
{
    return _path;
}

const std::vector<std::string>& LocationConfig::getMethods() const
{
    return _methods;
}

// const std::string& LocationConfig::getUploadStore() const
// {
//     return _upload_store;
// }

// int LocationConfig::getReturnCode() const
// {
//     return _return_code;
// }

// const std::string& LocationConfig::getReturnUri() const
// {
//     return _return_uri;
// }


void LocationConfig::setPath(const std::string& path)
{
    _path = path;
}

void LocationConfig::addMethod(const std::string& method) 
{
    _methods.push_back(method);
}

// void LocationConfig::setUploadStore(const std::string& path)
// {
//     _upload_store = path;
// }

// void LocationConfig::setReturn(int code, const std::string& uri)
// {
//     _return_code = code;
//     _return_uri  = uri;
// }