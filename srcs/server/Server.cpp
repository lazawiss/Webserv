/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 14:35:15 by lzannis           #+#    #+#             */
/*   Updated: 2026/05/27 19:07:37 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

Server::Server() : _sockfd(0){
    
    initHints();
    if (initRes() == false)
        throw std::logic_error("Error initRes");
    if (loopBindingSocket() == false)
        throw std::logic_error("Error loopBindingSocket");
    if (listeningSocket() == false)
        throw std::logic_error("Error listeningSocket");
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

/*
** ============================================================================
** Exec
** ============================================================================
*/

// Configure _hints : check if we need to take protocol from config file
struct addrinfo &    Server::initHints(){
    
    memset(&_hints, 0, sizeof _hints);
    _hints.ai_family = AF_UNSPEC;  // IPv4 or IPv6
    // _hints.ai_socktype = SOCK_STREAM;  // TCP
    _hints.ai_socktype = SOCK_DGRAM; // Datagram socket
   _hints.ai_flags = AI_PASSIVE; // For wildcard IP address
   _hints.ai_protocol = 0; //Any protocol
   _hints.ai_canonname = NULL;
   _hints.ai_addr = NULL;//struct 
   _hints.ai_next = NULL;//
   return _hints;
}

// getaddrinfo initialise struct _res out of struct _hints
bool    Server::initRes(){
    
    // Resolve "localhost" on port 8080 : first 2 args will come from config file 
    int status = getaddrinfo("localhost", "8080", &_hints, &_res);
    if (status != 0) {
        std::cout << "getaddrinfo: " << gai_strerror(status) << "ports" << std::endl;
        return false;
    }
    return true;
}

//show IPv4/IPv6 in a human-readable numeric form
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

// Create socket & bind it. If failed close socket 
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

// check if _p still exist & listen : put socket in passiv mode, ready for connection 
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

// main loop event : 
// accept : create dynamically a new connected socket for each new client, return a new int fd
// 
// getsockname : returns current socket address
void    Server::readingSocket(){
    
    for(;;){
        
        struct sockaddr_storage peer_addr;
        char                    buf[BUF_SIZE];

        
        socklen_t peer_addr_len = sizeof(peer_addr);

        int clientfd = accept(_sockfd, (struct sockaddr *) &peer_addr, &peer_addr_len );
        std::cout << "clientfd:" << clientfd << std::endl;

        // should fork() here : ONLY FORK() FOR CGI
        ssize_t n_read = read(clientfd, buf, BUF_SIZE); // read HTTP requests

        std::cout << "n_read:" << n_read << std::endl;
        if (n_read == -1){
            std::cerr << strerror(errno) << std::endl;
            close(clientfd);
            continue;
        }
        std::string request = std::string(buf, n_read);
        std::cout << "Request: " << request << std::endl;
        
        int s = getsockname(clientfd, (struct sockaddr *) &peer_addr, &peer_addr_len);
        if (clientfd == 0)
            std::cout << "Received " << static_cast<long>(n_read) << "bytes from " << peer_addr.ss_family << ":" << std::endl;
        else
            std::cout << "getnameinfo: " << gai_strerror(s) << std::endl;
            
        // Parse request
        if (request.find("GET / HTTP/1.1") != std::string::npos){ // same as EOF
            
            char    buffer[BUF_SIZE];

            // std::ifstream file("data/html/index.html".c_str());
            int indexfd = open("data/html/index.html", O_RDONLY);
            ssize_t n_read_index = read(indexfd, buffer, BUF_SIZE);
            std::cout << "n_read_index:" << n_read_index << std::endl;
            if (n_read_index == -1){
                std::cerr << strerror(errno) << std::endl;
                close(indexfd);
                continue;
            }

            // std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>Hello from C++!</body></html>";
            std::string response = std::string(buffer, n_read_index);

            if (send(clientfd, response.c_str(), response.size(), 0))
                std::cout << "Error sending response" << std::endl;
        } 

        close(clientfd);

    }
}

/*
** ============================================================================
** Signals
** ============================================================================
*/

// bool    Server::getInput( std::string & buf ){

//     if (!std::getline( std::cin, buf )){

//         if (std::cin.eof()){
            
//             std::cin.clear();
//             std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
//         }
//         exit(EXIT_SUCCESS);
//     }
//     if (buf.empty())
//         return false;
        
//     return true;
         
// }