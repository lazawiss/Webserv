/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>

class RequestHandler;
class ListenerManager;

// One CGI object = one script execution for one client.
// Lifecycle (spread over several epoll_wait wakeups):
// start()       : pipes + fork + execve (called from do_use_fd)
// onWritable()  : feed request body to child's stdin(EPOLLOUT on stdin pipe)
// onReadable()  : collect script output from child's stdout (EPOLLIN on stdout pipe)
// buildResponse(): CGI output -> full HTTP response, sent to _client_fd
// EpollLoop owns the object and is the only one who closes fds / deletes it.
class CGI
{
public:
    CGI(RequestHandler const &req, ListenerManager const &listen, int client_fd);
    ~CGI();

    bool        start();
    // both return true when their side is finished:
    // onWritable -> whole body written / onReadable -> EOF (script done)
    bool        onWritable();
    bool        onReadable();

    void        closeStdin();   // sends EOF to the script
    void        closeStdout();  // called after EOF; also reaps the child

    std::string buildResponse();

    int         getClientFd() const;
    int         getStdinFd() const;
    int         getStdoutFd() const;

private:
    // a CGI owns a pid and two pipes: copying would double-close them so moved into private
    CGI(CGI const &src);
    CGI &operator=(CGI const &other);

    void        buildEnv();
    std::string findInterpreter() const;

    pid_t                       _pid;
    int                         _client_fd;
    // Need both because we need to have two way communication so: 
    int                         _stdin_pipe[2];  // parent writes body ->[1]  [0]-> child stdin
    int                         _stdout_pipe[2]; // child stdout ->[1]  [0]-> parent reads

    size_t                      _bytesWritten;   // how much of _body was sent so far
    std::string                 _output;         // raw script output, accumulated

    std::string                 _scriptFilename;
    std::string                 _fullPath;
    std::string                 _queryString;
    std::string                 _method;
    std::string                 _body;
    std::string                 _contentType;
    std::string                 _contentLength;
    std::string                 _serverName;
    std::string                 _serverPort;

    std::vector<std::string>    _env;   // stores real vector of env variables, but execve cannot use std::string
    std::vector<char*>          _envp;  // NULL-terminated view of _env for execve
};
