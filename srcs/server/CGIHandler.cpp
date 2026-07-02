/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 19:39:04 by ankim             #+#    #+#             */
/*   Updated: 2026/07/02 19:26:34 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "HTTPParser.hpp"
# include "Server.hpp"
# include "EpollLoop.hpp"

/*ORTHODOX CANONICAL FORM*/

CGI::CGI(RequestHandler const& req, ListenerManager const & listen) : 
     _scriptFilename(req.getFilename()),
      _fullPath(req.getPath()),
      _queryString(req.getQueryString()),
      _method(req.getMethod()),
      _body(req.getBody()),
      _contentType(req.getContentType()),
      _contentLength(req.getContentLength()),
      _output(NULL) {};

CGI::CGI(const CGI& ref)
{
    _info = ref._info;
    _pid = ref._pid;
    _client_fd = ref._client_fd;
    _pipe_fd[2] = ref._pipe_fd[2];    
    _output = ref._output;

};
CGI &CGI::operator=(const CGI& ref)
{
    if (this != &ref)
    {
        this->_info = ref._info;
        this->_pid = ref._pid;
        this->_client_fd = ref._client_fd;
        this->_pipe_fd[2] = ref._pipe_fd[2];
        this->_output = ref._output;
        *this = ref;
    }
    return *this;
};
CGI::~CGI() {};

/* HELPERS */
bool    isValidCGI()
{
    
}

/* METHODS */

// env is created :

// char *env[] = {
//     method.c_str(),
//     query.c_str(),
//     script.c_str(),
//     NULL
// };

// char *argv[] = interpreter ( cgi extension) + path 
//char *argv[] = { "/usr/bin/python3", script_path.c_str(), NULL };

// CGI ENV COMPONeNTS:


// std::string method = "REQUEST_METHOD=" + request.method;        // "REQUEST_METHOD=GET"
// std::string script = "SCRIPT_FILENAME=" + root + request.path;  // "SCRIPT_FILENAME=/var/www/cgi-bin/hello.py"
//                                                                      config root + URI path
// std::string query  = "QUERY_STRING=" + request.query_string;    // "QUERY_STRING=name=andi"
//                                                                       everything after ? in URI
// std::string content = "CONTENT_TYPE=" // request header
// std::string content_len = "CONTENT_LENGTH=" // from request header
// std::string server_name = "SERVER_NAME=" // from config
// std::string server_port = "SERVER_PORT="// from config

bool CGI::start()
{
    buildEnv();
    // pipe, fork, dup2, exec,
    // char *argv[] = interpreter ( cgi extension) + path 
    execve(_fullPath.c_str(), argv, _envp.data());
    return true;
}


void CGI::buildEnv()
{
    _env.clear();

    _env.push_back("REQUEST_METHOD=" + _method);
    _env.push_back("SCRIPT_FILENAME=" + _fullPath);
    _env.push_back("QUERY_STRING=" + _queryString);
    _env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    _env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    _env.push_back("SERVER_NAME=" + _serverName);
    _env.push_back("SERVER_PORT=" + _serverPort);

    if (_method == "POST" || _method == "PUT")
    {
        _env.push_back("CONTENT_TYPE=" + _contentType);
        _env.push_back("CONTENT_LENGTH=" + _contentLength);
    }

    _envp.clear();
    for (size_t i = 0; i < _env.size(); ++i)
        _envp.push_back(const_cast<char*>(_env[i].c_str()));
    _envp.push_back(NULL);// execve requires the array to be NULL-terminated
}
