/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 17:20:07 by lzannis           #+#    #+#             */
/*   Updated: 2026/07/26 14:02:46 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lexer/Lexer.hpp"
#include "../parser/config/ServerConfig.hpp"
#include "HTTPParser.hpp"
#include "Server.hpp"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

HTTPParser:: HTTPParser(  std::string const & request,  const ServerConfig &serverConfig) : _request(request),
    _serverConfig(serverConfig), _code(), _type(), _method(), _requesttarget(), _httpversion(),  _boundary(), 
    _fileLength(), _fileName(), _fileBuf(), _errors(false), _upload(false), _isCGI(false), _fullPath(), _query_string(), 
    _scriptFilename(), _body(), _content_type(), _content_length(),  _content_int(0){}

HTTPParser::HTTPParser( HTTPParser const & src ) : _request(src._request), _serverConfig(src._serverConfig),
_code(src._code), _type(src._type), _method(src._method), _requesttarget(src._requesttarget),
_httpversion(src._httpversion), _boundary(src._boundary), _fileLength(src._fileLength), _fileName(src._fileName), 
_fileBuf(src._fileBuf), _errors(src._errors), _upload(src._upload), _isCGI(src._isCGI), _fullPath(src._fullPath), _query_string(src._query_string),
_scriptFilename(src._scriptFilename), _body(src._body), _content_type(src._content_type), _content_length(src._content_length),
_content_int(src._content_int){}


HTTPParser::~HTTPParser(){
    
}

HTTPParser &    HTTPParser::operator=( HTTPParser const & other ){

    if (this != &other ){

        this->_request = other._request;
        this->_code = other._code;
        this->_type = other._type;
        this->_method = other._method;
        this->_requesttarget = other._requesttarget;
        this->_httpversion = other._httpversion;
        this->_boundary = other._boundary;
        this->_fileLength = other._fileLength;
        this->_fileName = other._fileName;
        this->_fileBuf = other._fileBuf;
        this->_errors = other._errors;
        this->_upload = other._upload;
        this->_isCGI = other._isCGI;
        this->_fullPath = other._fullPath;
        this->_query_string = other._query_string;
        this->_scriptFilename = other._scriptFilename;
        this->_body = other._body;
        this->_content_type = other._content_type;
        this->_content_length = other._content_length;
        this->_content_int = other._content_int;
        this->_autoindexOn = other._autoindexOn;
        this->_rangeHeader = other._rangeHeader;
    }

    return *this;
}

std::string HTTPParser::getCode() const{
        
    return _code;
}

std::string HTTPParser::getType() const{
        
    return _type;
}

std::string HTTPParser::getMethod() const{
    
    return _method;
}

std::string HTTPParser::getBoundary() const{
        
    return _boundary;
}

std::string HTTPParser::getFileName() const{
        
    return _fileName;
}

std::string HTTPParser::getFileBuf() const{
        
    return _fileBuf;
}

bool     HTTPParser::getError() const{
    
    return _errors;
}

bool     HTTPParser::getUpload() const{
    
    return _upload;
}

std::string HTTPParser::setCode( std::string const & code ){
    
    _code = code;
    return _code;
}

std::string HTTPParser::setType( std::string const & type ){

    _type = type;
    return _type;
}

bool HTTPParser::setError(bool error){
    
    _errors = error;
    return _errors;
}


/*
** ============================================================================
** Parser HTTP - First Part: GET
** ============================================================================
*/

// CHECK REQUEST:
// -SIZE
// -METHOD
// -HOST
// making sure the request have the minimum requirement to execute the static part with GET 
// New Logic Parser Without Token :
// Look for spaces +store them in vector<size_t> : collectSpace()
// Then look at the strings in between those spaces : loop on vector<> space_inter : collectString()
// Assign string to variables after checks 

bool    HTTPParser::checkSize(){
    
    if (_request.size() > BUF_SIZE){
        
        _errors = true;
        _code = "413";
        _type = "text/html";

        return false;
    }

    return true;
}

