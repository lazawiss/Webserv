/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenerManager.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leazannis <leazannis@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 14:31:24 by lzannis           #+#    #+#             */
/*   Updated: 2026/07/25 18:42:04 by leazannis        ###   ########.fr       */
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

ListenerManager::ListenerManager( const std::string &host, const std::string &port ) :
    _res(NULL), _p(NULL), _sockfd(0), _node(host), _service(port) {}

ListenerManager::~ListenerManager()
{

    if (_res)
        freeaddrinfo(_res);
    if (_sockfd > 0)
        close(_sockfd);
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
        /* Allows immediate reuse of the port instead of waiting out TIME_WAIT */
        int yes = 1;
        if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
        {
            LOG_ERROR("setsockopt(SO_REUSEADDR) failed, trying next adress");
            close(_sockfd);
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
        LOG_ERROR("listen() failed - " + std::string(strerror(errno)));
        return false;
    }

    LOG_INFO("Server is up and running at http://" + _node + ":" + _service);
    LOG_SYSTEM("Server is now listening for incoming connections");

    return true;
}

void ListenerManager::releaseSockfd(){

    _sockfd = 0;
}
