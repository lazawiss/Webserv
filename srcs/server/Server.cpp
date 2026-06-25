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

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

Server::Server(){
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

void    Server::start(){
    
    _listenermanager.initHints();
    if (_listenermanager.initRes() == false)
        throw std::logic_error("Error initRes");
    if (_listenermanager.loopBindingSocket() == false)
        throw std::logic_error("Error loopBindingSocket");
    if (_listenermanager.listeningSocket() == false)
        throw std::logic_error("Error listeningSocket");

}
    
void    Server::run(){
         
    _signalManager.setupSignals();

    if (_epollloop.readingSocket( _listenermanager ) == false){
        
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