static bool isMethod( std::string const & str ){
    
    return str == "GET" || str == "POST" || str == "DELETE";
}

// static bool isDelimiter( std::string const & t ){
    
//     return t == "\r\n\r\n";
// }

// static bool isBoundary( Token const & t, std::string const & boundary ){
    
//     return t.value == boundary;
// }

static bool isSimpleSpace( int found ){
    
    return std::isspace(static_cast<unsigned char>(found));
}

void HTTPParser::extractBody()
{
    size_t headerEnd = _request.find("\r\n\r\n");
    size_t sepLen = 4;
    if (headerEnd == std::string::npos)
    {
        headerEnd = _request.find("\n\n");
        sepLen = 2;
    }
    if (headerEnd != std::string::npos)
        _body = _request.substr(headerEnd + sepLen);
}


//Collect all the spaces on 1 line
// Store them in vector<size_t>
std::vector<size_t>  HTTPParser::collectSpace( std::string::iterator pos ){
    
    std::vector<size_t> space_inter;
    
    while(pos != _request.end()){
        
        pos = find_if(pos, _request.end(), isSimpleSpace);
        if (pos == _request.end())
            break;
            
        space_inter.push_back(distance(_request.begin(), pos));
        if (*pos == '\n')
            break;

        pos++;
    }
    
    _pos = pos;

    return space_inter;
}

// Based on position of spaced previously collected, collect strings
// Store them in vector<std::string>
std::vector<std::string>  HTTPParser::collectString(  std::vector<size_t> space_inter ){

    std::vector<std::string> subss;
    
    for(size_t i = 0; i < space_inter.size(); ++i){
        
        size_t start = (i == 0) ? 0 : space_inter[i - 1] + 1;
        size_t end = space_inter[i];

        subss.push_back(_request.substr(start, end - start));
    }

    return subss;
}


// Check if it respect the standard form :
// request-line   = method SP request-target SP HTTP-version
bool    HTTPParser::checkRequestLine(){
    
    std::string::iterator space = _request.begin();
    
    std::vector<size_t> space_inter = collectSpace(space);

    std::vector<std::string> subss = collectString(space_inter);
 
    if (!subss.empty()){
        
        _method = subss[0];
        
        if (isMethod(_method))
            LOG_DEBUG("Method: " + _method);
        else{
            
            _errors = true;
            _code = "405";
            _type = "text/html";
            
            return false;
        }
        
        _requesttarget = subss[1];
        
        char const *slash = strrchr( _requesttarget.c_str(), '/');
        if (slash)
            LOG_DEBUG("RequestTarget: " + _requesttarget);

        _httpversion = subss[2];
        if ( _httpversion == "HTTP/1.1")
            LOG_DEBUG("HTTP version: " + _httpversion);
     
        return true;
    }
    _errors = true;
    _code = "400";
    _type = "text/html";
            
    return false;
}

// Check if the entry Host: correspond to the config file info
//or if it exist at all
bool    HTTPParser::checkHost( ListenerManager const & listener ){

    std::string hostname = listener.getNode();
    hostname += ":";
    hostname += listener.getService();

    LOG_DEBUG("Expected hostname: " + hostname);

    _pos++;
   
    std::vector<size_t> space_inter = collectSpace(_pos);

    std::vector<std::string> subss = collectString(space_inter);

    if (subss.size() > 1){
            
        char const *doublePoint = strrchr(subss[1] .c_str(), ':');
        std::string service = std::string(doublePoint , strlen(doublePoint));
        service.erase(service.begin());

        if (subss[1] == hostname || (listener.getNode() == "0.0.0.0" && listener.getService() == service) ){
            
            return true;
        }
        LOG_DEBUG("Host mismatch, got: " + subss[1]);
        _errors = true;
        _code = "421";
        _type = "text/html";
        
        return false;
        
    }
    _errors = true;
    _code = "400";
    _type = "text/html";
    
    return false;
}

