/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:03:45 by lzannis           #+#    #+#             */
/*   Updated: 2026/08/11 18:18:31 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../lexer/Lexer.hpp"
#include "../parser/config/ServerConfig.hpp"
#include <sstream>

#include "ListenerManager.hpp"
#include "HTTPParser.hpp"
#include "RequestHandler.hpp"
#include "Server.hpp"
#include <dirent.h> 

/*
** ============================================================================
** The Rule of Three
** ============================================================================
*/

RequestHandler::RequestHandler(
    std::string const & request,
    const ServerConfig & serverConfig) :
    _request(request), _serverConfig(serverConfig),
    _root(serverConfig.getRoot()), _header(), _size(),
    _pathToFile(), _n_read_index(0), _isCGI(false),
    _fullPath(), _query_string(), _scriptFilename(),
    _body(), _content_type(), _content_length(),
    _method()
{
    memset(_buffer, 0, BUF_SIZE);
}

RequestHandler::RequestHandler( RequestHandler const & src ) :
    _request(src._request), _serverConfig(src._serverConfig),
    _root(src._root), _header(src._header),
    _size(src._size), _pathToFile(src._pathToFile),
    _n_read_index(src._n_read_index), _isCGI(src._isCGI),
    _fullPath(src._fullPath), _query_string(src._query_string),
    _scriptFilename(src._scriptFilename),
    _body(src._body), _content_type(src._content_type),
    _content_length(src._content_length),
    _method(src._method)
{
    memcpy(_buffer, src._buffer, BUF_SIZE);
}

RequestHandler::~RequestHandler() {}

RequestHandler & RequestHandler::operator=( RequestHandler const & other )
{
    if ( this != &other)
    {
        _request        = other._request;
        _root           = other._root;
        _header         = other._header;
        _size           = other._size;
        _pathToFile     = other._pathToFile;

        memcpy(_buffer, other._buffer, BUF_SIZE);

        _n_read_index   = other._n_read_index;
        _isCGI          = other._isCGI;
        _fullPath       = other._fullPath;
        _query_string   = other._query_string;
        _scriptFilename = other._scriptFilename;
        _body           = other._body;
        _content_type   = other._content_type;
        _content_length = other._content_length;
        _method = other._method;
    }

    return *this;
}

/*
** ============================================================================
** Getters & Setters
** ============================================================================
*/

// ── buffer ──────────────────────────────────────────────────────────────────
std::string RequestHandler::getBuffer() const
{
    return std::string(_buffer, _n_read_index);
}

// ── header ──────────────────────────────────────────────────────────────────
std::string RequestHandler::getHeader() const
{
    return _header;
}

// ── size ────────────────────────────────────────────────────────────────────
std::string RequestHandler::getSize() const
{
    return _size;
}

// ── read index ──────────────────────────────────────────────────────────────
ssize_t RequestHandler::getNReadIndex() const
{
    return _n_read_index;
}

// ── cgi ─────────────────────────────────────────────────────────────────────
bool RequestHandler::getCGI() const
{
    return _isCGI;
}

// ── path ─────────────────────────────────────────────────────────────────────
std::string RequestHandler::getPath() const
{
    return _fullPath;
}

// ── filename ────────────────────────────────────────────────────────────────
std::string RequestHandler::getFilename() const
{
    return _scriptFilename;
}

// ── query string ────────────────────────────────────────────────────────────
std::string RequestHandler::getQueryString() const
{
    return _query_string;
}

// ── body ────────────────────────────────────────────────────────────────────
std::string RequestHandler::getBody() const
{
    return _body;
}
// ── content type ────────────────────────────────────────────────────────────
std::string RequestHandler::getContentType() const
{
    return _content_type;
}
// ── content length ──────────────────────────────────────────────────────────
std::string RequestHandler::getContentLength() const
{
    return _content_length;
}

// ── method ──────────────────────────────────────────────────────────────────
std::string RequestHandler::getMethod() const
{
    return _method;
}

