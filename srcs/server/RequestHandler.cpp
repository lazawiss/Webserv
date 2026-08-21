/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:03:45 by lzannis           #+#    #+#             */
/*   Updated: 2026/08/21 14:22:15 by lzannis          ###   ########.fr       */
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
    _method(), _pathInfo(), _scriptName(), _interpreter()
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
    _method(src._method), _pathInfo(src._pathInfo),
    _scriptName(src._scriptName), _interpreter(src._interpreter)
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
        _pathInfo       = other._pathInfo;
        _scriptName     = other._scriptName;
        _interpreter    = other._interpreter;
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

// ── cgi: PATH_INFO / SCRIPT_NAME / interpreter ──────────────────────────────
std::string RequestHandler::getPathInfo() const
{
    return _pathInfo;
}

std::string RequestHandler::getScriptName() const
{
    return _scriptName;
}

std::string RequestHandler::getInterpreter() const
{
    return _interpreter;
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

// bool RequestHandler::resolveRoot(const ServerConfig &cfg, HTTPParser HTTPParser)
// {
//     const std::vector<LocationConfig> &locs = cfg.getLocations();
//     std::string root;
//     std::string uri = HTTPParser.getRequestTarget();

//     LOG_INFO(COLOR_PINK + std::string("URI: ") + uri + COLOR_RESET);

//     char const *lastSlash = strrchr(uri.c_str(), '/');
//     if (!lastSlash){
        
//         HTTPParser.setError(true);
//         HTTPParser.setCode(HTTP_400);
//         HTTPParser.setType("text/html");
        
//         return false;
//     }
//     LOG_INFO(COLOR_PINK + std::string("lasttSlash: ") + lastSlash + COLOR_RESET);
    
//     int len = strlen(lastSlash);
//     std::cout << "len: " << len << std::endl;
   
//     int urilen = uri.size(); 
//     std::cout << "urilen: " << urilen << std::endl;
    
//     std::string newSlash = uri.substr(0, urilen - len);
//     LOG_INFO(COLOR_PINK + std::string("newSlash : ") + newSlash  + COLOR_RESET);

    
//     for (size_t i = 0; i < locs.size(); ++i)
//     {
//         const std::string &path = locs[i].getPath();
//         LOG_INFO(COLOR_CYAN + std::string("path: ") + path + COLOR_RESET);
      
//         if (uri.find(path) == 0 )
//         {
//             // pathLen = path.size();
//             root = locs[i].getRoot();
//             LOG_INFO(COLOR_CYAN + std::string("root: ") + root + COLOR_RESET);

//             char const *lastSlashRoot = strrchr(root.c_str(), '/');
//             if (!lastSlashRoot){
//             LOG_INFO(COLOR_RED + std::string("NO lastSlashRoot: ") + COLOR_RESET);
                
//                 HTTPParser.setError(true);
//                 HTTPParser.setCode(HTTP_400);
//                 HTTPParser.setType("text/html");
//                 return false;
//             }
        
//             LOG_INFO(COLOR_PINK + std::string("lastSlashRoot: ") + lastSlashRoot + COLOR_RESET);

//             if (newSlash.compare(lastSlashRoot) == 0)
//             {
//                 std::cout << "C EST UN MATCH" << std::endl;
//                 _root = locs[i].getRoot();
//                 break;
//             }
//         }
//     }

    
//     LOG_INFO(COLOR_PINK + std::string("ROOT: ") + _root + COLOR_RESET);

//     // MARQUE
//     if (_root.empty())
//     {
//         // root = bestLen->getRoot();
//         _root = _fullPath;
//         LOG_INFO(COLOR_CYAN + std::string("ROOT (buildFullPath(1)): ") + _root + COLOR_RESET);
//     }

//     std::string filename = std::string(lastSlash,len);
//     if (filename[0] == '/'){
//         filename.erase(filename.begin());
//         LOG_INFO(COLOR_PINK + std::string("filename after match: ") + filename + COLOR_RESET);
        
//         HTTPParser.setFileName(filename);
        
//         LOG_INFO(COLOR_CYAN + std::string("_filename after match: ") +  HTTPParser.getFileName() + COLOR_RESET);
//     }
//         LOG_INFO(COLOR_CYAN + std::string("_filename after match2: ") +  HTTPParser.getFileName() + COLOR_RESET);

//     return true;

// }

std::string RequestHandler::getFile( std::string const & code, bool const & error ){

    std::string file;
    if (error == true){
        
        file = "data/errors/";
        file += code;
        file += ".";
        file += "html";
    }
    else {
        
        LOG_INFO(COLOR_PINK + std::string("filename getFile: ") + code + COLOR_RESET);
        
        file = _root;
        file += "/";
        file += code;

        std::cout << "FULL PATH FROM get FILE " << code << std::endl;
    }

    // ------------ Debug ------------
    LOG_DEBUG("Serving file: " + file);

    return file;
}

std::string RequestHandler::getFileImage( std::string const & code){
    
    std::string file = "data/www/images";
    file += "/";
    file += code;
    // ------------ Debug ------------
    LOG_DEBUG("Serving image: " + file);
    return file;
}

std::string RequestHandler::getFileUpload( std::string const & code){

    std::string file = "data/upload";
    file += "/";
    file += code;
    // ------------ Debug ------------
    LOG_DEBUG("Serving upload: " + file);
    return file;
}
 
// construct message to send back to client 
//  header : code + Content-Type
std::string RequestHandler::buildAnswerHeader( std::string const & code, std::string const & type ){
    
    // ------------ Debug ------------
    LOG_DEBUG("Building response header, code: " + code);

    std::string codeName[14] =
    {
        "301",
        "302",
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
    for (int i = 0 ;i < 14; i++){
        
        if (codeName[i] == code){
            index = i;
            break;
        }
    }
    
    std::string str;

    switch (index) {
        
        case(0):{
            
            std::string loc = _pathToFile;
            str = "301 MOVE PERMANENTLY\r\nLocation: " + loc;
        }
        break;
        
        case(1):{
            
            std::string loc = _pathToFile;
            str = "302 FOUND\r\nLocation: " + loc;
        }
        break;
        
        case(2):
        str = "400 BAD REQUEST";
        break;

        case(3):
        str = "403 FORBIDDEN";
        break;

        case(4):
        str = "404 Not Found";
        break;

        case(5):
        str = "405 METHOD NOT ALLOWED\r\nAllow: GET, POST, DELETE";
        break;
        
        case(6):
        str = "411 LENGTH REQUIRED";
        break;

        case(7):
        str = "413 CONTENT TOO LARGE";
        break;
        
        case(8):
        str = "414 URI TOO LONG";
        break;
        
        case(9):
        str = "421 MISDIRECTED REQUEST";
        break;
        
        case(10):
        {
            std::string loc = _pathToFile;
            size_t pos = loc.find("data/upload/");
            if (pos != std::string::npos)
                loc.replace(pos, std::string("data/upload").size(), "/upload");
            str = "201 CREATED\r\nLocation: " + loc;
        }
        break;
        
        case(11):
        str = "204 NO CONTENT";
        break;

        case(12):
        str = "500 INTERNAL SERVER ERROR";
        break;

        case(13):
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

     LOG_INFO(COLOR_PINK + std::string("file dans AnswerFile(): ") + file + COLOR_RESET);
    struct stat sb;
    
    if (stat(file.c_str(), &sb) == -1)
    {
        LOG_ERROR("stat failed: " + file + " - " + strerror(errno));
        return false;
    }
    if (access(file.c_str(), R_OK) != 0)
    {
        LOG_ERROR("acccess failed: " + file + " - " + strerror(errno));
        return false;
    }

    std::ostringstream dbg; dbg << "File size: " << sb.st_size;
    // ------------ Debug ------------
    LOG_DEBUG(dbg.str());
    
    int indexfd = open(file.c_str(), O_RDONLY);
    if (indexfd == -1)
    {
        LOG_ERROR("Failed to open file: " + file + " - " + strerror(errno));
        return false;
    }
    _n_read_index = read(indexfd, _buffer, BUF_SIZE);
    close(indexfd);
    std::ostringstream oss; oss << _n_read_index;
    if (_n_read_index == -1 || _n_read_index > BUF_SIZE || _n_read_index == 0){
        
        LOG_ERROR("Failed to read file: " + oss.str() + " - " + strerror(errno));
        return false;
    }

    return true;

}


// open file + stock it in buffer to send back to client
// content = x-icon
bool    RequestHandler::answerFileIcon(){

    struct stat sb;
    
    std::string faviconPath = "data/www/favicon.ico/favicon-16x16.png";
    if (stat(faviconPath.c_str(), &sb) == -1 && S_ISREG(sb.st_mode)){
        LOG_ERROR("Stat failed for favicon: " + std::string(strerror(errno)));
        return false;
    }
    if (access(faviconPath.c_str(), R_OK) != 0)
    {
        LOG_ERROR("acccess failed: " + faviconPath + " - " + strerror(errno));
        return false;
    }

    int indexfd = open(faviconPath.c_str(), O_RDONLY);
    if (indexfd == -1){
        LOG_ERROR("Failed to open favicon: " + std::string(strerror(errno)));


        return false;
    }
    _n_read_index = read(indexfd, _buffer, BUF_SIZE);
    close(indexfd);
    if (_n_read_index == -1 || _n_read_index > BUF_SIZE || _n_read_index == 0)
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
    // _n_read_index = buf.size();
    _n_read_index = 0;
    outfile.close();

    struct stat sb;

    if (stat(_pathToFile.c_str(), &sb) == -1){
        LOG_ERROR("stat failed for uploaded file: " + std::string(strerror(errno)));
        return false;
    }

    std::ostringstream dbg;
    dbg << "Uploaded file size: " << sb.st_size;

    // ------------ Debug ------------
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
        
    std::string html;
    html += "<!DOCTYPE html>\n<html>\n<head><title>Index of ";
    html+= requestTarget;
    html += "</title></head>\n<body>\n<h1>Index of ";
    html += requestTarget;
    html += "</h1>\n<hr>\n<ul>\n";

    // format of directory entries, useful to grab all the files that are existing
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        LOG_INFO(COLOR_PINK + std::string("name generateAutoindex: ") + name + COLOR_RESET);

        if (name == ".")
            continue;
        // std::string entryPath = fullPath;
        std::string entryPath = _root;  // check entry a dir
          // check entry a dir
        LOG_INFO(COLOR_CYAN + std::string("entryPath: ") + entryPath+ COLOR_RESET);
        char const *lastSlash = strrchr(entryPath.c_str(), '/');
        if (!lastSlash){
        return "";
        }
        LOG_INFO(COLOR_PINK + std::string("lasttSlash: ") + lastSlash + COLOR_RESET);
        
        std::string dir = std::string(lastSlash, strlen(lastSlash));

        if (entryPath[entryPath.size() - 1] != '/')
            entryPath += "/";
        entryPath += name;

        LOG_INFO(COLOR_CYAN + std::string("entryPath 2: ") + entryPath+ COLOR_RESET);

        
        struct stat entry_stat;
        bool isDir = (stat(entryPath.c_str(), &entry_stat) == 0 
            && S_ISDIR(entry_stat.st_mode));

        std::cout << std::boolalpha << isDir << std::endl;

        
        html = html + "<li><a href=\"";
        html += dir;
        html += "/"; // this = trailing slash on directory
        html += name;
        html += "\">";
        html += name;
        if (isDir)
            html += "/";
        html += "</a></li>\n";
    }
    
    closedir(dir);

    html += "</ul>\n<hr>\n</body>\n</html>\n";
    LOG_INFO(COLOR_PINK + std::string("html generateAutoindex: ") + html + COLOR_RESET);
    
    return html;
}

void RequestHandler::sendError( HTTPParser & parser, HttpCode code ) {
    
    parser.setError(true);
    parser.setCode(code);
    parser.setType("text/html");
    std::string file = getFile(HTTPParser::httpCodeToString(parser.getCode()), parser.getError());
    if (answerFile(file) == false){
        if (errno == EACCES)
            parser.setCode(HTTP_403);
        if (errno == ENOENT)
            parser.setCode(HTTP_404);
    }
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
    const std::vector<LocationConfig> &locs = _serverConfig.getLocations();
    
    int bestIdx = -1;
    size_t bestLen = 0;
    for (size_t i = 0; i < locs.size(); i++)
    {
        const std::string &path = locs[i].getPath();
        if (HTTPParser.getRequestTarget().find(path) == 0 && path.size() > bestLen
            && (path == "/" || HTTPParser.getRequestTarget().size() == path.size()
                || HTTPParser.getRequestTarget()[path.size()] == '/' || HTTPParser.getRequestTarget()[path.size()] == '?'))
        {
            bestLen = path.size();
            bestIdx = (int)i;
        }
    }

    if (bestIdx == -1) {
        HTTPParser.setError(true);
        HTTPParser.setCode(HTTP_405);
        HTTPParser.setType("text/html"); 
        return false;
    }

    _root = locs[bestIdx].getRoot();
     LOG_INFO(COLOR_CYAN + std::string("_ROOT before Iscgi/findMethod(): ") + _root + COLOR_RESET);
    
    if (HTTPParser.isCGI()) 
    {
        if (HTTPParser.validateCGIRequest() == false)
        {
            LOG_ERROR("CGI request validation failed");
            sendError(HTTPParser, HTTP_502);
            buildAnswerHeader("502", "text/html");
            return true;
        }
        if (HTTPParser.getMethod() != "GET" && HTTPParser.getMethod() != "POST")
        {
            LOG_ERROR("CGI request validation failed: Need GET or POST as method");
            sendError(HTTPParser, HTTP_405);
            buildAnswerHeader("405", "text/html");
            return true;
        }

        _scriptFilename = HTTPParser.getScriptFilename();
        _fullPath = HTTPParser.getPath();
        _pathInfo = HTTPParser.getPathInfo();
        _scriptName = HTTPParser.getScriptName();
        _interpreter = HTTPParser.getInterpreter();
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
    // we check for redirection first and foremost, if not a redir, continues to static website
    else if (HTTPParser.isRedir() == true){
        
        _pathToFile = HTTPParser.getFileName();
        buildAnswerHeader(HTTPParser::httpCodeToString(HTTPParser.getCode()), HTTPParser.getType());
        return true;
    }
    
    int status = HTTPParser.findAutoIndex(locs, bestIdx);
    if (status == 2)
    {
        if (HTTPParser.getCode() == HTTP_AUTOINDEX)
        {
            // HTTPParser.getPath() is the resolved on-disk directory (root + URI).
            _body = generateAutoindex(HTTPParser.getPath(),
            HTTPParser.getRequestTarget());
            if (_body.empty()) {
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
                buildAnswerHeader(HTTPParser::httpCodeToString(HTTPParser.getCode()), "text/html");
            }
        }
        return true;
    }

    if (status == -1)
    {
        LOG_ERROR("AutoIndex is off and Index does not exist.");
        sendError(HTTPParser, HTTPParser.getCode());
        buildAnswerHeader(HTTPParser::httpCodeToString(HTTPParser.getCode()), "text/html");
        return true;
    }
    
    if (status == 1)
    {
        if (HTTPParser.findMethods(locs, bestIdx) == false) {
        LOG_ERROR("Method not implemented");
        sendError(HTTPParser, HTTPParser.getCode());
        buildAnswerHeader(HTTPParser::httpCodeToString(HTTPParser.getCode()), "text/html");
        return true;
        }
    }
    
    // if (_root.empty()){
        
    //     std::cout << "ROOT EMPTY" << std::endl;
    // if (HTTPParser.getIsIndex() == false &&  resolveRoot(_serverConfig, HTTPParser) == false){
    //     LOG_INFO(COLOR_RED + std::string("ERROR") + COLOR_RESET);
                
    //     LOG_ERROR("resolveRoot() failed");
    //     return false;
    // }
    
    LOG_INFO(COLOR_PINK + std::string("filename after resolve(): ") + HTTPParser.getFileName() + COLOR_RESET);

    if (HTTPParser.getIsIndex() == false)
        _root = HTTPParser.getHttpRoot();

    LOG_INFO(COLOR_CYAN + std::string("_root after resolve(): ") + _root + COLOR_RESET);
    
    bool rangeHandled = false;

    if (HTTPParser.getMethod() == "DELETE")
    {
        
        if (HTTPParser.getError() == true) {

            std::string file = getFile(HTTPParser::httpCodeToString(HTTPParser.getCode()),
                HTTPParser.getError());
            if (answerFile(file) == false){
                
                if (errno == EACCES)
                    sendError(HTTPParser, HTTPParser.setCode(HTTP_403));
                else if (errno == ENOENT)
                    sendError(HTTPParser, HTTPParser.setCode(HTTP_404));
                else 
                    sendError(HTTPParser, HTTPParser.setCode(HTTP_500));
            }

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

        std::string content = HTTPParser.getFileBuf();
        if (content.empty())
            content = HTTPParser.getBody();

        if (uploadFile(HTTPParser.getFileName(), content) == false)
            sendError(HTTPParser, HTTP_500);
        else {

            HTTPParser.setCode(HTTP_201);
            // TODO: set _content_type from HTTPParser.getType() if needed
        }
    } 

    else if (HTTPParser.getType() == "text/html"
        || HTTPParser.getType() == "text/css"
        || HTTPParser.getType() == "text/javascript"
        || HTTPParser.getType() == "application/javascript") 
    {
        
      
        std::string file = HTTPParser.getError()
            ? getFile(HTTPParser::httpCodeToString(HTTPParser.getCode()), true)
            : getFile(HTTPParser.getFileName(), false);

        //if (HTTPParser.getAutoindexOn() == true){
        //
        //    file = _root;
        //}
        
        if (answerFile(file) == false){
            if (errno == EACCES)
                sendError(HTTPParser, HTTPParser.setCode(HTTP_403));
            else if (errno == ENOENT)
                sendError(HTTPParser, HTTPParser.setCode(HTTP_404));
            else 
                sendError(HTTPParser, HTTPParser.setCode(HTTP_500));
        }
            
    } 
    else if (HTTPParser.getType() == "text/plain") 
    {

        std::string file = getFileUpload(HTTPParser.getFileName());
        if (answerFile(file) == false){
            
            if (errno == EACCES)
                sendError(HTTPParser, HTTPParser.setCode(HTTP_403));
            else if (errno == ENOENT)
                sendError(HTTPParser, HTTPParser.setCode(HTTP_404));
            else 
                sendError(HTTPParser, HTTPParser.setCode(HTTP_500));
        }

    } 
    else if (HTTPParser.getType() == "image/jpeg"
        || HTTPParser.getType() == "image/png"
        || HTTPParser.getType() == "image/gif"
        || HTTPParser.getType() == "image/webp") {

            
        std::string file = getFileImage(HTTPParser.getFileName());
        std::cout << "FILE: "<< file << std::endl;

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
            std::cout << "uploadFile: "<< uploadFile << std::endl;
            
            if (answerFile(uploadFile) == false){
                
                if (errno == EACCES)
                    sendError(HTTPParser, HTTPParser.setCode(HTTP_403));
                else if (errno == ENOENT)
                    sendError(HTTPParser, HTTPParser.setCode(HTTP_404));
                else 
                    sendError(HTTPParser, HTTPParser.setCode(HTTP_500));
            }

        }
        else {

            std::string imgFile = getFileImage(HTTPParser.getFileName());
            std::cout << "imgFile: "<< imgFile<< std::endl;

            if (answerFile(imgFile) == false){
                
                if (errno == EACCES)
                    sendError(HTTPParser, HTTPParser.setCode(HTTP_403));
                else if (errno == ENOENT)
                    sendError(HTTPParser, HTTPParser.setCode(HTTP_404));
                else 
                    sendError(HTTPParser, HTTPParser.setCode(HTTP_500));
            }
        }

    } else if (HTTPParser.getType() == "image/x-icon") {

        if (answerFileIcon() == false){
            
            if (errno == EACCES)
                sendError(HTTPParser, HTTPParser.setCode(HTTP_403));
            else if (errno == ENOENT)
                sendError(HTTPParser, HTTPParser.setCode(HTTP_404));
            else 
                sendError(HTTPParser, HTTPParser.setCode(HTTP_500));
        }

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
    _n_read_index = read(fd, _buffer, length); // CHECK -1 / 0
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