bool    HTTPParser::isRequestValid( ListenerManager const & listen ){
    
    if (checkSize() == false){
        LOG_ERROR("Request size exceeds limit");
        return false;
    }

    if (checkRequestLine() == false){
        LOG_ERROR("Invalid request line");
        return false;
    }

    // check for root because need full path
    buildFullPath(); // resolve _fullPath + _autoindexOn from the matched location
    extractRange();  // capture "Range:" header for 206 Partial Content, if present
    extractBody();
    parseCGI();
    
    if (checkHost(listen) == false){
        LOG_ERROR("Host header invalid or missing");
        return false;
    }
    
    return true;
}

/* --------- CGI PARSING INCLUSION ------------*/

void    HTTPParser::parseCGI(){
        // GET /cgi-bin/hello.py?name=andi HTTP/1.1
    if (_requesttarget.find("/cgi-bin/") == std::string::npos)
    {
        _isCGI = false;
        return;
    }

    size_t pos = _requesttarget.find('?');
    if (pos != std::string::npos)
    {
        _scriptFilename = _requesttarget.substr(0, pos);
        _query_string = _requesttarget.substr(pos + 1);
    }
    else
    {
        _scriptFilename = _requesttarget;
        _query_string = "";
    }
    _isCGI = true;
    return;
}

bool    HTTPParser::isCGI() const{
    return _isCGI;
}

bool HTTPParser::validateCGIRequest()
{
    if (_method != "GET" && _method != "POST" && _method != "DELETE")
    {
        _code = "405";
        return false;
    }
    if (_method == "POST")
    {
        if (!checkContentType())  { LOG_ERROR("CGI POST: missing Content-Type"); return false; }
        if (!checkContentLength()){ LOG_ERROR("CGI POST: missing Content-Length"); return false; }
    }
    _code = "cgi";
    return true;
}

std::string     HTTPParser::getScriptFilename() const {
    return _scriptFilename;
}

std::string     HTTPParser::getQueryString() const {
    return _query_string;
}
std::string     HTTPParser::getBody() const{
    return _body;
}
std::string     HTTPParser::getContentType() const{
    return _content_type;
}
std::string     HTTPParser::getContentLength() const{
    return _content_length;
}
std::string     HTTPParser::getRequestTarget() const{
    return _requesttarget;
}

std::string     HTTPParser::getPath() const{
    return _fullPath;
}

const LocationConfig*   HTTPParser::matchLocation() const{

    const std::vector<LocationConfig> &locs = _serverConfig.getLocations();
    const LocationConfig *best = NULL;
    size_t bestLen = 0;

    for (size_t i = 0; i < locs.size(); ++i){
        const std::string &path = locs[i].getPath();
        if (_requesttarget.compare(0, path.size(), path) == 0 && path.size() >= bestLen){
            bestLen = path.size();
            best = &locs[i];
        }
    }
    return best;
}

// root + full URI = for auto index
void    HTTPParser::buildFullPath(){

    const LocationConfig *loc = matchLocation();

    std::string root = (loc && !loc->getRoot().empty()) ? loc->getRoot()
                                                         : _serverConfig.getRoot();
    _autoindexOn = (loc && loc->getAutoindex() == "on");

    _fullPath = root;
    if (!_fullPath.empty() && _fullPath[_fullPath.size() - 1] == '/'
        && !_requesttarget.empty() && _requesttarget[0] == '/')
        _fullPath.erase(_fullPath.size() - 1);
    _fullPath += _requesttarget;

    LOG_DEBUG(std::string("[HTTPParser] _fullPath: '") + _fullPath + "' autoindex=" + (_autoindexOn ? "on" : "off"));
}

