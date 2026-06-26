/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 14:35:15 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/09 14:24:34 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "SignalManager.hpp"
#include "ListenerManager.hpp"
#include "EpollLoop.hpp"
#include "../parser/config/GlobalConfig.hpp"

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

Server::Server( const GlobalConfig &config ) : _config(config)
{
    LOG_SEP();
    LOG_SYSTEM("Server STARTING...");
}

Server::Server( Server const & src ){
    
    *this = src;
}

Server::~Server(){

    LOG_SEP();
    LOG_SYSTEM("Server STOP...");
    
}
    
Server & Server::operator=( Server const & other ){

    if (this != &other)
        *this = other;

    return *this;

}

/*
** ============================================================================
** Start & Run Server
** ============================================================================
*/

void    Server::start()
{
    const std::vector<ServerConfig> &servers = _config.getServers();

    for (size_t i = 0; i < servers.size(); i++)
    {
        const std::vector<std::string> &listens = servers[i].getListen();

        for (size_t j = 0; j < listens.size(); j++)
        {
            const std::string &listen = listens[j];

            size_t point = listen.find(':');
            if (point == std::string::npos)
                throw std::runtime_error("Invalid listen directive: " + listen);

            std::string host = listen.substr(0, point);
            std::string port = listen.substr(point + 1);

            ListenerManager listenermanagers(host, port);
            listenermanagers.initHints();
            if (listenermanagers.initRes() == false)
                throw std::logic_error("Error initRes");
            if (listenermanagers.loopBindingSocket() == false)
                throw std::logic_error("Error loopBindingSocket");
            if (listenermanagers.listeningSocket() == false)
                throw std::logic_error("Error listeningSocket");

            _listenermanagers.push_back(listenermanagers);
        }
    }

}
    
void    Server::run()
{
    _signalManager.setupSignals();

    if (_epollloop.readingSocket( _listenermanagers ) == false){
        
        throw std::logic_error("Error readingSocket");
    }
}

volatile sig_atomic_t Server::_quit = 0;

/*
** ============================================================================
** Member methods
** ============================================================================
*/

std::string logTimestamp()
{
    struct timeval tv;
    char buf[16];

    gettimeofday(&tv, NULL);
    struct tm *tm_info = localtime(&tv.tv_sec);
    strftime(buf, sizeof(buf), "%H:%M:%S", tm_info);

    std::ostringstream oss;
    oss << buf << "." << std::setw(3) << std::setfill('0') << (tv.tv_usec / 1000);

    return oss.str();
}