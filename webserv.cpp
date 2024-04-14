/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iecharak <iecharak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/24 20:27:19 by iecharak          #+#    #+#             */
/*   Updated: 2024/04/08 15:18:07 by iecharak         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "config.hpp"

bool    isDupPortHost(std::deque<server> &servers, server &serv)
{
    for (std::deque<server>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        if (it->host == serv.host && it->port == serv.port && it->serverSocket != -1)
        return true;
    }
    return 0;
}

void    serversSetup(std::deque<server> &servers)
{
    //page 354
    
    for(std::deque<server>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        if (isDupPortHost(servers, *it))
            continue;
        struct addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = AI_PASSIVE;

        struct addrinfo *bind_address;
        if (getaddrinfo(it->host.c_str(), it->portKey.c_str(), &hints, &bind_address))
            throw MyRuntimeError("Error: getaddrinfo() failed. Please make sure the port " + it->portKey + " and the network interface " + it->host + " are available");;
        int socket_listen;
        socket_listen = socket(bind_address->ai_family, bind_address->ai_socktype, bind_address->ai_protocol);
        if (socket_listen < 0)
            throw MyRuntimeError("Error: socket() failed.");
        int yes = 1;
        if (setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, (void*)&yes, sizeof(yes)) < 0)
            throw MyRuntimeError("Error: setsockopt() failed.");

        fcntl(socket_listen, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
        
        if (bind(socket_listen, bind_address->ai_addr, bind_address->ai_addrlen))
            throw MyRuntimeError("Error: bind() failed. Please make sure the port " + it->portKey + " and the network interface " + it->host + " are available");
        
        freeaddrinfo(bind_address);
        if (listen(socket_listen, SOMAXCONN) < 0) // sysctl -a | grep somaxconn
            throw MyRuntimeError("Error: listen() failed.");
        
        it->serverSocket = socket_listen;
        std::cout << "Listening on " << it->host << ":" << it->port << std::endl;
    }
}

int updateMaxSocket(std::deque<server> &servers)
{
    int max = 0;
    for(std::deque<server>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        if(it->serverSocket == -1)
            continue;
        if (max < it->maxSocket)
            max = it->maxSocket;
    }
    return max;
}

void    multiplexing(std::deque<server> &servers)
{
    struct timeval timeout;
    fd_set readSet;
    fd_set writeSet;
    memset(&readSet, 0, sizeof(readSet));
    memset(&writeSet, 0, sizeof(writeSet));
    for (std::deque<server>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        if(it->serverSocket == -1)
            continue;
        FD_SET(it->serverSocket, &readSet);
        FD_SET(it->serverSocket, &writeSet);
    }
    int maxSocket = servers.back().serverSocket;
    
    fd_set readSetTmp;
    fd_set writeSetTmp;
    signal(SIGPIPE, SIG_IGN);
    while (1)
    {
        readSetTmp = readSet;
        writeSetTmp = writeSet;
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        if (select(maxSocket + 1, &readSetTmp, &writeSetTmp, 0, &timeout) == -1)
            std::cerr << "Error in select\n";
        for (std::deque<server>::iterator it = servers.begin(); it != servers.end(); ++it)
        {
            if(it->serverSocket == -1)
                continue;
            if (FD_ISSET(it->serverSocket, &readSetTmp))
            {
                sockaddr_storage clientAddr;
                socklen_t clientAddrSize = sizeof(clientAddr);
                int clientSocket = accept(it->serverSocket, (sockaddr*)&clientAddr, &clientAddrSize); 
                if (clientSocket > 0)
                {
                    fcntl(clientSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
                    it->addClient(clientSocket);
                    FD_SET(clientSocket, &readSet);
                    FD_SET(clientSocket, &writeSet);
                }
            }
            it->handleClients(servers, &readSetTmp, &writeSetTmp);
            it->dropClients(&readSet, &writeSet);
            maxSocket = updateMaxSocket(servers);
        }
    }
}

int main(int ac, char** arv)
{
    try
    {
        if (ac > 2)
            throw MyRuntimeError("Please use ----->  ./webserv [configuration file]");
        
        std::deque<server> servers;
        if ( ac == 2)
            parseConfigFile(servers, arv[1]);
        else
            parseConfigFile(servers, "conf.conf");
        printServers(servers);
        std::srand(std::time(0));
        std::vector<int> serverSockets;
        serversSetup(servers);
        multiplexing(servers);
    }
    catch(std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch(...)
    {
        std::cerr << "Erorr" << std::endl;
    }
}