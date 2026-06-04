/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SignalManager.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 13:27:30 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/04 15:43:24 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <limits>
// #include "Server.hpp"

class Server;

class SignalManager {

private:

    static int      _sig;
    
protected:

public:

                    SignalManager();
                    SignalManager( SignalManager const & src );
                    ~SignalManager();
    SignalManager & operator=( SignalManager const & other );

        
    void            setupSignals();
    void            setupSignalsFork();

};
