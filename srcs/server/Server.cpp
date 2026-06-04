/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 14:35:15 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/04 15:34:29 by lzannis          ###   ########.fr       */
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

}

Server::Server( Server const & src ){
    
    *this = src;
}

Server::~Server(){

    std::cout << "Destructor Server" << std::endl;
    
}
    
Server & Server::operator=( Server const & other ){

    if (this != &other)
        *this = other;

    return *this;

}

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