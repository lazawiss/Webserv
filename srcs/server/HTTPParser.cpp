/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leazannis <leazannis@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 17:20:07 by lzannis           #+#    #+#             */
/*   Updated: 2026/08/11 23:18:00 by leazannis        ###   ########.fr       */
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
    _request(request), _serverConfig(serverConfig),
    _code(HTTP_INDEX), _type(), _method(),
    _requesttarget(), _httpversion(), _boundary(),
    _fileLength(), _fileName(), _fileBuf(),
    _errors(false), _upload(false), _content_length(0), 
    _connectionType(CONN_KEEP_ALIVE), _host("8080"),
    _isCGI(false), _fullPath(), _query_string(), _scriptFilename(),
    _body(), _content_type(), _content_int(0), _isContentLengthFound(false),
    _isHostFound(false), _isContentTypeFound(false), _fileContentType() {}

HTTPParser::HTTPParser( HTTPParser const & src ) :
    _request(src._request), _serverConfig(src._serverConfig),
    _code(src._code), _type(src._type),
    _method(src._method), _requesttarget(src._requesttarget),
    _httpversion(src._httpversion), _boundary(src._boundary),
    _fileLength(src._fileLength), _fileName(src._fileName),
    _fileBuf(src._fileBuf), _errors(src._errors),
    _upload(src._upload), _content_length(src._content_length), 
    _connectionType(src._connectionType),
    _host(src._host), _isCGI(src._isCGI),
    _fullPath(src._fullPath), _query_string(src._query_string),
    _scriptFilename(src._scriptFilename), _body(src._body),
    _content_type(src._content_type),
    _content_int(src._content_int),
    _isContentLengthFound(src._isContentLengthFound),
    _isHostFound(src._isHostFound),
    _isContentTypeFound(src._isContentTypeFound), 
    _fileContentType(src._fileContentType) {}

HTTPParser::~HTTPParser() {}