/*
** ============================================================================
** Request Handler
** ============================================================================
*/

static std::string resolveRoot(const ServerConfig &cfg, const std::string &uri)
{
    const std::vector<LocationConfig> &locs = cfg.getLocations();
    size_t bestLen = 0;
    std::string root;
    for (size_t i = 0; i < locs.size(); ++i)
    {
        const std::string &path = locs[i].getPath();
        if (uri.find(path) == 0 && path.size() > bestLen
            && (path == "/" || uri.size() == path.size() || uri[path.size()] == '/' || uri[path.size()] == '?'))
        {
            bestLen = path.size();
            root = locs[i].getRoot();
        }
    }

    return root;
}

std::string RequestHandler::getFile( std::string const & code, bool const & error ){

    std::string file;
    if (error == true){
        
        file = "data/errors/";
        file += code;
        file += ".";
        file += "html";
    }
    else {
        
        file = _root;
        file += "/";
        file += code;
    }
 
    LOG_DEBUG("Serving file: " + file);

    return file;
}

std::string RequestHandler::getFileImage( std::string const & code){
    
    std::string file = "data/www/images";
    file += "/";
    file += code;
    LOG_DEBUG("Serving image: " + file);
    return file;
}

std::string RequestHandler::getFileUpload( std::string const & code){

    std::string file = "data/upload";
    file += "/";
    file += code;
    LOG_DEBUG("Serving upload: " + file);
    return file;
}
 
// construct message to send back to client 
//  header : code + Content-Type
std::string RequestHandler::buildAnswerHeader( std::string const & code, std::string const & type ){
    
    LOG_DEBUG("Building response header, code: " + code);

    std::string codeName[12] =
    {
        "400",
        "403",
        "404",
        "405",
        "411",
        "413",
        "414",
        "421",
        "201",
        "204",
        "500",
        "502"
    };
    
    int index = -1;
    for (int i = 0 ;i < 11; i++){
        
        if (codeName[i] == code){
            index = i;
            break;
        }
    }
    
    std::string str;

    switch (index) {
        
        case(0):
        str = "400 BAD REQUEST";
        break;

        case(1):
        str = "403 FORBIDDEN";
        break;
        
        case(2):
        str = "404 Not Found";
        break;

        case(3):
        str = "405 METHOD NOT ALLOWED\r\nAllow: GET, POST, DELETE";
        break;

        case(4):
        str = "411 LENGTH REQUIRED";
        break;
        
        case(5):
        str = "413 CONTENT TOO LARGE";
        break;
        
        case(6):
        str = "414 URI TOO LONG";
        break;
        
        case(7):
        str = "421 MISDIRECTED REQUEST";
        break;
        
        case(8):
        {
            std::string loc = _pathToFile;
            size_t pos = loc.find("data/upload/");
            if (pos != std::string::npos)
                loc.replace(pos, std::string("data/upload").size(), "/upload");
            str = "201 CREATED\r\nLocation: " + loc;
        }
        break;
        
        case(9):
        str = "204 NO CONTENT";
        break;

        case(10):
        str = "500 INTERNAL SERVER ERROR";
        break;

        case(11):
        str = "502 BADGATEWAY";
        break;

        default:
        str = "200 OK";
        break;
        
    }
    
    _header = "HTTP/1.1 ";
    _header += str;
    _header += "\r\n";
    _header += "Content-Type: ";
    _header += type;
    _header += "\r\n";
    _header += "Content-Length: ";
    
    std::stringstream ss;
    ss << _n_read_index;
    std::string size = ss.str();
    _header += size;
    
    _header += "\r\n\r\n";
    
    return _header;
}

