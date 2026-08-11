/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leazannis <leazannis@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 14:35:15 by lzannis           #+#    #+#             */
/*   Updated: 2026/08/11 22:49:57 by leazannis        ###   ########.fr       */
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

    for (size_t i = 0; i < _listenermanagers.size(); i++)
        delete _listenermanagers[i];

    LOG_SEP();
    LOG_SYSTEM("Server STOP...");

}
    
Server & Server::operator=( Server const & other ){

    (void)other;
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
        const std::string &listen = servers[i].getListen();

        size_t nbr = listen.find(':');
        std::string host = listen.substr(0, nbr);
        std::string port = listen.substr(nbr + 1);

        ListenerManager *lm = new ListenerManager(host, port);
        lm->initHints();
        if (lm->initRes() == false)
        {
            delete lm;
            throw std::logic_error("Error initRes");
        }
        if (lm->loopBindingSocket() == false)
        {
            delete lm;
            throw std::logic_error("Error loopBindingSocket");
        }
        if (lm->listeningSocket() == false)
        {
            delete lm;
            throw std::logic_error("Error listeningSocket");
        }

        _listenermanagers.push_back(lm);
    }

}
    
void    Server::run()
{
    _signalManager.setupSignals();

    if (_epollloop.readingSocket( _listenermanagers, _config ) == false){
        
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