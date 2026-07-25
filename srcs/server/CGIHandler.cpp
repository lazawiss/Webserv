/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leazannis <leazannis@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/05 18:29:42 by andikim           #+#    #+#             */
/*   Updated: 2026/07/25 20:15:17 by leazannis        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGIHandler.hpp"
#include "RequestHandler.hpp"
#include "ListenerManager.hpp"
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
    _scriptFilename(req.getFilename()),
    _fullPath(req.getPath()),
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
        // child still alive at destruction = script hung -> kill + reap
        kill(_pid, SIGKILL);
        waitpid(_pid, NULL, 0);
    }
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

std::string CGI::findInterpreter() const
{
    size_t dot = _scriptFilename.rfind('.');
    if (dot == std::string::npos)
        return "";
    std::string ext = _scriptFilename.substr(dot);

    if (ext == ".py")
        return "/usr/bin/python3";
    if (ext == ".php")
        return "/usr/bin/php-cgi";
    return "";
}

void CGI::buildEnv()
{
    _env.clear();

    _env.push_back("REQUEST_METHOD=" + _method);
    _env.push_back("SCRIPT_FILENAME=" + _fullPath);
    _env.push_back("SCRIPT_NAME=" + _scriptFilename);
    _env.push_back("QUERY_STRING=" + _queryString);
    _env.push_back("GATEWAY_INTERFACE=CGI/1.1");
    _env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    _env.push_back("SERVER_NAME=" + _serverName);
    _env.push_back("SERVER_PORT=" + _serverPort);
    _env.push_back("REDIRECT_STATUS=200"); // php-cgi refuses to run without this (security guard against CVE-2012-1823)

    if (_method == "POST")
    {
        _env.push_back("CONTENT_TYPE=" + _contentType);
        _env.push_back("CONTENT_LENGTH=" + _contentLength);
    }

    // execve wants char*[]: point into _env's storage, NULL-terminated.
    // _env must stay alive until execve 
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
    std::string interpreter = findInterpreter();

    if (interpreter.empty() || access(_fullPath.c_str(), R_OK) != 0
        || access(interpreter.c_str(), X_OK) != 0)
    {
        std::cerr << "CGI: invalid script or interpreter: " << _fullPath << std::endl;
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
        // CHILD: becomes script, never returns - just exec
        dup2(_stdin_pipe[0], STDIN_FILENO);    // body being read-> script's stdin
        dup2(_stdout_pipe[1], STDOUT_FILENO);  // script's stdout -> pipe
        // close ALL pipe ends: the dup2 copies stay open. If we kept
        // _stdin_pipe[1] open here, the script would never see EOF on stdin.
        // Because kernel will look for ANY write end of this pipe - so even if parent
        // closed, need to close child or else child read() blocks forever
        close(_stdin_pipe[0]);
        close(_stdin_pipe[1]);
        close(_stdout_pipe[0]);
        close(_stdout_pipe[1]);

        char *argv[] = {
            const_cast<char*>(interpreter.c_str()),
            const_cast<char*>(_fullPath.c_str()),
            NULL
        };
        execve(interpreter.c_str(), argv, &_envp[0]);

        // only reached if execve failed
        std::cerr << "CGI: execve failed: " << strerror(errno) << std::endl;
        std::exit(1);
    }

    //  PARENT : keep only the ends parents use ----
    close(_stdin_pipe[0]); // child's stdin read end
    _stdin_pipe[0] = -1;
    close(_stdout_pipe[1]); // child's stdout write end
    _stdout_pipe[1] = -1;

    // every fd that goes through epoll must be non-blocking ; does this work @Lea?
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
bool CGI::onWritable()
{
    if (_bytesWritten >= _body.size())
        return true; // nothing left to send because all body complete

    ssize_t n = write(_stdin_pipe[1],
                      _body.c_str() + _bytesWritten,
                      _body.size() - _bytesWritten); // for remaining bytes
    if (n > 0)
        _bytesWritten += static_cast<size_t>(n);
    // n == -1 means the pipe is full for the moment; epoll will fire again.

    return _bytesWritten >= _body.size();
}


bool CGI::onReadable()
{
    char    buf[4096];
    ssize_t n = read(_stdout_pipe[0], buf, sizeof(buf));

    if (n > 0)
    {
        _output.append(buf, static_cast<size_t>(n));
        return false; // maybe more coming; epoll will tell us
    }
    return (n == 0); // 0 = EOF ; -1 = epoll woke me up but nothing to read
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