// open file + stock it in buffer to send back to client
// content = text
bool    RequestHandler::answerFile( std::string const & file ){

    struct stat sb;
    
    if (stat(file.c_str(), &sb) == -1)
    {
        LOG_ERROR("stat failed: " + file + " - " + strerror(errno));
        return false;
    }
    if (S_ISDIR(sb.st_mode))
    {
        LOG_DEBUG("Path is a directory: " + file);
        return false; 
    }
    std::ostringstream dbg; dbg << "File size: " << sb.st_size;
    LOG_DEBUG(dbg.str());
    std::cout << "YOOO" << std::endl;
    int indexfd = open(file.c_str(), O_RDONLY);
    if (indexfd == -1)
    {
        LOG_ERROR("Failed to open file: " + file + " - " + strerror(errno));
        return false;
    }
    _n_read_index = read(indexfd, _buffer, BUF_SIZE);
    close(indexfd);
    if (_n_read_index == -1)
        return false;

    return true;

}


// open file + stock it in buffer to send back to client
// content = x-icon
bool    RequestHandler::answerFileIcon(){

    struct stat sb;
    
    if (stat("data/www/favicon.ico/favicon-16x16.png", &sb) == -1){
        LOG_ERROR("stat failed for favicon: " + std::string(strerror(errno)));
        return false;
    }

    int indexfd = open("data/www/favicon.ico/favicon-16x16.png", O_RDONLY);
    if (indexfd == -1){
        LOG_ERROR("Failed to open favicon: " + std::string(strerror(errno)));
        return false;
    }
    _n_read_index = read(indexfd, _buffer, BUF_SIZE);
    close(indexfd);
    if (_n_read_index == -1)
        return false;

    return true;
}


bool    RequestHandler::uploadFile( std::string const & filename, std::string const & buf ){
    
    _pathToFile = "data/upload/" + filename;
    
    LOG_INFO("Uploading file to: " + _pathToFile);

    std::ofstream outfile(_pathToFile.c_str(), std::ios::binary);
    if (!outfile){
        LOG_ERROR("Failed to open upload file: " + std::string(strerror(errno)));
        return false;
    }
    outfile.write(buf.data(), buf.size());

    if (!outfile.good()){
        LOG_ERROR("Failed to write upload file: " + std::string(strerror(errno)));
        return false;
    }
    _n_read_index = buf.size();

    outfile.close();

    struct stat sb;

    if (stat(_pathToFile.c_str(), &sb) == -1){
        LOG_ERROR("stat failed for uploaded file: " + std::string(strerror(errno)));
        return false;
    }

    std::ostringstream dbg;
    dbg << "Uploaded file size: " << sb.st_size;
    LOG_DEBUG(dbg.str());

    return true;
}

bool    RequestHandler::removeFile( std::string const & filename ){
    
    _pathToFile = filename;
    
    LOG_INFO("Removing file: " + _pathToFile);

    struct stat sb;

    if (stat(_pathToFile.c_str(), &sb) == -1){
        LOG_ERROR("stat failed for file to delete: " + std::string(strerror(errno)));
        return false;
    }

    if (remove(_pathToFile.c_str()) < 0 ){
        LOG_ERROR("Failed to delete file: " + std::string(strerror(errno)));
        return false;
    }
    
    return true;
}

std::string RequestHandler::generateAutoindex(const std::string &fullPath, const std::string &requestTarget)
{
    DIR *dir = opendir(fullPath.c_str());
    if (dir == NULL)
        return ("");
    // and then after should be 403? or 500, is it an error unexpected though?
    std::string html;
    html += "<!DOCTYPE html>\n<html>\n<head><title>Index of ";
    html+= requestTarget; // or requestTarget - is it same thing here?
    html += "</title></head>\n<body>\n<h1>Index of ";
    html += requestTarget;
    html += "</h1>\n<hr>\n<ul>\n";

    // format of directory entries, useful to grab all the files that are existing, girl
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        if (name == ".")
            continue;
        std::string entryPath = fullPath;  // check entry a dir
        if (entryPath[entryPath.size() - 1] != '/')
            entryPath += "/";
        entryPath += name;
        
        struct stat entry_stat;
        bool isDir = (stat(entryPath.c_str(), &entry_stat) == 0 
            && S_ISDIR(entry_stat.st_mode));
        html = html + "<li><a href=\"";
        html += name;
        if (isDir)
            html += "/"; // this = trailing slash on directory
        html += "\">";
        html += name;
        if (isDir)
            html += "/";
        html += "</a></li>\n";
    }
    closedir(dir);

    html += "</ul>\n<hr>\n</body>\n</html>\n";
    return html;
}

