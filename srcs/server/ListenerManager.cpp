/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenerManager.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 14:31:24 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/04 17:19:16 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ListenerManager.hpp"
#include "Server.hpp"

/*
** ============================================================================
** Orthodox canonical form
** ============================================================================
*/

ListenerManager::ListenerManager() : _res(NULL), _p(NULL), _sockfd(0),
    _node("localhost"), _service("8080") {}

ListenerManager::ListenerManager( ListenerManager const & src ) :
    _res(NULL), _p(NULL),_sockfd(src._sockfd),
    _node(src._node), _service(src._service) {}

ListenerManager::~ListenerManager()
{

    if (_res)
        freeaddrinfo(_res);
    if (_sockfd > 0)
        close(_sockfd);
}

ListenerManager &   ListenerManager::operator=( ListenerManager const & other ){
    
    if (this != &other)
    {
        this->_sockfd = other._sockfd;
        this->_node = other._node;
        this->_service = other._service;
    }
    return *this;
}

/*
** ============================================================================
** Getters & Setters
** ============================================================================
*/

// ── sockfd ──────────────────────────────────────────────────────────────────
int ListenerManager::getSockfd() const {

    return _sockfd;
}

// ── node ────────────────────────────────────────────────────────────────────
std::string ListenerManager::getNode() const {

    return _node;
}

// ── services ────────────────────────────────────────────────────────────────
std::string ListenerManager:: getService() const {

    return _service;
}

/*
** ============================================================================
** Member methods
** ============================================================================
*/

/**
** @brief Initializes the _hints struct.
**
** Sets the address family to AF_UNSPEC (accepts both IPv4 and IPv6),
** the socket type to SOCK_STREAM (TCP), and AI_PASSIVE so the returned
** address can be used with bind() to accept incoming connections on any
** local interface.
**
** @return Reference to the initialized _hints struct.
**/
struct addrinfo &    ListenerManager::initHints(){
    
    memset(&_hints, 0, sizeof _hints);
    _hints.ai_family = AF_UNSPEC;       // IPv4 or IPv6
    _hints.ai_socktype = SOCK_STREAM;   // TCP
    _hints.ai_flags = AI_PASSIVE;       // For wildcard IP address
    _hints.ai_protocol = 0;             // Any protocol
    _hints.ai_canonname = NULL;
    _hints.ai_addr = NULL;
    _hints.ai_next = NULL;
    return _hints;
}

/**
** @brief Resolves the host/port pair into a list of usable socket addresses.
**
** Calls getaddrinfo() with _node and _service (set from the config file)
** and _hints as criteria. On success, _res points to a linked list of
** struct addrinfo results that loopBindingSocket() will iterate over.
**
** @return true on success, false if getaddrinfo() fails.
**/
bool    ListenerManager::initRes(){
    
    int status = getaddrinfo(_node.c_str(), _service.c_str(), &_hints, &_res);
    if (status != 0)
    {
        LOG_ERROR(std::string("getaddrinfo() failed: ") + gai_strerror(status));
        return false;
    }
    return true;
}

/**
** @brief Iterates over the address list from getaddrinfo() and binds the
** first address that succeeds.
**
** Creates a socket for each entry in the _res linked list and attempts
** to bind it. Closes the socket and moves to the next entry on failure.
** Leaves _p pointing to the address that succeeded, so listeningSocket()
** can verify it.
**
** @return true if a socket was created and bound, false if all entries failed.
**/
bool   ListenerManager::loopBindingSocket(){
    
    for (_p = _res; _p != NULL; _p = _p->ai_next)
    {
        _sockfd = socket(_p->ai_family, _p->ai_socktype, _p->ai_protocol);
        if (_sockfd == -1)
        {
            LOG_ERROR("socket() failed, trying next adress");
            continue;
        }
        if (bind(_sockfd, _p->ai_addr, _p->ai_addrlen) == 0)
        {
            LOG_SYSTEM("Socket successfully bound to port " + _service);
            return true;
        }
        close(_sockfd);
    }
    
    return false;
}

/**
** @brief Puts the bound socket into passive listening mode.
**
** Verifies that _p is not NULL (meaning bind succeeded), then calls
** listen() with LISTEN_BACKLOG as the connection queue size, making
** the socket ready to accept incoming connections.
**
** @return true on success, false if _p is NULL or listen() fails.
**/
bool   ListenerManager::listeningSocket(){
    
    if (_p == NULL)
    {
        LOG_ERROR("Could not bind — no address succeeded");
        return false;
    }
    
    if (listen(_sockfd,LISTEN_BACKLOG) == -1)
    {
        LOG_ERROR("listen() failed - " + strerror(errno));
        return false;
    }

    LOG_SYSTEM("Socket is now listening for incoming connections");
    LOG_INFO("Server is up and running at http://" + _node + ":" + _service);
    
    return true;
}

/*
** ============================================================================
** Debug
** ============================================================================
*/

//show IPv4/IPv6 in a human-readable numeric form
// void    ListenerManager::findAddress(){

//     char            ipstr[INET6_ADDRSTRLEN];

//      // Iterate through the linked list of results
//     std::cout << "IP addresses for localhost:\n" << std::endl;
//     for (_p = _res; _p != NULL; _p = _p->ai_next) {
//         void *addr;
//         std::string ipver;

//         // Get the pointer to the address based on family
//         if (_p->ai_family == AF_INET) {  // IPv4
//             struct sockaddr_in *ipv4 = (struct sockaddr_in *)_p->ai_addr;
//             addr = &(ipv4->sin_addr);
//             ipver = "IPv4";
//         } else {  // IPv6
//             struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)_p->ai_addr;
//             addr = &(ipv6->sin6_addr);
//             ipver = "IPv6";
//         }
    
//         // Convert binary IP to human-readable string
//         inet_ntop(_p->ai_family, addr, ipstr, sizeof ipstr);
//         std::cout << ipver <<": " << ipstr << std::endl;
//     }
// }