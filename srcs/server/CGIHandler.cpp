/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/28 19:39:04 by ankim             #+#    #+#             */
/*   Updated: 2026/07/02 12:18:08 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "HTTPParser.hpp"
# include "Server.hpp"
# include "EpollLoop.hpp"

/*ORTHODOX CANONICAL FORM*/

CGI::CGI(const HTTPParser& ref, const ListenerManager& ref2) : _info(ref), _output(NULL) {};

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