// grab the raw value of the range header! 
// _rangeHeader reste empty when the header is absent
void    HTTPParser::extractRange(){

    size_t start = _request.find("Range:");
    if (start == std::string::npos)
        return;
    start += 6;
    size_t end = _request.find("\r\n", start);
    if (end == std::string::npos)
        end = _request.find("\n", start);
    if (end == std::string::npos)
        return;
    _rangeHeader = _request.substr(start, end - start);
    size_t nonSpace = _rangeHeader.find_first_not_of(" \t"); // unsure butttt
    if (nonSpace == std::string::npos)
        _rangeHeader.clear();
    else
        _rangeHeader = _rangeHeader.substr(nonSpace);
    LOG_DEBUG("[HTTPParser] Range: '" + _rangeHeader + "'");
}

std::string     HTTPParser::getRange() const{
    return _rangeHeader;
}

/* --------- CGI PARSING INCLUSION ------------*/
/*
** ============================================================================
** Parser HTTP - Second Part: POST
** ============================================================================
*/

//Parsing the minimum requirement to upload file with POST:
// - CONTENT-TYPE
// - CONTENT-LENGTH
// - CONTENT-DISPOSITION :
//      - TYPE : ex: form-data
//      - NAME : part of the form that collects uploaded file
//      - FILENAME : NAME OF THE UPLOADED FILE

//Beginning of request POST : 
// Content-Type: content-type; boundary=----[...]
bool    HTTPParser::checkContentType(){

    size_t start = _request.find("Content-Type:"); 
    std::string::iterator space = _request.begin() + start;
    
    std::vector<size_t> space_inter = collectSpace(space);

    std::vector<std::string> subss = collectString(space_inter);

    if (!subss.empty()){
        
        _type = subss[1];
        _type.erase(_type.end() - 1);
        LOG_DEBUG("Content-Type: " + _type);
        _boundary = subss[2];
        if (_isCGI)
            return true;
        _boundary.erase(_boundary.begin(),_boundary.begin() + 9);
        LOG_DEBUG("boundary: " + _boundary);
        
        return true;
    }
    _errors = true;
    _code = "400";
    _type = "text/html";
    
    return false;
}

bool    HTTPParser::checkContentLength(){

    _pos++;
    std::string::iterator space = _pos;
    
    std::vector<size_t> space_inter = collectSpace(space);

    std::vector<std::string> subss = collectString(space_inter);
 
     if (!subss.empty()){
        
        _fileLength = subss[1];
        std::stringstream ss(_fileLength);
        if (_isCGI)
        {
            int len;
            ss >> len;
            LOG_DEBUG("Content-Length: " + _fileLength);
            if (len > BUF_SIZE){
                LOG_ERROR("File size exceeds limit");
                return false;
            }
            _content_int = len;
            return true;
        }

        size_t len;
        ss >> len;
        LOG_DEBUG("Content-Length: " + _fileLength);
        if (len > BUF_SIZE){
            LOG_ERROR("File size exceeds limit");
            return false;
        }
        return true;
     }
    _errors = true;
    _code = "400";
    _type = "text/html";
    
    return false;
}


bool    HTTPParser::checkContentDisposition(){
    
    size_t pos = _request.find("Content-Disposition:");
    
    std::string::iterator newpos = _request.begin() + pos;

    std::vector<size_t> space_inter = collectSpace(newpos);

    std::vector<std::string> subss = collectString(space_inter);
     
     if (!subss.empty()){
        
            std::string type = subss[1];
            char const *slash = strchr(_type.c_str(), '/');
            std::string checktype = std::string(slash, strlen(slash));
            checktype.erase(checktype.begin());
            checktype.erase(checktype.end() - 1);
            std::string name = subss[2];
            name.erase(name.end() - 1);
            name.erase(name.begin(), name.begin() + 6);
            _fileName = subss[3];
            _fileName.erase(_fileName.end() - 1);
            _fileName.erase(_fileName.begin(), _fileName.begin() + 10 );
            LOG_DEBUG("Disposition type: " + checktype);
            LOG_DEBUG("Disposition name: " + name);
            LOG_DEBUG("Upload filename: " + _fileName);

            return true;
     }
    _errors = true;
    _code = "400";
    _type = "text/html";
    
    return false;
}