void RequestHandler::sendError( HTTPParser & parser, HttpCode code ) {
    
    parser.setError(true);
    parser.setCode(code);
    parser.setType("text/html");
    std::string file = getFile(HTTPParser::httpCodeToString(parser.getCode()), parser.getError());
    answerFile(file);
}

/**
** @brief Handles a full HTTP request from parsing to response.
**
** Instantiates HTTPParser, validates the request, dispatches to
** the right handler (CGI, DELETE, POST, GET, autoindex, range…),
** then builds the response header.
**
** @param listen  the listener that accepted this client
** @return        true on success, false on unrecoverable error
**/
bool RequestHandler::handleRequest(  ListenerManager const & listen ) {
    
    HTTPParser HTTPParser(_request, _serverConfig);

    if (HTTPParser.isRequestValid(listen) == false)
    {
        sendError(HTTPParser, HTTPParser.getCode());
        buildAnswerHeader(HTTPParser::httpCodeToString(HTTPParser.getCode()), "text/html");
        return true;

    } 
    else if (HTTPParser.isCGI()) 
    {

        if (HTTPParser.validateCGIRequest() == false)
        {
            LOG_ERROR("CGI request validation failed");
            sendError(HTTPParser, HTTP_502);
            buildAnswerHeader("502", "text/html");
            return true;
        }
        if (HTTPParser.getMethod() != "GET" || HTTPParser.getMethod() != "POST")
        {
            LOG_ERROR("CGI request validation failed: Need GET or POST as method");
            sendError(HTTPParser, HTTP_405);
            buildAnswerHeader("405", "text/html");
            return true;
        }

        _scriptFilename = HTTPParser.getScriptFilename();
        _fullPath = "data/" + _scriptFilename;
        _query_string = HTTPParser.getQueryString();
        _body = HTTPParser.getBody();
        _content_type = HTTPParser.getContentType();

        std::ostringstream oss;
        oss << HTTPParser.getContentLength();
        _content_length = oss.str();

        _method = HTTPParser.getMethod();
        _isCGI = true;

        return true;

    } 
    else if (HTTPParser.findMethods() == false) {

        LOG_ERROR("Method not implemented");
        sendError(HTTPParser, HTTPParser.getCode());
        buildAnswerHeader(HTTPParser::httpCodeToString(HTTPParser.getCode()), "text/html");
        return true;

    }

    if (_root.empty())
        _root = resolveRoot(_serverConfig, HTTPParser.getRequestTarget());


    bool rangeHandled = false;

    if (HTTPParser.getMethod() == "DELETE")
    {
        
        if (HTTPParser.getError() == true) {

            std::string file = getFile(HTTPParser::httpCodeToString(HTTPParser.getCode()),
                HTTPParser.getError());
            answerFile(file);

        } else {

            std::string file = getFileUpload(HTTPParser.getFileName());

            if (removeFile(file) == false)
                sendError(HTTPParser, errno == ENOENT ? HTTP_404 : HTTP_500);
            else
            {
                HTTPParser.setCode(HTTP_204);
                HTTPParser.setType("text/html");
            }
        }

    } 
    else if (HTTPParser.getMethod() == "POST") {

        if (uploadFile(HTTPParser.getFileName(),
            HTTPParser.getFileBuf()) == false)
            sendError(HTTPParser, HTTP_500);
        else {

            HTTPParser.setCode(HTTP_201);
            // TODO: set _content_type from HTTPParser.getType() if needed
        }
    } 
    else if (HTTPParser.getCode() == HTTP_AUTOINDEX) 
    {
        // HTTPParser.getPath() is the resolved on-disk directory (root + URI).
        _body = generateAutoindex(HTTPParser.getPath(),
            HTTPParser.getRequestTarget());
        if (_body.empty()) {
            std::cout << ":P1" << std::endl;
            HTTPParser.setError(true);
            HTTPParser.setCode(HTTP_403); // if real dir didn't open
            HTTPParser.setType("text/html");
        } 
        else {

            // put the listing where epoll reads the response body
            // (getBuffer()/getNReadIndex()) and let Content-Length mirror
            if (_body.size() > (size_t)BUF_SIZE)
                _body.resize(BUF_SIZE);
            memcpy(_buffer, _body.data(), _body.size());
            _n_read_index = (ssize_t)_body.size();
            std::cout << "hello" << std::endl;
        }
        std::cout << ":P:" << std::endl;
    } 
    else if (HTTPParser.getType() == "text/html"
        || HTTPParser.getType() == "text/css"
        || HTTPParser.getType() == "text/javascript"
        || HTTPParser.getType() == "application/javascript") 
    {
        if (HTTPParser.getCode() == HTTP_AUTOINDEX)
        {
            std::cout << "are you realyl ayuto" << std::endl;
        }
        std::string file = HTTPParser.getError()
            ? getFile(HTTPParser::httpCodeToString(HTTPParser.getCode()), true)
            : getFile(HTTPParser.getFileName(), false);
        if (answerFile(file) == false)
            sendError(HTTPParser);
    } 
    else if (HTTPParser.getType() == "text/plain") 
    {

        std::string file = getFileUpload(HTTPParser.getFileName());
        if (answerFile(file) == false)
            sendError(HTTPParser, HTTP_500);

    } 
    else if (HTTPParser.getType() == "image/jpeg"
        || HTTPParser.getType() == "image/png"
        || HTTPParser.getType() == "image/gif"
        || HTTPParser.getType() == "image/webp") {

        std::string file = getFileImage(HTTPParser.getFileName());

        //  206 Partial Content path (only range set)
        struct stat sb;
        std::string range = HTTPParser.getRange();
        if (!range.empty() && stat(file.c_str(), &sb) == 0)
        {
            ByteRange r = parseRangeHeader(range, (long)sb.st_size);
            if (r.unsatisfiable)
            {
                build416Header((long)sb.st_size);
                rangeHandled = true;
            }
            else if (r.valid && answerFilePartial(file, r))
            {
                buildPartialHeader(HTTPParser.getType(), r, (long)sb.st_size);
                rangeHandled = true;
            }
        }
        if (HTTPParser.getUpload() == true) {

            std::string uploadFile = getFileUpload(HTTPParser.getFileName());
            if (answerFile(uploadFile) == false)
                sendError(HTTPParser, HTTP_500);

        }
        else {

            std::string imgFile = getFileImage(HTTPParser.getFileName());
            if (answerFile(imgFile) == false)
                sendError(HTTPParser, HTTP_500);
        }

    } else if (HTTPParser.getType() == "image/x-icon") {

        if (answerFileIcon() == false)
            sendError(HTTPParser, HTTP_500);

    } else if (HTTPParser.getType() == "multipart/form-data") {

        if (uploadFile(HTTPParser.getFileName(),
            HTTPParser.getFileBuf()) == false)
            sendError(HTTPParser, HTTP_500);
        else
        {
            HTTPParser.setCode(HTTP_201);
            char const *lastPoint =
                strrchr(HTTPParser.getFileName().c_str(), '.');
            if (lastPoint)
            {
                std::string suffix(lastPoint, strlen(lastPoint));
                HTTPParser.setType(HTTPParser.addSuffix(suffix));
            }
        }
    }

    if (!rangeHandled) // a 206/416 header was already built by the range path
        buildAnswerHeader(HTTPParser::httpCodeToString(HTTPParser.getCode()), HTTPParser.getType());

    return true;
}

