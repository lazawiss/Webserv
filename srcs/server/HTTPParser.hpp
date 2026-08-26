/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 15:54:09 by lzannis           #+#    #+#             */
/*   Updated: 2026/08/26 16:02:22 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#include "../lexer/Lexer.hpp"
#include "ListenerManager.hpp"
#include "../parser/config/ServerConfig.hpp"

#define SERVER_OK     0
#define SERVER_ERROR -1

enum ConnectionType
{
    CONN_NONE,
    CONN_CLOSE,
    CONN_KEEP_ALIVE
};

enum HttpCode
{
    HTTP_301,
    HTTP_302,
    HTTP_400,
    HTTP_403,
    HTTP_404,
    HTTP_405,
    HTTP_413,
    HTTP_421,
    HTTP_201,
    HTTP_204,
    HTTP_500,
    HTTP_502,
    HTTP_CGI,
    HTTP_INDEX,
    HTTP_AUTOINDEX,
    HTTP_FAVICON,
    HTTP_FILE
};

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <limits>



typedef enum RequestParser
{
    CONTENT_TYPE,
    CONTENT_LENGTH,
    CONNECTION,
    HOST,
    RANGE,

    UNKNOWN,

} RequestParser ;

class HTTPParser
{

private:

    std::string                 _request;           // raw request received from the buffer
    const ServerConfig          &_serverConfig;     // server config matched to this request
    std::string                 _httpRoot;             // get _root from RequestHandler
    HttpCode                    _code;              // HTTP response code to send
    std::string                 _type;              // MIME type for the response
    std::string                 _method;            // GET || POST || DELETE
    std::string                 _requesttarget;     // path from the request line
    std::string                 _httpversion;       // HTTP/1.1
    std::string                 _boundary;          // multipart boundary string
    std::string                 _fileLength;        // raw Content-Length string (upload)
    std::string                 _fileName;          // filename extracted from the request
    std::string                 _fileBuf;           // binary content of the uploaded file
    bool                        _errors;            // true if a parsing error occurred
    bool                        _upload;            // true if this is a file upload request
    bool                        _isIndex;           // true if this is file is an index
    size_t                     _httpMaxbodysize;   // get client-max-body-size form Location < Server < Global 
                                                    // + translate str into ssize_t




    size_t                      _content_length;    // validated Content-Length value
    ConnectionType              _connectionType;    // keep-alive || close
    std::string                 _host;              // Host header value (host:port)

    bool                        _isCGI;             // true if target is under /cgi-bin/
    std::string                 _fullPath;          // resolved filesystem path
    std::string                 _query_string;      // query string from CGI URL (after '?')
    std::string                 _scriptFilename;    // CGI script path (before '?')
    std::string                 _body;              // everything after the first \r\n\r\n
    std::string                 _content_type;      // Content-Type header value
    int                         _content_int;       // Content-Length as integer (CGI)
    std::string                 _pathInfo;          // CGI PATH_INFO: URI left over after the script
    std::string                 _scriptName;        // CGI SCRIPT_NAME: URL path of the script itself
    std::string                 _cgiInterpreter;    // interpreter taken from the location's cgi_extension

    bool                        _autoindexOn;       // true if autoindex is enabled for the matched location
    std::string                 _rangeHeader;       // Range header value (bytes=X-Y)
    bool                        _isContentLengthFound;
    bool                        _isHostFound;
    bool                        _isContentTypeFound;

    std::string                 _fileContentType;

public:

                                HTTPParser( std::string const & request, const ServerConfig &serverConfig  );
                                HTTPParser( HTTPParser const & src );    
                                ~HTTPParser();    
    HTTPParser &                operator=( HTTPParser const & other );
            
    HttpCode                    getCode() const;
    std::string                 getHttpRoot() const;
    std::string                 getType() const;
    std::string                 getMethod() const;
    std::string                 getBoundary() const;
    std::string                 getFileName() const;
    std::string                 getFileBuf() const;
    bool                        getError() const;
    bool                        getUpload() const;
    bool                        getIsIndex() const;
    std::string                 getPath() const;
    std::string                 getPathInfo() const;
    std::string                 getScriptName() const;
    std::string                 getInterpreter() const;
    std::string                 getScriptFilename() const;
    std::string                 getQueryString() const;
    std::string                 getBody() const;
    std::string                 getContentType() const;
    size_t                      getContentLength() const;
    std::string                 getRequestTarget() const;
    std::string                 getRange() const;
    bool                        getAutoindexOn() const;

    HttpCode                    setCode( HttpCode code );
    std::string                 setFileName(std::string filename);
    std::string                 setHttpRoot( std::string httpRoot );
    std::string                 setType( std::string const & type );
    bool                        setError( bool error );    

    bool                        isCGI() const;

    void                        buildFullPath();
    bool                        resolveRoot();

    void                        checkRange( std::string const & value );

    void                        parseCGI();
    bool                        validateCGIRequest();
    bool                        buildCGIPath();
    static int                  matchLocation(const std::vector<LocationConfig> &locs, const std::string &uri);

    bool                        varNotFound400 ( size_t var );
    bool                        doesCharCExist400 ( char const *str );
    
    bool                        checkSize();
    bool                        checkRequestLine();
    int                         checkConnection(std::string const & value);
    int                         checkHost(std::string const & value, ListenerManager const & listener);
    bool                        validateHost(std::string const & value, std::string & listen);
    bool                        matchHost(std::string const & host, ListenerManager const & listener);

    bool                        isRequestValid( ListenerManager const & listen );
                
    bool                        checkContentDisposition( size_t & curPos );
    bool                        gatherFile( size_t curPos );
    
    std::string                 addSuffix(std::string suffix);
    bool                        compareMethodWithConfigFile(const std::vector<LocationConfig> &locs, int bestIdx);
    bool                        isRedir();
    bool                        findMethods(const std::vector<LocationConfig> &locs, int bestIdx);
    int                         findAutoIndex(const std::vector<LocationConfig> &locs, int bestIdx);

    static std::string          httpCodeToString( HttpCode code );

    int                         checkContentLength(std::string const & value);
    int                         checkContentType(std::string const & value);

    static bool                 containsCaseInsensitive( std::string const & haystack, std::string const & needle );
    void                        resolveConnectionType();
    static size_t               parseBodySize( std::string const & s );
    size_t                      getEffectiveBodyLimit() const;
};
