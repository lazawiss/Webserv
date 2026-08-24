/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/05 18:29:42 by andikim           #+#    #+#             */
/*   Updated: 2026/08/18 16:19:12 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHandler.hpp"
#include "RequestHandler.hpp"
#include "ListenerManager.hpp"
#include "Server.hpp"
#include <fcntl.h>
#include <sys/wait.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iostream>

/*
** ============================================================================
** Constructor / Destructor
** ============================================================================
*/

CGI::CGI(RequestHandler const &req, ListenerManager const &listen, int client_fd) :
    _pid(-1),
    _client_fd(client_fd),
    _bytesWritten(0),
    _startTime(0),
    _scriptFilename(req.getFilename()),
    _fullPath(req.getPath()),
    _pathInfo(req.getPathInfo()),
    _scriptName(req.getScriptName()),
    _cgiInterpreter(req.getInterpreter()),
    _queryString(req.getQueryString()),
    _method(req.getMethod()),
    _body(req.getBody()),
    _contentType(req.getContentType()),
    _contentLength(req.getContentLength()),
    _serverName(listen.getNode()),
    _serverPort(listen.getService())
{
    _stdin_pipe[0] = -1;
    _stdin_pipe[1] = -1;
    _stdout_pipe[0] = -1;
    _stdout_pipe[1] = -1;
}

CGI::~CGI()
{
    if (_stdin_pipe[1] != -1)
        close(_stdin_pipe[1]);
    if (_stdout_pipe[0] != -1)
        close(_stdout_pipe[0]);
    if (_pid > 0)
    {
        kill(_pid, SIGKILL);
        waitpid(_pid, NULL, 0);
    }
}

bool CGI::hasTimedOut(time_t now) const
{
    if (_startTime == 0)
        return false;
    return (now - _startTime) >= CGI_TIMEOUT;
}

/*
** ============================================================================
** Getters
** ============================================================================
*/

int CGI::getClientFd() const { return _client_fd; }
int CGI::getStdinFd() const  { return _stdin_pipe[1]; }   // OUR end (write) from PARENT
int CGI::getStdoutFd() const { return _stdout_pipe[0]; }  // OUR end (read) from PARENT

/*
** ============================================================================
** Environment
** ============================================================================
*/

// std::string CGI::findInterpreter() const
// {
//     size_t dot = _scriptFilename.rfind('.');
//     if (dot == std::string::npos)
//         return "";
//     std::string ext = _scriptFilename.substr(dot);

//     if (ext == ".py")
//         return "/usr/bin/python3";
//     if (ext == ".php")
//         return "/usr/bin/php-cgi";
//     return "";
// }

void CGI::buildEnv()
{
    _env.clear();

    _env.push_back("REQUEST_METHOD=" + _method);
    _env.push_back("SCRIPT_FILENAME=" + _fullPath);
    _env.push_back("SCRIPT_NAME="
        + (_scriptName.empty() ? _scriptFilename : _scriptName));
    if (_pathInfo.empty() == false)
        _env.push_back("PATH_INFO=" + _pathInfo);
    _env.push_back("QUERY_STRING=" + _queryString);
    _env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    _env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    _env.push_back("SERVER_NAME=" + _serverName);
    _env.push_back("SERVER_PORT=" + _serverPort);
    _env.push_back("REDIRECT_STATUS=200"); 

    if (_method == "POST") 
    {
        _env.push_back("CONTENT_TYPE=" + _contentType);
        _env.push_back("CONTENT_LENGTH=" + _contentLength);
    }

    _envp.clear();
    for (size_t i = 0; i < _env.size(); ++i)
        _envp.push_back(const_cast<char*>(_env[i].c_str()));
    _envp.push_back(NULL);
}

/*
** ============================================================================
** Launch : pipe + fork + execve
** ============================================================================
*/

bool CGI::start()
{
    signal(SIGPIPE, SIG_IGN);

    if (_cgiInterpreter.empty() || access(_fullPath.c_str(), R_OK) != 0
        || access(_cgiInterpreter.c_str(), X_OK) != 0)
    {
        LOG_ERROR("CGI: invalid script or interpreter: " + _fullPath);
        return false;
    }

    if (pipe(_stdin_pipe) == -1) // parents writes -> child reads
        return false;
    if (pipe(_stdout_pipe) == -1) // child writes -> parent read
    {
        close(_stdin_pipe[1]); // close parent write
        close(_stdin_pipe[0]); // close child read
        _stdin_pipe[0] = -1;
        _stdin_pipe[1] = -1;
        return false;
    }

    buildEnv();

    _pid = fork();
    if (_pid == -1)
    {
        closeStdin();
        closeStdout();
        return false;
    }

    if (_pid == 0)
    {
        dup2(_stdin_pipe[0], STDIN_FILENO);    // body being read-> script's stdin
        dup2(_stdout_pipe[1], STDOUT_FILENO);  // script's stdout -> pipe
        close(_stdin_pipe[0]);
        close(_stdin_pipe[1]);
        close(_stdout_pipe[0]);
        close(_stdout_pipe[1]);

        char *argv[] = {
            const_cast<char*>(_cgiInterpreter.c_str()),
            const_cast<char*>(_fullPath.c_str()),
            NULL
        };
        execve(_cgiInterpreter.c_str(), argv, &_envp[0]);

        // only reached if execve failed
        LOG_ERROR("CGI: execve failed: " + std::string(strerror(errno)));
        std::exit(1);
    }

    _startTime = time(NULL); 
    close(_stdin_pipe[0]); // child's stdin read end
    _stdin_pipe[0] = -1;
    close(_stdout_pipe[1]); // child's stdout write end
    _stdout_pipe[1] = -1;
    fcntl(_stdin_pipe[1], F_SETFL, O_NONBLOCK);
    fcntl(_stdout_pipe[0], F_SETFL, O_NONBLOCK);

    return true;
}

