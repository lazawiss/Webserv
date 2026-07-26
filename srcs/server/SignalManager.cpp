/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SignalManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leazannis <leazannis@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 13:27:06 by lzannis           #+#    #+#             */
/*   Updated: 2026/07/25 20:11:39 by leazannis        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "SignalManager.hpp"
#include "Server.hpp"

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

SignalManager::SignalManager(){
    
}

SignalManager::SignalManager( SignalManager const & src ){
    
    *this = src;
}

SignalManager::~SignalManager(){
    
}

SignalManager & SignalManager::operator=( SignalManager const & other ){
    
    if (this != &other )
        *this = other;

    return *this;
}
        
/*
** ============================================================================
** Signals
** ============================================================================
*/


static void  sigintHandler(int _sig){

    if (_sig == SIGTERM || _sig == SIGINT){
        Server::_quit = 1;
        write(STDERR_FILENO, "[System]   Server stopping\n", 27);
    }
}

void    SignalManager::setupSignals(){
    
    signal(SIGINT, sigintHandler);
    signal(SIGTERM, sigintHandler);
    signal(SIGHUP, sigintHandler);
    signal(SIGQUIT, SIG_IGN);
}

static void sigHandlerFork(int _sig){

    write(STDERR_FILENO, "Signal received\n", 16);
    
    if (_sig == SIGTERM || _sig == SIGINT){

        Server::_quit = 0;
    }
    
    // exit(_sig);
}

void    SignalManager::setupSignalsFork(){
    
    signal(SIGINT, sigHandlerFork);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGQUIT, SIG_DFL);
}

int SignalManager::_sig = 0;
