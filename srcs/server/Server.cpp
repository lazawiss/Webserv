/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 14:35:15 by lzannis           #+#    #+#             */
/*   Updated: 2026/05/25 18:58:09 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server() : _sockfd(0){
    
    initHints();
    if (initRes() == false)
        throw std::exception();
    if (loopBindingSocket() == false)
        throw std::exception();
    if (listeningSocket() == false)
        throw std::exception();
    readingSocket();

}

Server::Server( Server const & src ){
    
    *this = src;
}

Server::~Server(){
    
    if (_res)
        freeaddrinfo(_res);
}
    
Server & Server::operator=( Server const & other ){

    if (this != &other)
        *this = other;
    return *this;
}

struct addrinfo &    Server::initHints(){
    
    // Configure hints
    memset(&_hints, 0, sizeof _hints);
    _hints.ai_family = AF_UNSPEC;  // IPv4 or IPv6
    _hints.ai_socktype = SOCK_STREAM;  // TCP
    // hints.ai_socktype = SOCK_DGRAM; // Datagram socket
   _hints.ai_flags = AI_PASSIVE; // For wildcard IP address
   _hints.ai_protocol = 0; //Any protocol
   _hints.ai_canonname = NULL;
   _hints.ai_addr = NULL;//struct 
   _hints.ai_next = NULL;//
   return _hints;
}

bool    Server::initRes(){
    
    // Resolve "localhost" on port 8080
    int status = getaddrinfo("localhost", "8080", &_hints, &_res);
    if (status != 0) {
        std::cout << "getaddrinfo: " << gai_strerror(status) << "ports" << std::endl;
        return false;
    }
    return true;
}

void    Server::findAddress(){

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

bool    Server::loopBindingSocket(){

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

bool    Server::listeningSocket(){
    
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

void    Server::readingSocket(){
    
    for(;;){
        
        struct sockaddr_storage peer_addr;
        char            buf[BUF_SIZE];
        
        socklen_t peer_addr_len = sizeof(peer_addr);

        int clientfd = accept(_sockfd, (struct sockaddr *) &peer_addr, &peer_addr_len );
        std::cout << "clientfd:" << clientfd << std::endl;
        ssize_t n_read = read(clientfd, buf, BUF_SIZE);

        std::cout << "n_read:" << n_read << std::endl;
        if (n_read == -1)
            continue;
        
        // char host[NI_MAXHOST], service[NI_MAXSERV];

        int s = getsockname(clientfd, (struct sockaddr *) &peer_addr, &peer_addr_len);
        if (clientfd == 0)
            std::cout << "Received " << static_cast<long>(n_read) << "bytes from " << peer_addr.ss_family << ":" << std::endl;
        else
            std::cout << "getnameinfo: " << gai_strerror(s) << std::endl;

        if (send(clientfd, buf, n_read, 0))
            std::cout << "Error sending response" << std::endl;

        close(clientfd);

    }
}