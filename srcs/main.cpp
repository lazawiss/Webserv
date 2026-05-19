/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 17:44:47 by lzannis           #+#    #+#             */
/*   Updated: 2026/05/19 18:05:42 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors/Errors.hpp"
#include "parser/Parser.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <netdb.h>
#include <cstring>
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <cstdio>

#define BUF_SIZE 500

int parse_arguments(int argc);

int parse_arguments(int argc)
{
    if (argc != 2)
    {
        std::cerr << "Error: Invalid number of arguments!" << "\n";
        std::cerr << "Usage: ./webserv ./data/config/<config_file>" << std::endl;
        return ERROR_ARGS;
    }
    return SUCCESS;
}
int main(int argc, char **argv)
{
    int             result;
    struct addrinfo hints, *res, *p;
    char            ipstr[INET6_ADDRSTRLEN];

    // TO DO: try/catch 
    
    if ((result = parse_arguments(argc)) != SUCCESS)
        return result;

    if ((result = parse_file(argv[1])) != SUCCESS)
        return result;

    // Configure hints
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;  // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP
    // hints.ai_socktype = SOCK_DGRAM; // Datagram socket
    hints.ai_flags = AI_PASSIVE; // For wildcard IP address
    hints.ai_protocol = 0; //Any protocol
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    // Resolve "google.com" on port 80
    int status = getaddrinfo("localhost", "8080", &hints, &res);
    if (status != 0) {
        std::cout << "getaddrinfo: " << gai_strerror(status) << "ports" << std::endl;
        return 1;
    }

    // Iterate through the linked list of results
    std::cout << "IP addresses for localhost:\n" << std::endl;
    for (p = res; p != NULL; p = p->ai_next) {
        void *addr;
        std::string ipver;

        // Get the pointer to the address based on family
        if (p->ai_family == AF_INET) {  // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else {  // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // Convert binary IP to human-readable string
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        std::cout << ipver <<": " << ipstr << std::endl;
    }

    // Free the linked list
    freeaddrinfo(res);

    return 0;
}