// SO GIRLS: flow is set at the top of handleRequest()'s file branches:
//  HTTPParser.getRange() gives the raw "Range:" value ("", if none)
//  parseRangeHeader(value, fileSize) -> ByteRange
//  r.unsatisfiable -> send build416Header(); r.valid -> answerFilePartial 
//+ buildPartialHeader (206); otherwise fall through to the REGULar 200 // 
//  single range only; multipart/multi-range (chec k with other teams but it would be such a pain uguys)
// Parse a single "bytes=..." range against a known file size.
// supported forms are the follw: "bytes=start-end", "bytes=start-" (to EOF), "bytes=-suffix".
// Range : start - end
// range : start - EOF
// range : -N bytes

RequestHandler::ByteRange RequestHandler::parseRangeHeader(std::string const& rangeValue, long fileSize)
{
    ByteRange r;

    // only the bytes unit is ok; anything else considered no range (200).
    const std::string prefix = "bytes=";
    if (rangeValue.compare(0, prefix.size(), prefix) != 0)
        return r;
    std::string spec = rangeValue.substr(prefix.size());

    size_t dash = spec.find('-');
    if (dash == std::string::npos)
        return r; // malformed ; ignore, serve 200

    std::string startStr = spec.substr(0, dash);
    std::string endStr = spec.substr(dash + 1);

    long start;
    long end;
    if (startStr.empty()) {

        // suffix form "-N" - last N bytes
        if (endStr.empty())
            return r;
        long suffix = atol(endStr.c_str());
        if (suffix <= 0)
        {
            r.unsatisfiable = true; // 416
            return r;
        }
        if (suffix > fileSize)
            suffix = fileSize;
        start = fileSize - suffix;
        end = fileSize - 1;

    } else {
        start = atol(startStr.c_str());
        end = endStr.empty() ? fileSize - 1 : atol(endStr.c_str());
        if (end > fileSize - 1)
            end = fileSize - 1; // it's the clamp of EOF
    }

    if (start < 0 || start >= fileSize || start > end)
    {
        // 416; if start > end or start >= fileSize
        r.unsatisfiable = true;
        return r;
    }

    r.start = start;
    r.end = end;
    r.valid = true;
    return r;
}