HTTPParser &    HTTPParser::operator=( HTTPParser const & other )
{
    if (this != &other )
    {
        _request            = other._request;
        _code               = other._code;
        _type               = other._type;
        _method             = other._method;
        _requesttarget      = other._requesttarget;
        _httpversion        = other._httpversion;
        _boundary           = other._boundary;
        _fileLength         = other._fileLength;
        _fileName           = other._fileName;
        _fileBuf            = other._fileBuf;
        _errors             = other._errors;
        _upload             = other._upload;
        _connectionType     = other._connectionType;
        _content_length     = other._content_length;
        _host               = other._host;
        _isCGI              = other._isCGI;
        _fullPath           = other._fullPath;
        _query_string       = other._query_string;
        _scriptFilename     = other._scriptFilename;
        _body               = other._body;
        _content_type       = other._content_type;
        _content_int        = other._content_int;
        _autoindexOn        = other._autoindexOn;
        _rangeHeader        = other._rangeHeader;
        _isContentLengthFound = other._isContentLengthFound;
        _isHostFound        = other._isHostFound;
        _isContentTypeFound = other._isContentTypeFound;
        _fileContentType    = other._fileContentType;

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
size_t     HTTPParser::getContentLength() const
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

// ── cgi ─────────────────────────────────────────────────────────────────────
bool HTTPParser::isCGI() const
{
    return _isCGI;
}

/*
** ============================================================================
** Parser HTTP
** ============================================================================
*/

static RequestParser getHeaderType( const std::string & key ) {

    if (key == "Connection")        return CONNECTION;
    if (key == "Content-Length")    return CONTENT_LENGTH;
    if (key == "Content-Type")      return CONTENT_TYPE;
    if (key == "Host")              return HOST;
    if (key == "Range")             return RANGE;

    return UNKNOWN;
}

bool HTTPParser::isRequestValid( ListenerManager const & listen ) {

    if (checkSize() == false)
        return LOG_ERROR("Request size exceeds limit"), false;

    if (checkRequestLine() == false)
        return LOG_ERROR("Invalid request line"), false;
    
    size_t headerEnd = _request.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return LOG_ERROR("Malformed request: missing end of header"), false;
    _body = _request.substr(headerEnd + 4);

    parseCGI();

    size_t i = _request.find("\r\n");
    if (i == std::string::npos)
        return false;

    i += 2;
    if (i < _request.size() && (_request[i] == ' ' || _request[i] == '\t'))
        return LOG_ERROR("Leading whitespace after request-line"), false;

    while (i < _request.size())
    {
        size_t end = _request.find("\r\n", i);
        if (end == std::string::npos)
            return LOG_ERROR("Malformed request: missing CRLF"), false;

        if (end == i)
            break;

        std::string line = _request.substr(i, end - i);
        size_t colon = line.find(":");
        if (colon == std::string::npos)
            return LOG_ERROR("Malformed header line: " + line), false;

        std::string key = line.substr(0, colon);
        std::string value = line.substr(colon + 1);
        
        if (!value.empty() && value[0] == ' ')
            value = value.substr(1);

        if (!value.empty() && (value[0] == ' ' || value[0] == '\t'))
            return LOG_ERROR("Invalid whitespace after ':' in header: " + line), false;

        switch (getHeaderType(key))
        {
            case HOST:

                if (_isHostFound == true)
                    return LOG_ERROR("Duplicate 'Host' header"), false;

                if (checkHost(value, listen) != SERVER_OK)
                    return false;

                _isHostFound = true;
                break;

            case CONNECTION:

                if (checkConnection(value) != SERVER_OK)
                    return false;
                break;

            case CONTENT_TYPE:

                if (_isContentTypeFound == true)
                    return LOG_ERROR("Duplicate 'Content-type' header"), false;

                if (checkContentType(value) != SERVER_OK)
                    return false;
                
                _isContentTypeFound = true;
                break;
            
            case CONTENT_LENGTH:

                if (_isContentLengthFound)
                    return LOG_ERROR("Duplicate 'Content-Length' header"), false;
    
                if (checkContentLength(value) != SERVER_OK)
                    return false;

                _isContentLengthFound = true;
                break;

            case RANGE:

                checkRange(value);
                break;

            default:
                break;
        }

        i = end + 2;
        if (i < _request.size() && (_request[i] == ' ' || _request[i] == '\t'))
            return LOG_ERROR("Leading whitespace after request-line"), false;
    }

    if (_isHostFound == false)
        return LOG_ERROR("Host header missing"), false;
    
    resolveConnectionType();

    buildFullPath();

    return true;
}

void HTTPParser::parseCGI(){
        // GET /cgi-bin/hello.py?name=andi HTTP/1.1
    if (_requesttarget.find("/cgi-bin/") == std::string::npos) {
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
}

bool HTTPParser::validateCGIRequest() {

    if (_scriptFilename.empty())
        return false;
    
    std::string path = "data/" + _scriptFilename;
    struct stat st;
    if (stat(path.c_str(), &st) != 0 || !S_ISREG(st.st_mode))
        return false;
    
    return true;
}

/*
** ============================================================================
** Parser HTTP - GET
** ============================================================================
*/

bool HTTPParser::checkSize() {
    
    if (_request.size() > BUF_SIZE) {
        
        _errors = true;
        _code = HTTP_413;
        _type = "text/html";

        return false;
    }

    return true;
}

bool HTTPParser::checkRequestLine() {

    size_t lineEnd = _request.find("\r\n");
    if (varNotFound400(lineEnd) == false)
        return false;

    std::string line = _request.substr(0, lineEnd);

    size_t firstSpace = line.find(' ');
    if (varNotFound400(firstSpace) == false)
        return false;

    size_t secondSpace = line.find(' ', firstSpace + 1);
    if (varNotFound400(secondSpace) == false)
        return false;

    if (line.find(' ', secondSpace + 1) != std::string::npos) {
        _errors = true;
        _code   = HTTP_400;
        _type   = "text/html";

        return false;
    }

    _method         = line.substr(0, firstSpace);
    _requesttarget  = line.substr(firstSpace + 1, secondSpace - firstSpace - 1);
    _httpversion    = line.substr(secondSpace + 1);

    if (_method != "GET" && _method != "POST" && _method != "DELETE") {
        _errors = true;
        _code   = HTTP_405;
        _type   = "text/html";

        return false;
    }

    // ------------ Debug ------------
    LOG_DEBUG("Method: " + _method);
    LOG_DEBUG("RequestTarget: " + _requesttarget);

    if (_httpversion != "HTTP/1.1") {

        LOG_ERROR("Unsupported HTTP version: " + _httpversion);

        _errors = true;
        _code   = HTTP_400;
        _type   = "text/html";

        return false;
    }

    // ------------ Debug ------------
    LOG_DEBUG("HTTP version: " + _httpversion);

    return true;
}

int HTTPParser::checkHost( std::string const & value, ListenerManager const & listener ) {

    std::string listen;

    if (validateHost(value, listen) == false)
    {
        LOG_ERROR("Invalid Host header: '" + value + "'");

        _errors = true;
        _code   = HTTP_400;
        _type   = "text/html";

        return SERVER_ERROR;
    }

    if (matchHost(listen, listener) == false)
    {
        // ------------ Debug ------------
        LOG_DEBUG("Host mismatch, got: " + value);

        _errors = true;
        _code   = HTTP_421;
        _type   = "text/html";

        return SERVER_ERROR;
    }

    _host = listen;

    return SERVER_OK;
}

bool HTTPParser::validateHost( std::string const & value, std::string & listen ) {

    if (value.empty())
        return false;

    char const *colon = strrchr(value.c_str(), ':');

    std::string host;
    std::string port;

    if (colon == NULL || colon == value.c_str() || *(colon + 1) == '\0')
        return false;

    host = std::string(value.c_str(), colon);
    port = std::string(colon + 1);

    for (size_t i = 0; i < host.size(); ++i)
    {
        char c = host[i];
        if (isalnum(static_cast<unsigned char>(c)) == false && c != '.' && c != '-')
            return false;
    }
    for (size_t i = 0; i < port.size(); ++i)
    {
        if (isdigit((unsigned char)port[i]) == false)
            return false;
    }

    listen = host;
    if (port.empty() == false)
        listen += ":" + port;

    return true;
}

bool HTTPParser::matchHost( std::string const & host, ListenerManager const & listener ) {

    std::string hostname = listener.getNode() + ":" + listener.getService();

    char const *colon = strrchr(host.c_str(), ':');
    if (colon == NULL)
        return false;

    std::string hostName = std::string(host.c_str(), colon);
    std::string service  = std::string(colon + 1);

    if (host == hostname)
        return true;

    if (listener.getNode() == "0.0.0.0" && listener.getService() == service)
        return true;

    return false;
}

// Resolve default Connection behavior when the header didn't set it.
// Server only accepts HTTP/1.1, so default is keep-alive.
void HTTPParser::resolveConnectionType() {

    if (_connectionType == CONN_NONE)
        _connectionType = CONN_KEEP_ALIVE;
}

// Parses the "Connection" header value (nginx-style: substring match,
// case-insensitive). Unknown values are ignored; default is resolved
// later by resolveConnectionType().
int HTTPParser::checkConnection( std::string const & value ) {

    if (containsCaseInsensitive(value, "close"))
        _connectionType = CONN_CLOSE;
    else if (containsCaseInsensitive(value, "keep-alive"))
        _connectionType = CONN_KEEP_ALIVE;

    return SERVER_OK;
}

// Stores the Range header value if it follows the "bytes=X-Y" format,
// ignores it otherwise.
void HTTPParser::checkRange( std::string const & value ) {

    if (value.substr(0, 6) != "bytes=")
        return;

    _rangeHeader = value;

    // ------------ Debug ------------
    LOG_DEBUG("[HTTPParser] Range: '" + _rangeHeader + "'");
}


/*
** ============================================================================
** Parser HTTP - GET/POST
** ============================================================================
*/

int HTTPParser::checkContentType( std::string const & value ) {

    if (value.empty()) {
        LOG_ERROR("Empty Content-Type header");

        _errors = true;
        _code   = HTTP_400;
        _type   = "text/html";

        return SERVER_ERROR;
    }

    // multipart/form-data; boundary=ExampleBoundaryString
    size_t semicolon = value.find(';');
    std::string type = (semicolon == std::string::npos) ? value : value.substr(0, semicolon);

    while (!type.empty() && isspace(static_cast<unsigned char>(type[type.size() - 1])))
        type.erase(type.size() - 1);

    _type = type;
    _content_type = type;

    // ------------ Debug ------------
    LOG_DEBUG("Content-Type(1): " + _type);
    // ------------ Debug ------------
    LOG_DEBUG("Content-Type(2): " + _content_type);

    if (_isCGI)
        return SERVER_OK;
    
    if (_type != "multipart/form-data")
        return SERVER_OK;

    if (semicolon == std::string::npos) {
        LOG_ERROR("Missing boundary in Content-Type: '" + value + "'");

        _errors = true;
        _code   = HTTP_400;
        _type   = "text/html";

        return SERVER_ERROR;
    }

    size_t boundary = value.find("boundary=", semicolon);
    if (boundary == std::string::npos) {
        LOG_ERROR("Missing boundary in Content-Type: '" + value + "'");

        _errors = true;
        _code   = HTTP_400;
        _type   = "text/html";

        return SERVER_ERROR;
    }

    // MARQUE for Lea/Delphine: what to do in this case:
    // value = "multipart/form-data; boundary=\"----WebKit123\"; charset=utf-8"
    _boundary = value.substr(boundary + 9);
    // ------------ Debug ------------
    LOG_DEBUG("boundary: " + _boundary);

    return SERVER_OK;
}

int HTTPParser::checkContentLength( std::string const & value ) {

    if (value.empty()) {
        LOG_ERROR("Empty Content-Length header");

        _errors = true;
        _code   = HTTP_400;
        _type   = "text/html";

        return SERVER_ERROR;
    }

    for (size_t i = 0; i < value.size(); i++) {
        if (!std::isdigit(static_cast<unsigned char>(value[i]))) {
            LOG_ERROR("Invalid Content-Length: '" + value + "'");

            _errors = true;
            _code   = HTTP_400;
            _type   = "text/html";

            return SERVER_ERROR;
        }
    }

    std::stringstream ss(value);
    size_t len;
    ss >> len;

    if (ss.fail()) {
        LOG_ERROR("Content-Length overflow: '" + value + "'");

        _errors = true;
        _code   = HTTP_400;
        _type   = "text/html";

        return SERVER_ERROR;
    }

    size_t limit = parseBodySize(_serverConfig.getClientMaxBodySize());
    if (limit == 0)
        limit = BUF_SIZE;
    if (len > limit) {
        LOG_ERROR("Content-Length exceeds limit");

        _errors = true;
        _code   = HTTP_413;
        _type   = "text/html";

        return SERVER_ERROR;
    }

    _content_length = len;
    std::ostringstream oss;
    oss << _content_length;
    // ------------ Debug ------------
    LOG_DEBUG("Content-Length: " + oss.str());

    return SERVER_OK;
}

size_t HTTPParser::parseBodySize( std::string const & s ) {
    if (s.empty())
        return 0;

    std::stringstream ss(s);
    size_t value;
    ss >> value;
    if (ss.fail())
        return 0;

    char unit = '\0';
    ss >> unit;

    switch (std::toupper(static_cast<unsigned char>(unit))) {
        case 'K': return value * 1024UL;
        case 'M': return value * 1024UL * 1024UL;
        case 'G': return value * 1024UL * 1024UL * 1024UL;

        default:  return value;
    }
}

/*
** ============================================================================
** Parser HTTP - POST
** ============================================================================
*/

// Parses "Content-Disposition: form-data; name="..."; filename="...""
bool HTTPParser::checkContentDisposition( size_t & curPos ) {

    size_t pos = _body.find("Content-Disposition:");
    if (varNotFound400(pos) == false)
        return LOG_ERROR("Content-Disposition missing for POST method"), false;

    size_t lineEnd = _body.find("\r\n", pos);
    if (varNotFound400(lineEnd) == false)
        return LOG_ERROR("Malformed request: missing CRLF"), false;

    std::string line = _body.substr(pos, lineEnd - pos);

    if (varNotFound400(line.find("form-data")) == false)
        return LOG_ERROR("Content-Disposition is not a form-data type"), false;

    size_t fnamePos = line.find("filename=\"");
    if (varNotFound400(fnamePos) == false)
        return LOG_ERROR("Content-Disposition needs a filename"), false;

    size_t fnameStart = fnamePos + 10;
    size_t fnameEnd = line.find("\"", fnameStart);
    if (varNotFound400(fnameEnd) == false)
        return false;

    _fileName = line.substr(fnameStart, fnameEnd - fnameStart);
    // ------------ Debug ------------
    LOG_DEBUG("Upload filename: " + _fileName);

    curPos = lineEnd + 2;
    return true;
}

bool HTTPParser::gatherFile( size_t curPos ) {

    size_t lineEnd = _body.find("\r\n", curPos);
    if (varNotFound400(lineEnd) == false)
        return LOG_ERROR("Malformed request: missing CRLF"), false;

    std::string line = _body.substr(curPos, lineEnd - curPos);

    // Content-Type: image/jpeg
    size_t pos = line.find("Content-Type:");
    if (pos == 0) {
        size_t j = pos + 13; //MARQUE
        while (j < line.size() && (line[j] == ' ' || line[j] == '\t'))
            j++;

        _fileContentType = line.substr(j);
    
        // ------------ Debug ------------
        LOG_DEBUG("File Content-Type: " + _fileContentType);
    } else {
        _fileContentType = "";
    }

    size_t position = lineEnd + 2;
    if (position + 1 < _body.size() && _body[position] == '\r' && _body[position + 1] == '\n')
        position += 2;

    std::string endMarker = "--" + _boundary; // ----boundary=
    size_t endPos = _body.find(endMarker, position);
    if (varNotFound400(endPos) == false)
        return LOG_ERROR("End boundary not found in body"), false;

    size_t fileEnd = endPos;
    if (fileEnd >= 2 && _body[fileEnd - 2] == '\r' && _body[fileEnd - 1] == '\n')
        fileEnd -= 2;

    if (fileEnd < position) {
        LOG_ERROR("Malformed or empty file content");

        _errors = true;
        _code = HTTP_400;
        _type = "text/html";

        return false;
    }

    _fileBuf = _body.substr(position, fileEnd - position);

    std::ostringstream oss;
    oss << _fileBuf.size();

    // ------------ Debug ------------
    LOG_DEBUG("Gathered file bytes: " + oss.str());

    return true;
}

/*
** ============================================================================
** Method helpers
** ============================================================================
*/

bool HTTPParser::varNotFound400 ( size_t var ) {
    
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

bool HTTPParser::containsCaseInsensitive( std::string const & haystack, std::string const & needle ) {

    std::string h = haystack;
    std::string n = needle;

    std::transform(h.begin(), h.end(), h.begin(), ::tolower);
    std::transform(n.begin(), n.end(), n.begin(), ::tolower);
    if (h.find(n) == std::string::npos)
        return false;

    return true;
}

void HTTPParser::buildFullPath() {

    const std::vector<LocationConfig> &locs = _serverConfig.getLocations();
    const LocationConfig *bestLoc = NULL;
    size_t bestLen = 0;

    for (size_t i = 0; i < locs.size(); ++i)
    {
        const std::string &locPath = locs[i].getPath();
        if (_requesttarget.compare(0, locPath.size(), locPath) == 0
            && locPath.size() >= bestLen)
        {
            bestLen  = locPath.size();
            bestLoc  = &locs[i];
        }
    }

    // MARQUE
    std::string root;
    if (bestLoc && !bestLoc->getRoot().empty())
        root = bestLoc->getRoot();
    else
        root = _serverConfig.getRoot();

    _autoindexOn = (bestLoc && bestLoc->getAutoindex() == "on");

    std::string suffix = _requesttarget;
    if (bestLoc)
        suffix = _requesttarget.substr(bestLoc->getPath().size());

    _fullPath = root;

    if (suffix.empty()) {
    } else if (suffix[0] == '/') {
        _fullPath += suffix;
    } else {
        _fullPath += '/';
        _fullPath += suffix;
    }

    // ------------ Debug ------------
    LOG_DEBUG(std::string("[HTTPParser] _fullPath: '") + _fullPath + 
        "' autoindex=" + (_autoindexOn ? "on" : "off"));
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
    if (suffix == ".css")
        _type = "text/css";
    if (suffix == ".js")
        _type = "text/javascript";

    return _type;
}

std::string HTTPParser::httpCodeToString( HttpCode code )
{
    switch (code)
    {
        case HTTP_301:          return "301";
        case HTTP_302:          return "302";
        case HTTP_400:          return "400";
        case HTTP_403:          return "403";
        case HTTP_404:          return "404";
        case HTTP_405:          return "405";
        case HTTP_413:          return "413";
        case HTTP_421:          return "421";
        case HTTP_201:          return "201";
        case HTTP_204:          return "204";
        case HTTP_500:          return "500";
        case HTTP_502:          return "502";
        case HTTP_CGI:          return "200";
        case HTTP_INDEX:        return "200";
        case HTTP_AUTOINDEX:    return "200";
        case HTTP_FAVICON:      return "200";
        case HTTP_FILE:         return "200";

        default:                return "200";
    }
}

/*
** ============================================================================
** Parser HTTP - Dispatch per Method
** ============================================================================
*/

bool HTTPParser::compareMethodWithConfigFile() {

    const std::vector<LocationConfig> &locs = _serverConfig.getLocations();
    if (locs.size() == 0) {
        _code = HTTP_INDEX;
        _type = "text/html";

        return true;
    }

    // Find the best matching location for the current URI (longest prefix match)
    int bestIdx = -1;
    size_t bestLen = 0;
    for (size_t i = 0; i < locs.size(); i++)
    {
        const std::string &path = locs[i].getPath();
        if (_requesttarget.find(path) == 0 && path.size() > bestLen
            && (path == "/" || _requesttarget.size() == path.size()
                || _requesttarget[path.size()] == '/' || _requesttarget[path.size()] == '?'))
        {
            bestLen = path.size();
            bestIdx = (int)i;
        }
    }

    if (bestIdx == -1) {
        _errors = true;
        _code = HTTP_405;
        _type = "text/html";
        return false;
    }

    const std::vector<std::string> &methodVector = locs[bestIdx].getMethods();
    for (size_t j = 0; j < methodVector.size(); j++)
    {
        if (_method == methodVector[j])
            return true;
    }

    _errors = true;
    _code = HTTP_405;
    _type = "text/html";

    return false;
}

bool HTTPParser::isRedir(){

     const std::vector<LocationConfig> &locs = _serverConfig.getLocations();
    if (locs.size() == 0) {
        _code = HTTP_INDEX;
        _type = "text/html";

        return true;
    }
    std::cout << "locs.size(): " << locs.size() << std::endl;

    std::cout << "_requesttarget: " << _requesttarget << std::endl;
    int bestIdx = -1;
    size_t bestLen = 0;
    for (size_t i = 0; i < locs.size(); i++)
    {
        const std::string &path = locs[i].getPath();
        std::cout << "path: " << path << std::endl;
        if (_requesttarget.find(path) == 0)
        {
            bestLen = path.size();
            std::cout << "bestLen: " << bestLen << std::endl;
            bestIdx = (int)i;
        }
    }
    
    std::cout << "bestIdx: "<< bestIdx << std::endl;
    if (bestIdx == -1) {
        _errors = true;
        _code = HTTP_405;
        _type = "text/html";
        return false;
    }
    
    const std::map<int, std::string> &returnMap = locs[bestIdx].getReturn();
    std::cout << returnMap.size() << std::endl;
    
    if (returnMap.size() > 0){
        
        std::map<int, std::string>::const_iterator it = returnMap.begin();
        std::cout << it->first << " : " << it->second << std::endl;
        
        if (it->first == 301){
            _code = HTTP_301;
            _fileName = it->second;
            _type = "text/html";
            return true;
        }
        if (it->first == 302){
            _code = HTTP_302;
            _fileName = it->second;
            _type = "text/uri-list";
            return true;
        }
    }

    return true;
}


bool HTTPParser::findMethods() {

    if(isRedir() == false)
        return false;
    if (compareMethodWithConfigFile() == true)
    {
        if (_method == "GET")
        {
            const std::vector<LocationConfig> &locs = _serverConfig.getLocations();

            for (size_t i = 0; i < locs.size(); i++)
            {
                const std::vector<std::string> &indexVector = locs[i].getIndex();
                if (indexVector.size() > 0){
                    for (size_t j = 0; j < indexVector.size(); j++)
                    {
                        struct stat sb;
                        std::string locRoot = locs[i].getRoot().empty() ? _serverConfig.getRoot() : locs[i].getRoot();
                        std::string index = locRoot + "/" + indexVector[j];
                        if (stat(index.c_str(), &sb) == 0) {
                            _fileName = indexVector[j];
                            std::cout << "_filename:" <<_fileName << std::endl;
                            _code = HTTP_FILE;
                            break ;
                        }
                    }
                }
                else {
                    _code = HTTP_INDEX;
                }
            }

            {
                struct stat path_stat;
                if (stat(_fullPath.c_str(), &path_stat) != -1 && S_ISDIR(path_stat.st_mode))
                {
                    std::string indexPath = _fullPath;
                    if (indexPath[indexPath.size() - 1] != '/')
                        indexPath += "/";
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

            if (_requesttarget.find("/upload") != std::string::npos)
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
                char const *lastPoint = strrchr(_requesttarget.c_str(), '.');
                if (lastPoint)
                    _type = addSuffix(std::string(lastPoint, strlen(lastPoint)));
                else
                    _type = "text/html";

                return true;
            }
        
        }
        else if (_method == "POST") {

            if (_requesttarget.rfind("/upload", 0) == 0)
            {
                // _requesttarget = "/upload/../../../../etc/cron.d/evil";
                if (_requesttarget.find("..") != std::string::npos) {
                    LOG_ERROR("POST upload: path traversal attempt");
                    return false;
                }

                if (_requesttarget == "/upload") {
                    
                    if (_isContentTypeFound == false) {
                        LOG_ERROR("POST upload: missing Content-Type");
                        return false;
                    }
                    if (_isContentLengthFound == false) {
                        LOG_ERROR("POST upload: missing Content-Length");
                        return false;
                    }
                    size_t curPos = 0;
                    if (checkContentDisposition(curPos) == false) {
                        LOG_ERROR("POST upload: missing Content-Disposition");
                        return false;
                    }
                    if (gatherFile(curPos) == false) {
                        LOG_ERROR("POST upload: failed to gather file content");
                        return false;
                    }
                    return true;

                } 
                else 
                {
                    char const *lastSlash = strrchr(_requesttarget.c_str(), '/');
                    if (doesCharCExist400(lastSlash) == false)
                        return false;
                    
                    char const *lastPoint = strrchr(_requesttarget.c_str(), '.');
                    if (doesCharCExist400(lastPoint) == false || lastPoint < lastSlash)
                        return false;

                    std::string name = std::string(lastSlash, strlen(lastSlash));
                    name.erase(name.begin());

                    if (name.empty())
                        return LOG_ERROR("POST upload: empty filename"), false;

                    // ------------ Debug ------------
                    LOG_DEBUG("POST upload target: " + name);

                    _fileName = name;
                    _code = HTTP_FILE;
                    std::string suffix = std::string(lastPoint, strlen(lastPoint));
                    _type = addSuffix(suffix);

                    return true;
                }

            }
        
        } 
        else if (_method == "DELETE") {
            
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

                // ------------ Debug ------------
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
