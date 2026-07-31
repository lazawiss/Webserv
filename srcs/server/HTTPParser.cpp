/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 17:20:07 by lzannis           #+#    #+#             */
/*   Updated: 2026/07/30 17:56:26 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lexer/Lexer.hpp"
#include "../parser/config/ServerConfig.hpp"

#include "HTTPParser.hpp"
#include "Server.hpp"

/*
** ============================================================================
** The Rule of Three
** ============================================================================
*/

HTTPParser::HTTPParser( std::string const & request,
    const ServerConfig & serverConfig) :
    _request(request),
    _serverConfig(serverConfig),
    _code(HTTP_INDEX), _type(), _method(),
    _requesttarget(), _httpversion(), _boundary(),
    _fileLength(), _fileName(), _fileBuf(),
    _errors(false), _upload(false), _isCGI(false),
    _fullPath(), _query_string(), _scriptFilename(),
    _body(), _content_type(), _content_length(),
    _content_int(0) {}

HTTPParser::HTTPParser( HTTPParser const & src ) :
    _request(src._request),
    _serverConfig(src._serverConfig),
    _code(src._code), _type(src._type),
    _method(src._method), _requesttarget(src._requesttarget),
    _httpversion(src._httpversion), _boundary(src._boundary),
    _fileLength(src._fileLength), _fileName(src._fileName),
    _fileBuf(src._fileBuf), _errors(src._errors),
    _upload(src._upload), _isCGI(src._isCGI),
    _fullPath(src._fullPath), _query_string(src._query_string),
    _scriptFilename(src._scriptFilename), _body(src._body),
    _content_type(src._content_type),
    _content_length(src._content_length),
    _content_int(src._content_int) {}

HTTPParser::~HTTPParser() {}

HTTPParser &    HTTPParser::operator=( HTTPParser const & other )
{
    if (this != &other )
    {
        _request        = other._request;
        _code           = other._code;
        _type           = other._type;
        _method         = other._method;
        _requesttarget  = other._requesttarget;
        _httpversion    = other._httpversion;
        _boundary       = other._boundary;
        _fileLength     = other._fileLength;
        _fileName       = other._fileName;
        _fileBuf        = other._fileBuf;
        _errors         = other._errors;
        _upload         = other._upload;
        _isCGI          = other._isCGI;
        _fullPath       = other._fullPath;
        _query_string   = other._query_string;
        _scriptFilename = other._scriptFilename;
        _body           = other._body;
        _content_type   = other._content_type;
        _content_length = other._content_length;
        _content_int    = other._content_int;
        _autoindexOn    = other._autoindexOn;
        _rangeHeader    = other._rangeHeader;
    }

    return *this;
}

/*
** ============================================================================
** Getters & Setters
** ============================================================================
*/

// ── code ────────────────────────────────────────────────────────────────────
HttpCode HTTPParser::getCode() const
{
    return _code;
}

HttpCode HTTPParser::setCode( HttpCode code )
{
    _code = code;
    return _code;
}

// ── type ────────────────────────────────────────────────────────────────────
std::string HTTPParser::getType() const
{        
    return _type;
}

std::string HTTPParser::setType( std::string const & type )
{
    _type = type;
    return _type;
}

// ── method ──────────────────────────────────────────────────────────────────
std::string HTTPParser::getMethod() const
{    
    return _method;
}

// ── boundary ────────────────────────────────────────────────────────────────
std::string HTTPParser::getBoundary() const
{        
    return _boundary;
}

// ── file name ───────────────────────────────────────────────────────────────
std::string HTTPParser::getFileName() const
{        
    return _fileName;
}

// ── file buff ───────────────────────────────────────────────────────────────
std::string HTTPParser::getFileBuf() const
{        
    return _fileBuf;
}

// ── error ───────────────────────────────────────────────────────────────────
bool     HTTPParser::getError() const
{    
    return _errors;
}

bool HTTPParser::setError(bool error)
{    
    _errors = error;
    return _errors;
}

// ── upload ──────────────────────────────────────────────────────────────────
bool     HTTPParser::getUpload() const
{ 
    return _upload;
}

// ── script filename ─────────────────────────────────────────────────────────
std::string     HTTPParser::getScriptFilename() const
{
    return _scriptFilename;
}
// ── query string ────────────────────────────────────────────────────────────
std::string     HTTPParser::getQueryString() const
{
    return _query_string;
}

// ── body ────────────────────────────────────────────────────────────────────
std::string     HTTPParser::getBody() const
{
    return _body;
}