// read bytes (r.start to r.end) of FILE into _buffer and set _n_read_index to that length, so RS sends only the slice
bool RequestHandler::answerFilePartial(std::string const & file, ByteRange const & r)
{
    long length = r.end - r.start + 1;
    if (length <= 0 || length > BUF_SIZE)
    {
        std::ostringstream oss;
        oss << "Partial range out of bounds: " << length;
        LOG_ERROR(oss.str());

        return false;
    }

    int fd = open(file.c_str(), O_RDONLY);
    if (fd == -1)
    {
        LOG_ERROR("Failed to open file for partial read: "
            + std::string(strerror(errno)));
        return false;
    }
    if (lseek(fd, r.start, SEEK_SET) == (off_t)-1)
    {
        LOG_ERROR("lseek failed: " + std::string(strerror(errno)));
        close(fd);
        return false;
    }
    _n_read_index = read(fd, _buffer, length);
    close(fd);
    if (_n_read_index != length)
    {
        LOG_ERROR("Short partial read");
        return false;
    }
    return true;
}

std::string RequestHandler::buildPartialHeader(std::string const & type, ByteRange const & r, long fileSize)
{
    std::stringstream ss;
    ss << "HTTP/1.1 206 Partial Content\r\n"
       << "Content-Type: " << type << "\r\n"
       << "Accept-Ranges: bytes\r\n"
       << "Content-Range: bytes " << r.start << "-"
       << r.end << "/" << fileSize << "\r\n"
       << "Content-Length: " << (r.end - r.start + 1) << "\r\n\r\n";
    _header = ss.str();
    return _header;
}

std::string RequestHandler::build416Header(long fileSize)
{
    std::stringstream ss;
    ss << "HTTP/1.1 416 Range Not Satisfiable\r\n"
       << "Content-Range: bytes */" << fileSize << "\r\n"
       << "Content-Length: 0\r\n\r\n";
    _header = ss.str();
    _n_read_index = 0;
    return _header;
}
