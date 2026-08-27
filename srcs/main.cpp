/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 17:44:47 by lzannis           #+#    #+#             */
/*   Updated: 2026/08/26 16:56:10 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser/Parser.hpp"
#include "server/Server.hpp"

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

int parse_arguments(int argc);

int main(int argc, char **argv)
{
    int             result;
    if ((result = parse_arguments(argc)) != 1)
        return result;
    
    try
    {
        GlobalConfig config;
        
        config = parse_file(argv[1]);
        Server server(config);
        
        server.start();
        server.run();
    }
    catch(const std::exception &e)
    {
        LOG_ERROR(e.what());
        return -1;
    }
  
    return 0;
}

int parse_arguments(int argc)
{
    if (argc != 2)
    {
        LOG_ERROR("Invalid number of arguments!");
        LOG_ERROR("Usage: ./webserv ./data/config/<config_file>");

        return -1;
    }
    return 1;
}