// ── content type ────────────────────────────────────────────────────────────
std::string     HTTPParser::getContentType() const
{
    return _content_type;
}

// ── content length ──────────────────────────────────────────────────────────
std::string     HTTPParser::getContentLength() const
{
    return _content_length;
}

// ── request target ──────────────────────────────────────────────────────────
std::string     HTTPParser::getRequestTarget() const
{
    return _requesttarget;
}

// ── path ────────────────────────────────────────────────────────────────────
std::string     HTTPParser::getPath() const
{
    return _fullPath;
}

// ── range ───────────────────────────────────────────────────────────────────
std::string     HTTPParser::getRange() const
{
    return _rangeHeader;
}

/*
** ============================================================================
** Parser HTTP - First Part: GET
** ============================================================================
*/

bool HTTPParser::varNotFound400 ( size_t var ){
    
    if (var == std::string::npos)
    {
        _errors = true;
        _code = HTTP_400;
        _type = "text/html";
        
        return false;
    }
    
    return true;
}

bool  HTTPParser::doesCharCExist400 ( char const *str ){
    
    if (!str){
        
        _errors = true;
        _code = HTTP_400;
        _type = "text/html";
        
        return false;
    }
    
    return true;
}

static bool isMethod( std::string const & str ) {
    
    return str == "GET" || str == "POST" || str == "DELETE";
}

static bool isSimpleSpace( int found ){
    
    return found == ' ';
}

bool HTTPParser::checkSize() {
    
    if (_request.size() > BUF_SIZE){
        
        _errors = true;
        _code = HTTP_413;
        _type = "text/html";

        return false;
    }

    return true;
}

void HTTPParser::extractBody() {

    size_t headerEnd = _request.find("\r\n\r\n");
    size_t sepLen = 4;
    
    if (headerEnd != std::string::npos)
        _body = _request.substr(headerEnd + sepLen);
}

std::vector<size_t> HTTPParser::collectSpace( std::string::iterator start, std::string::iterator end ) {
    
    std::vector<size_t> space_inter;
    std::string::iterator pos = start;
    while(pos != end){
        
        pos = find_if(pos, end, isSimpleSpace);
        if (pos == end)
            break;
        
        space_inter.push_back(distance(start, pos));
         
        if (*pos == '\r' || *pos == '\n')
                break;
        
        pos++;
    }
    
    return space_inter;
}

std::vector<std::string> HTTPParser::collectString(std::string & line,
    std::vector<size_t> & space_inter ) {

    std::vector<std::string> subss;
    
    for(size_t i = 0; i < space_inter.size(); ++i){
        
        size_t start = (i == 0) ? 0 : space_inter[i - 1] + 1;
        size_t end = space_inter[i];

        subss.push_back(line.substr(start, end - start));
    }
    
    if (!space_inter.empty()){
        size_t separator = space_inter.back();
        if (separator < line.size())
            subss.push_back(line.substr(separator + 1));
    }

    return subss;
}

