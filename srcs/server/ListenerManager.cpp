/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenerManager.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 14:31:24 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/04 15:00:58 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListenerManager.hpp"
#include "Server.hpp"

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

ListenerManager::  ListenerManager() : _sockfd(0){
    
}

ListenerManager::ListenerManager( ListenerManager const & src ) : _sockfd(src._sockfd){
    
}

ListenerManager::~ListenerManager(){

    std::cout << "Destructor ListenerManager" << std::endl;
    if (_res)
        freeaddrinfo(_res);
    if (_sockfd)
        close(_sockfd);
}

ListenerManager &   ListenerManager::operator=( ListenerManager const & other ){
    
    if (this != &other)
        this->_sockfd = other._sockfd;
    return *this;
}


int ListenerManager::getSockfd() const{
    
    return _sockfd;
}


/*
** ============================================================================
** Open Socket
** ============================================================================
*/

// Configure _hints : check if we need to take protocol from config file
struct addrinfo &    ListenerManager::initHints(){
    
    memset(&_hints, 0, sizeof _hints);
    _hints.ai_family = AF_UNSPEC;  // IPv4 or IPv6
    _hints.ai_socktype = SOCK_STREAM;  // TCP
    // _hints.ai_socktype = SOCK_DGRAM; // Datagram socket
   _hints.ai_flags = AI_PASSIVE; // For wildcard IP address
   _hints.ai_protocol = 0; //Any protocol
   _hints.ai_canonname = NULL;
   _hints.ai_addr = NULL;//struct 
   _hints.ai_next = NULL;//
   return _hints;
}

// getaddrinfo initialise struct _res out of struct _hints
bool    ListenerManager::initRes(){
    
    // Resolve "localhost" on port 8080 : first 2 args will come from config file 
    int status = getaddrinfo("localhost", "8080", &_hints, &_res);
    if (status != 0) {
        std::cout << "getaddrinfo: " << gai_strerror(status) << "ports" << std::endl;
        return false;
    }
    return true;
}

//show IPv4/IPv6 in a human-readable numeric form
void    ListenerManager::findAddress(){

    char            ipstr[INET6_ADDRSTRLEN];

     // Iterate through the linked list of results
    std::cout << "IP addresses for localhost:\n" << std::endl;
    for (_p = _res; _p != NULL; _p = _p->ai_next) {
        void *addr;
        std::string ipver;

        // Get the pointer to the address based on family
        if (_p->ai_family == AF_INET) {  // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)_p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else {  // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)_p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }
    
        // Convert binary IP to human-readable string
        inet_ntop(_p->ai_family, addr, ipstr, sizeof ipstr);
        std::cout << ipver <<": " << ipstr << std::endl;
    }
}

// Create socket & bind it. If failed close socket 
bool   ListenerManager::loopBindingSocket(){

     /* getaddrinfo() returns a list of address structures.
       Try each address until we successfully bind(2).
       If socket(2) (or bind(2)) fails, we (close the socket
       and) try the next address. */

    for (_p = _res; _p != NULL; _p = _p->ai_next) {
        
        _sockfd = socket(_p->ai_family, _p->ai_socktype, _p->ai_protocol);
        if (_sockfd == -1){
            std::cout << "Error socket" << std::endl;
            return false;
        }
        if (bind(_sockfd, _p->ai_addr, _p->ai_addrlen) == 0){
            std::cout << "Bind" << std::endl;
            return true;
        }
        close(_sockfd);
    }
    
    return false;
}

// check if _p still exist & listen : put socket in passiv mode, ready for connection 
bool   ListenerManager::listeningSocket(){
    
    // if (_res)
    //     freeaddrinfo(_res); //no longer needed

    if (_p == NULL){ //no address succeeded
        std::cout << "Could not bind" << std::endl;
        return false;
    }

    if (listen(_sockfd,LISTEN_BACKLOG) == -1){ //no address succeeded
        std::cout << "Could not listen" << std::endl;
        return false;
    }
    
    std::cout << "Listen" << std::endl;
    std::cout << "Serveur en écoute sur http://localhost:8080" << std::endl;
    
    return true;
}