bool    HTTPParser::gatherFile(){
    
    _pos++;
    std::vector<size_t> space_inter = collectSpace(_pos);

    std::vector<std::string> subss = collectString(space_inter);

    _type = subss[1];
    LOG_DEBUG("File Content-Type: " + _type);

    _pos += 3;

    std::string endOfFile = _boundary + "--";
    size_t end_pos = _request.find(endOfFile);

    if (end_pos == std::string::npos){
        LOG_ERROR("End boundary not found in request");
        return false;
    }
    _fileBuf.assign(_pos, _request.begin() + end_pos - 4);
    return true;
}

/*
** ============================================================================
** Parser HTTP - Third Part: Dispatch per Method + Helpers
** ============================================================================
*/

// Main function findMethods() :
// Compare method found in request and in config file
// if no correspondance : error 405 method not accepted
// then get index from config file :
// if multiple, check if valide then goes to the next
// if none valid, index by default

bool HTTPParser::compareMethodWithConfigFile(){
    
    const std::vector<LocationConfig> &locs = _serverConfig.getLocations();
    if (locs.size() == 0){
        _code = "index.html";
        _type = "text/html";
        return true;
    }
    
    for (size_t i = 0;i < locs.size(); i++){
        const std::vector<std::string> &methodVector = locs[i].getMethods();
        if (methodVector.size() > 0){
            for (size_t i = 0; i < methodVector.size() ; i++){
                if ( _method == methodVector[i])
                    return true;
            }
        }
    }
    _errors = true;
    _code = "405";
    _type = "text/html";
    
    return false;
}

std::string     HTTPParser::addSuffix(std::string suffix){
    
    if (suffix  == ".jpg")
        _type = "image/jpeg";
    if (suffix  == ".png")
        _type = "image/png";
    if (suffix  == ".gif")
        _type = "image/gif";
    if (suffix  == ".webp")
        _type = "image/webp";
    if (suffix == ".txt")
        _type = "text/plain";
    if (suffix == ".html")
        _type = "text/html";
    return _type;
}