bool HTTPParser::checkRequestLine() {
    
    size_t requestLineEnd = _request.find("\r\n");
    if (varNotFound400(requestLineEnd) == false)
        return false;
        
    std::string line = _request.substr(0, requestLineEnd);
    
    std::string::iterator space = line.begin();
    std::string::iterator end = line.end();

    std::vector<size_t> space_inter = collectSpace(space, end);
 
    std::vector<std::string> subss = collectString(line, space_inter);
 
    if (subss.size() == 3) {

        _method = subss[0];
        
        if (isMethod(_method))
            LOG_DEBUG("Method: " + _method);
        else{
            
            _errors = true;
            _code = HTTP_405;
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
    _code = HTTP_400;
    _type = "text/html";

    return false;
}

bool HTTPParser::checkHost( ListenerManager const & listener ) {

    std::string hostname = listener.getNode();
    hostname += ":";
    hostname += listener.getService();

    LOG_DEBUG("Expected hostname: " + hostname);

    size_t host = _request.find("Host:");
    if (varNotFound400(host) == false)
        return false;
    size_t requestLineEnd = _request.find("\r\n", host);
    if (varNotFound400(requestLineEnd) == false)
        return false;
    std::string line = _request.substr(host, requestLineEnd - host);
    
    std::string::iterator start = line.begin();
    std::string::iterator end = line.end();

    std::vector<size_t> space_inter = collectSpace(start, end);
    
    std::vector<std::string> subss = collectString(line,space_inter);
   
    
    if (subss.size() == 2) {

        char const *doublePoint = strrchr(subss[1].c_str(), ':');
        if (doesCharCExist400(doublePoint) == false)
            return false;
        std::string service = std::string(doublePoint, strlen(doublePoint));
        service.erase(service.begin());

        if (subss[1] == hostname || (listener.getNode() == "0.0.0.0" &&
         listener.getService() == service)){
            
            return true;
        }
        LOG_DEBUG("Host mismatch, got: " + subss[1]);
        _errors = true;
        _code = HTTP_421;
        _type = "text/html";

        return false;

    }

    _errors = true;
    _code = HTTP_400;
    _type = "text/html";

    return false;
}

bool HTTPParser::isRequestValid( ListenerManager const & listen ) {
    
    if (checkSize() == false) {
        LOG_ERROR("Request size exceeds limit");
        return false;
    }

    if (checkRequestLine() == false) {
        LOG_ERROR("Invalid request line");
        return false;
    }

    if (checkHost(listen) == false) {
        LOG_ERROR("Host header invalid or missing");
        return false;
    }
    
    buildFullPath();
    extractRange();
    extractBody();
    parseCGI();
    
    return true;
}

/* --------- CGI PARSING INCLUSION ------------*/

void HTTPParser::parseCGI(){
        // GET /cgi-bin/hello.py?name=andi HTTP/1.1
    if (_requesttarget.find("/cgi-bin/") == std::string::npos)
    {
        _isCGI = false;
        return;
    }

    size_t pos = _requesttarget.find('?');
    if (pos != std::string::npos) {

        _scriptFilename = _requesttarget.substr(0, pos);
        _query_string = _requesttarget.substr(pos + 1);
    
    } else {

        _scriptFilename = _requesttarget;
        _query_string = "";
    }

    _isCGI = true;
    return;
}

bool HTTPParser::isCGI() const
{
    return _isCGI;
}

bool HTTPParser::validateCGIRequest()
{
    if (_method != "GET" && _method != "POST" && _method != "DELETE")
    {
        _errors = true;
        _code = HTTP_405;
        return false;
    }
    if (_method == "POST")
    {
        if (!checkContentType())
        {
            LOG_ERROR("CGI POST: missing Content-Type");
            return false;
        }
        if (!checkContentLength()){
            LOG_ERROR("CGI POST: missing Content-Length");
            return false;
        }
    }

    _code = HTTP_CGI;

    return true;
}

const LocationConfig* HTTPParser::matchLocation() const {

    const std::vector<LocationConfig> &locs = _serverConfig.getLocations();
    const LocationConfig *best = NULL;
    size_t bestLen = 0;

    for (size_t i = 0; i < locs.size(); ++i)
    {
        const std::string &path = locs[i].getPath();
        if (_requesttarget.compare(0, path.size(), path) == 0
            && path.size() >= bestLen)
        {
            bestLen = path.size();
            best = &locs[i];
        }
    }

    return best;
}

void HTTPParser::buildFullPath() {

    const LocationConfig *loc = matchLocation();

    std::string root = (loc && !loc->getRoot().empty())
        ? loc->getRoot() : _serverConfig.getRoot();
    _autoindexOn = (loc && loc->getAutoindex() == "on");

    _fullPath = root;
    if (!_fullPath.empty() && _fullPath[_fullPath.size() - 1] == '/'
        && !_requesttarget.empty() && _requesttarget[0] == '/')
        _fullPath.erase(_fullPath.size() - 1);
    _fullPath += _requesttarget;

    LOG_DEBUG(std::string("[HTTPParser] _fullPath: '") + _fullPath
        + "' autoindex=" + (_autoindexOn ? "on" : "off"));
}

void HTTPParser::extractRange() {//bool

    size_t start = _request.find("Range:");
    if (start == std::string::npos)//if varNotFound400(start)
        return;
    start += 6;
    size_t end = _request.find("\r\n", start);
    if (end == std::string::npos)
        end = _request.find("\n", start);
    if (end == std::string::npos)
        return;
    _rangeHeader = _request.substr(start, end - start);
    size_t nonSpace = _rangeHeader.find_first_not_of(" \t");
    if (nonSpace == std::string::npos)
        _rangeHeader.clear();
    else
        _rangeHeader = _rangeHeader.substr(nonSpace);
    LOG_DEBUG("[HTTPParser] Range: '" + _rangeHeader + "'");
}

/*
** ============================================================================
** Parser HTTP - Second Part: POST
** ============================================================================
*/

bool HTTPParser::checkContentType() {

    size_t start = _request.find("Content-Type:");
    if (varNotFound400(start) == false)
        return false;
        
    size_t requestLineEnd = _request.find("\r\n", start);
    if (varNotFound400(requestLineEnd) == false)
        return false;
    std::string line = _request.substr(start, requestLineEnd - start);

    std::string::iterator space = line.begin();
    std::string::iterator end = line.end();

    std::vector<size_t> space_inter = collectSpace(space, end);
    
    std::vector<std::string> subss = collectString(line, space_inter);

    if (subss.size() >= 2) {

        _type = subss[1];
        _type.erase(_type.end() - 1);
        LOG_DEBUG("Content-Type: " + _type);
        if (_isCGI)
            return true;
        if (subss.size() < 3)
        {
            _errors = true;
            _code = HTTP_400;
            _type = "text/html";

            return false;
        }

        _boundary = subss[2];
        _boundary.erase(_boundary.begin(), _boundary.begin() + 9);
        LOG_DEBUG("boundary: " + _boundary);

        return true;
    }

    _errors = true;
    _code = HTTP_400;
    _type = "text/html";

    return false;
}

bool HTTPParser::checkContentLength() {

    size_t start = _request.find("Content-Length:");
    if (varNotFound400(start) == false){
        LOG_ERROR("No COntent-Length");
        return false;
    }
    
    size_t requestLineEnd = _request.find("\r\n", start);
    if (varNotFound400(requestLineEnd) == false)
        return false;
    std::string line = _request.substr(start, requestLineEnd - start);
    std::string::iterator space = line.begin();
    std::string::iterator end = line.end();
    
    std::vector<size_t> space_inter = collectSpace(space, end);
    
    std::vector<std::string> subss = collectString(line, space_inter);

    if (subss.size() == 2) {

        _fileLength = subss[1];
        std::stringstream ss(_fileLength);
        if (_isCGI)
        {
            int len;
            ss >> len;
            LOG_DEBUG("Content-Length: " + _fileLength);

            if (len > BUF_SIZE) {
                LOG_ERROR("File size exceeds limit");
                return false;
            }
            _content_int = len;
            _content_length = _fileLength;

            return true;
        }

        size_t len;
        ss >> len;
        LOG_DEBUG("Content-Length: " + _fileLength);

        if (len > BUF_SIZE) {
            LOG_ERROR("File size exceeds limit");
            return false;
        }
        _content_length = _fileLength;
        return true;
    }

    _errors = true;
    _code = HTTP_400;
    _type = "text/html";

    return false;
}

bool HTTPParser::checkContentDisposition() {
    
    size_t pos = _request.find("Content-Disposition:");
    if (varNotFound400(pos) == false)
        return false;
    
    size_t requestLineEnd = _request.find("\r\n", pos);
    if (varNotFound400(requestLineEnd) == false)
        return false;
        
    std::string line = _request.substr(pos, requestLineEnd - pos);

    std::string::iterator newpos = line.begin();
    std::string::iterator end = line.end();

    std::vector<size_t> space_inter = collectSpace(newpos, end);

    std::vector<std::string> subsss = collectString(line,space_inter);
     
     if (subsss.size() == 4) {

        std::string type = subsss[1];
        char const *slash = strchr(_type.c_str(), '/');
        if (doesCharCExist400(slash) == false)
            return false;

        std::string checktype = std::string(slash, strlen(slash));
        checktype.erase(checktype.begin());
        checktype.erase(checktype.end() - 1);
        std::string name = subsss[2];
        name.erase(name.end() - 2);
        name.erase(name.begin(), name.begin() + 6);
        _fileName = subsss[3];
        _fileName.erase(_fileName.end() - 1);
        _fileName.erase(_fileName.begin(), _fileName.begin() + 10 );
        LOG_DEBUG("Disposition type: " + checktype);
        LOG_DEBUG("Disposition name: " + name);
        LOG_DEBUG("Upload filename: " + _fileName);

        _pos = _request.begin() + requestLineEnd; 
        return true;
    }

    _errors = true;
    _code = HTTP_400;
    _type = "text/html";

    return false;
}

bool HTTPParser::gatherFile() {
    
    _pos++;
    _pos++;
    
    size_t begin = distance(_request.begin(),_pos);
    size_t requestLineEnd = _request.find("\r\n",begin);
    if (varNotFound400(requestLineEnd) == false)
        return false;
    
    std::string line = _request.substr(begin, requestLineEnd - begin);
    std::string::iterator start = line.begin();
    std::string::iterator end = line.end();
    
    std::vector<size_t> space_inter = collectSpace(start, end);

    std::vector<std::string> subss = collectString(line,space_inter);
     
    if (subss.size() < 2)
    {
        _errors = true;
        _code = HTTP_400;
        _type = "text/html";

        return false;
    }

    _type = subss[1];
    LOG_DEBUG("File Content-Type: " + _type);

    _pos = _request.begin() + requestLineEnd;
    _pos += 4;

    std::string endOfFile = _boundary + "--";
    size_t end_pos = _request.find(endOfFile);
    if (varNotFound400(requestLineEnd) == false){
        LOG_ERROR("End boundary not found in request");
        return false;
    }
 
    _fileBuf.assign(_pos, _request.begin() + end_pos - 4);
    std::cout << "Firsts 100 octets de _fileBuf : '" <<
    std::string(_fileBuf.begin(), _fileBuf.begin() + 100) << "'" << std::endl;
    std::cout << "Derniers 100 octets de _fileBuf : '" <<
    std::string(_fileBuf.end() - 100, _fileBuf.end()) << "'" << std::endl;
    std::cout << "20 derniers octets de _fileBuf : ";
    for (size_t i = _fileBuf.size() - 20; i < _fileBuf.size(); ++i) {
        printf("%02X ", static_cast<unsigned char>(_fileBuf[i]));
    }
    std::cout << std::endl;

    return true;
}

/*
** ============================================================================
** Parser HTTP - Third Part: Dispatch per Method + Helpers
** ============================================================================
*/



bool HTTPParser::compareMethodWithConfigFile() {
    
    const std::vector<LocationConfig> &locs = _serverConfig.getLocations();
    if (locs.size() == 0) {
        _code = HTTP_INDEX;
        _type = "text/html";
        return true;
    }
    
    for (size_t i = 0;i < locs.size(); i++)
    {
        const std::vector<std::string> &methodVector = locs[i].getMethods();
        if (methodVector.size() > 0) {
            for (size_t i = 0; i < methodVector.size() ; i++) {
                if ( _method == methodVector[i])
                    return true;
            }
        }
    }

    _errors = true;
    _code = HTTP_405;
    _type = "text/html";

    return false;
}


std::string HTTPParser::addSuffix(std::string suffix) {
    
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
    if (suffix == ".bla")
        _type = "text/bla";
    if (suffix == ".pouic")
        _type = "text/pouic";
    if (suffix == ".bad_extension")
        _type = "text/bad_extension";
    return _type;
}

// Main function findMethods() :
// Compare method found in request and in config file
// if no correspondance : error 405 method not accepted
// then get index from config file :
// if multiple, check if valide then goes to the next
// if none valid, index by default
bool HTTPParser::findMethods() {

    if (compareMethodWithConfigFile() == true)
    {
        if (_method == "GET")
        {
            const std::vector<LocationConfig> &locs =
                _serverConfig.getLocations();

            for (size_t i = 0;i < locs.size(); i++)
            {
                const std::vector<std::string> &indexVector =
                    locs[i].getIndex();
                if (indexVector.size() > 0){
                    for (size_t i = 0; i < indexVector.size() ; i++){
                        struct stat sb;
                        std::string index = "data/www/html/" + indexVector[i];
                        if (stat(index.c_str(), &sb) == 0){
                            _fileName = indexVector[i];
                            _code = HTTP_FILE;
                            break ;
                        }
                    }
                }
                else{
                    _code = HTTP_INDEX;
                }
            }

            if (_requesttarget == "/" || _requesttarget == "/api")
            {
                struct stat path_stat;
                if (stat(_fullPath.c_str(), &path_stat) == -1)
                {
                    LOG_ERROR("stat failed for: " + _fullPath);

                    _errors = true;
                    _code = HTTP_404;
                    _type = "text/html";

                    return false;
                }

                if (S_ISDIR(path_stat.st_mode))
                {
                    std::string indexPath = _fullPath;
                    if (indexPath[indexPath.size() - 1] != '/')
                        indexPath += "/";
                    // indexPath += "index.html";
                    indexPath += "index.html";

                    struct stat index_stat;
                    if (stat(indexPath.c_str(), &index_stat) == 0
                        && S_ISREG(index_stat.st_mode))
                    {
                        _code = HTTP_INDEX;
                        _type = "text/html";

                        return true;
                    }
                    if (_autoindexOn)
                    {
                        _code = HTTP_AUTOINDEX;
                        _type = "text/html";

                        return true;
                    }
                    // case of autoindex == off and index doesn't exist
                    _errors = true;
                    _code = HTTP_403;
                    _type = "text/html";

                    return false;
                }
            }

            if (_requesttarget.find("/images") == 0)
            {
                char const *lastSlash = strrchr(_requesttarget.c_str(), '/');
                if (doesCharCExist400(lastSlash) == false)
                    return false;
                char const *lastPoint = strrchr(_requesttarget.c_str(), '.');
                if (doesCharCExist400(lastPoint) == false)
                    return false;
                std::string name = std::string(lastSlash, strlen(lastSlash));
                name.erase(name.begin());
                _fileName = name;
                _code = HTTP_FILE;
                std::string suffix = std::string(lastPoint, strlen(lastPoint));
                _type = addSuffix(suffix);

                return true;
            }

            if (_requesttarget.find("/data/upload") != std::string::npos)
            {
                char const *lastSlash = strrchr(_requesttarget.c_str(), '/');
                if (doesCharCExist400(lastSlash) == false)
                    return false;
                std::string name = std::string(lastSlash, strlen(lastSlash));
                name.erase(name.begin());
                _fileName = name;
                _code = HTTP_FILE;
                char const *lastPoint = strrchr(_requesttarget.c_str(), '.');
                if (lastPoint) {

                    std::string suffix =
                        std::string(lastPoint, strlen(lastPoint));
                    _type = addSuffix(suffix);

                } else {

                    _type = "image/png";
                }

                _upload = true;

                return true;
            }
           
            if (_requesttarget == "/favicon.ico"){

                _code = HTTP_FAVICON;
                _type = "image/x-icon";

                return true;

            } else {
                _fileName = _requesttarget;
                _fileName.erase(_fileName.begin());
                _code = HTTP_FILE;
                _type = "text/html";

                return true;
            }
        
        } else if (_method == "POST") {

            if (_requesttarget.find("/upload") != std::string::npos)
            {
                
                if (_requesttarget == "/upload") {
                    
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

                } else {

                    char const *lastSlash = strrchr(_requesttarget.c_str(), '/');
                    if (doesCharCExist400(lastSlash) == false)
                        return false;
                    char const *lastPoint = strrchr(_requesttarget.c_str(), '.');
                    if (doesCharCExist400(lastPoint) == false)
                        return false;
                    std::string name = std::string(lastSlash, strlen(lastSlash));
                    name.erase(name.begin());

                    LOG_DEBUG("POST upload target: " + name);

                    _fileName = name;
                    _code = HTTP_FILE;
                    std::string suffix =
                        std::string(lastPoint, strlen(lastPoint));
                    _type = addSuffix(suffix);

                    return true;
                }

            }
        
        } else if (_method == "DELETE") {
            
            if (_requesttarget.find("/upload") != std::string::npos)
            {
                
                char const *lastSlash = strrchr(_requesttarget.c_str(), '/');
                if (doesCharCExist400(lastSlash) == false)
                    return false;
                char const *lastPoint = strrchr(_requesttarget.c_str(), '.');
                if (doesCharCExist400(lastPoint) == false)
                    return false;
                std::string name = std::string(lastSlash, strlen(lastSlash));
                name.erase(name.begin());
                LOG_DEBUG("DELETE target: " + name);

                _fileName = name;
                _code = HTTP_FILE;
                std::string suffix = std::string(lastPoint, strlen(lastPoint));
                _type = addSuffix(suffix);

                return true;
            }
        }

        _errors = true;
        _code = HTTP_404;
        _type = "text/html";

        return false;
    }

    return false;
}

std::string HTTPParser::httpCodeToString( HttpCode code )
{
    switch (code)
    {
        case HTTP_400: return "400";
        case HTTP_403: return "403";
        case HTTP_404: return "404";
        case HTTP_405: return "405";
        case HTTP_413: return "413";
        case HTTP_421: return "421";
        case HTTP_201:      return "201";
        case HTTP_204:      return "204";
        case HTTP_500:      return "500";
        case HTTP_CGI:      return "200";
        case HTTP_INDEX:    return "200";
        case HTTP_AUTOINDEX:return "200";
        case HTTP_FAVICON:  return "200";
        case HTTP_FILE:     return "200";
        default:            return "200";
    }
}

bool HTTPParser::findPath()
{
    return true;
}

bool HTTPParser::findHeaders()
{
    return true;
}