/*
** ============================================================================
** epoll callbacks
** ============================================================================
*/

// EPOLLOUT on _stdin_pipe[1]: the pipe has room, write more of the body.
// Returns true when the whole body has been handed to the script —
// EpollLoop then deregisters the fd and calls closeStdin() (= EOF).
Result CGI::onWritable()
{
    if (_bytesWritten >= _body.size())
        return SUCCESS; // nothing left to send because all body complete

    ssize_t n = write(_stdin_pipe[1],
                      _body.c_str() + _bytesWritten,
                      _body.size() - _bytesWritten); // for remaining bytes

    if (n == -1)
        return ERR;

    if (n > 0)
    {
        _bytesWritten += static_cast<size_t>(n);
        if (_bytesWritten >= _body.size())
            return SUCCESS;
    }

    return HOLD;
}

Result CGI::onReadable()
{
    char    buf[4096];
    ssize_t n = read(_stdout_pipe[0], buf, sizeof(buf));

    if (n == -1)
        return ERR; 

    if (n == 0)
        return SUCCESS;

    _output.append(buf, static_cast<size_t>(n));
    return HOLD;
}

void CGI::closeStdin()
{
    if (_stdin_pipe[1] != -1)
    {
        close(_stdin_pipe[1]);
        _stdin_pipe[1] = -1;
    }
}

void CGI::closeStdout()
{
    if (_stdout_pipe[0] != -1)
    {
        close(_stdout_pipe[0]);
        _stdout_pipe[0] = -1;
    }
    // EOF received -> child has exited (or is exiting): reap it, no zombie.
    if (_pid > 0)
    {
        waitpid(_pid, NULL, 0);
        _pid = -1;
    }
}

/*
** ============================================================================
** CGI output -> HTTP response
** ============================================================================
*/

// CGI-> HTTP, which means: headers, blank line, body.
// Content-Type: text/html\r\n\r\n<html>...
// Split it, pick up an optional "Status:" header, and wrap the
// rest into a real HTTP/1.1 response with a correct Content-Length (maybe repeat of Lea?)
std::string CGI::buildResponse()
{
    std::string headers;
    std::string body;

    size_t sep = _output.find("\r\n\r\n");
    size_t sepLen = 4;
    if (sep == std::string::npos)
    {
        sep = _output.find("\n\n"); // some scripts use bare \n
        sepLen = 2;
    }

    if (sep == std::string::npos)
    {
        // no header block at all -> invalid CGI output
        return "HTTP/1.1 502 Bad Gateway\r\nContent-Length: 0\r\n\r\n";
    }

    headers = _output.substr(0, sep);
    body = _output.substr(sep + sepLen);

    // "Status: 404 Not Found" lets the script pick its own status code
    std::string statusLine = "200 OK";
    size_t st = headers.find("Status:");
    if (st != std::string::npos)
    {
        size_t end = headers.find('\n', st);
        statusLine = headers.substr(st + 7, end - (st + 7));
        while (!statusLine.empty() && (statusLine[0] == ' '))
            statusLine.erase(0, 1); // for " 200 OK"
        while (!statusLine.empty() && (statusLine[statusLine.size() - 1] == '\r'))
            statusLine.erase(statusLine.size() - 1);
        size_t lineStart = headers.rfind('\n', st);
        if (lineStart == std::string::npos)
            lineStart = 0;
        else
            lineStart = lineStart + 1;
        size_t eraseEnd;
        if (end == std::string::npos)
            eraseEnd = headers.size();
        else
            eraseEnd = end + 1;
        headers.erase(lineStart, eraseEnd - lineStart);
    }

    std::stringstream ss;
    ss << body.size();

    std::string response = "HTTP/1.1 " + statusLine + "\r\n";
    if (!headers.empty())
    {
        response += headers;
        if (headers[headers.size() - 1] != '\n')
            response += "\r\n";
    }
    response += "Content-Length: " + ss.str() + "\r\n";
    response += "Connection: close\r\n";
    response += "\r\n";
    response += body;

    return response;
}