bool    HTTPParser::findMethods(){
    if (compareMethodWithConfigFile() == true)
    {
        if (_method == "GET")
        {
            const std::vector<LocationConfig> &locs = _serverConfig.getLocations();
            
            for (size_t i = 0;i < locs.size(); i++){
                const std::vector<std::string> &indexVector = locs[i].getIndex();
                if (indexVector.size() > 0){
                    for (size_t i = 0; i < indexVector.size() ; i++){
                        struct stat sb;
                        std::string index = "data/www/html/" + indexVector[i];
                        if (stat(index.c_str(), &sb) == 0){
                            _code = indexVector[i];
                            break ;
                        }
                    }
                }
                else{
                    _code = "index.html";
                }
            }

            if (_requesttarget == "/" || _requesttarget == "/api") {
                    struct stat path_stat;
                    if (stat(_fullPath.c_str(), &path_stat) == -1)
                    {
                        _errors = true;
                    if (stat(_fullPath.c_str(), &path_stat) == -1)
                    {
                        LOG_ERROR("stat failed for: " + _fullPath);
                    }
                        _code = "404"; // assuming that we are just not existing
                    _type = "text/html";
                    return false;
                    }
                    if (S_ISDIR(path_stat.st_mode)) // file type and mode, is a directory?
                    {
                        std::string indexPath = _fullPath;
                        if (indexPath[indexPath.size() - 1] != '/')
                            indexPath += "/";
                        indexPath += "index.html";
                        struct stat index_stat;
                        if (stat(indexPath.c_str(), &index_stat) == 0 && S_ISREG(index_stat.st_mode))
                        {
                            // this means that the index file exists and it is a file
                            _code = "index.html";
                            _type = "text/html";
                            return true;
                        }
                        if (_autoindexOn)
                        {
                            _code = "autoindex";
                            _type = "text/html";
                            return true;
                        }
                        // case of autoindex == off and index doesn't exist
                        _errors = true;
                        _code = "403"; // because directory is and it exists but we are not going to show you. authorization code
                        _type = "text/html";
                        return false;
                    }
            }
            if (_requesttarget.find("/images") != std::string::npos) {
                
                char const *lastPoint = strrchr(_requesttarget.c_str(), '.');
                char const *lastSlash = strrchr(_requesttarget.c_str(), '/');
                std::string name = std::string(lastSlash, strlen(lastSlash));
                name.erase(name.begin());
                _code = name;
                std::string suffix = std::string(lastPoint, strlen(lastPoint));
                _type = addSuffix(suffix);
                
                return true;
            }

            if (_requesttarget.find("/data/upload") != std::string::npos) {
                char const *lastSlash = strrchr(_requesttarget.c_str(), '/');
                std::string name = std::string(lastSlash, strlen(lastSlash));
                name.erase(name.begin());
                _code = name;
                char const *lastPoint = strrchr(_requesttarget.c_str(), '.');
                if (lastPoint){
                    
                    std::string suffix = std::string(lastPoint, strlen(lastPoint));
                    _type = addSuffix(suffix);
                }
                else
                    _type = "image/png";
                
                _upload = true;
                return true;
            }

            if (_requesttarget == "/favicon.ico"){           
                
                _code = "favicon.ico";
                _type = "image/x-icon";
                
                return true;
            }

            else{
                _code = _requesttarget;
                _code.erase(_code.begin());
                _type = "text/html";
                return true;
            }
        }

        else if (_method == "POST"){
            if (_requesttarget.find("/upload") != std::string::npos){
                
                if (_requesttarget == "/upload"){
                    
                    if (checkContentType() == false){
                        LOG_ERROR("POST upload: missing Content-Type");
                        return false;
                    }
                    if (checkContentLength() == false){
                        LOG_ERROR("POST upload: missing Content-Length");
                        return false;
                    }
                    if (checkContentDisposition() == false){
                        LOG_ERROR("POST upload: missing Content-Disposition");
                        return false;
                    }
                    if (gatherFile() == false){
                        LOG_ERROR("POST upload: failed to gather file content");
                        return false;
                    }
                    return true;
                }
                else{
                    
                    char const *lastPoint = strrchr(_requesttarget.c_str(), '.');
                    char const *lastSlash = strrchr(_requesttarget.c_str(), '/');
                    std::string name = std::string(lastSlash, strlen(lastSlash));
                    name.erase(name.begin());
                    LOG_DEBUG("POST upload target: " + name);
                    
                    _code = name; //? fichier specifique 
                    std::string suffix = std::string(lastPoint, strlen(lastPoint));
                    _type = addSuffix(suffix);
                    return true;
                }
                //on success send 201 CREATED + Location: path to ressource
                // return true;
            }
        }
        else if (_method == "DELETE"){
            
            if (_requesttarget.find("/upload") != std::string::npos){
                
                char const *lastPoint = strrchr(_requesttarget.c_str(), '.');
                char const *lastSlash = strrchr(_requesttarget.c_str(), '/');
                std::string name = std::string(lastSlash, strlen(lastSlash));
                name.erase(name.begin());
                LOG_DEBUG("DELETE target: " + name);
                
                _code = name; //? fichier specifique 
                std::string suffix = std::string(lastPoint, strlen(lastPoint));
                // _code = "200"; //? fichier specifique 
                // 204 No Content
                _type = addSuffix(suffix);
                return true;
            }
        }
        _errors = true;
        _code = "404";
        _type = "text/html";
        return false;
    }

    return false;
}
    

bool    HTTPParser::findPath(){
    return true;
}

bool    HTTPParser::findHeaders(){
    
    return true;
    
}

