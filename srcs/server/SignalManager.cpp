/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SignalManager.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 13:27:06 by lzannis           #+#    #+#             */
/*   Updated: 2026/08/25 21:30:26 by lzannis          ###   ########.fr       */
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

    (void)other;
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
        ssize_t bytes = write(STDERR_FILENO, "[System]   Server stopping\n", 27);
        if (bytes == -1){
            LOG_ERROR("Write failed.");
            return;
        }
        if(bytes == 0){
            LOG_ERROR("No bytes to write.");
            return;
        }
    }
}

void    SignalManager::setupSignals(){
    
    signal(SIGINT, sigintHandler);
    signal(SIGTERM, sigintHandler);
    signal(SIGHUP, sigintHandler);
    signal(SIGQUIT, SIG_IGN);
}

static void sigHandlerFork(int _sig){

    ssize_t bytes = write(STDERR_FILENO, "Signal received\n", 16);
    if (bytes == -1){
        LOG_ERROR("Write failed.");
        return;
    }
    if(bytes == 0){
        LOG_ERROR("No bytes to write.");
        return;
    }
    
    if (_sig == SIGTERM || _sig == SIGINT){

        Server::_quit = 0;
    }
}

void    SignalManager::setupSignalsFork(){
    
    signal(SIGINT, sigHandlerFork);
    signal(SIGPIPE, SIG_IGN);
    signal(SIGQUIT, SIG_DFL);
}

int SignalManager::_sig = 0;
