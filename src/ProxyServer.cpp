#include "ProxyServer.h"

#include <cstring>
#include <unistd.h>
#include <iostream>
#include <fcntl.h>

ProxyServer::ProxyServer(int listenPort, const std::string& dbHost, int dbPort, const std::string& logFile){

    _serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (_serverSocket == 0){
        perror("Socket create failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0 ,sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(listenPort);

    if ( bind(_serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0 ){
        perror("Bind failed");
        close(_serverSocket);
        exit(EXIT_FAILURE);
    }

    if ( listen(_serverSocket, 10) < 0 ){
        perror("Error on listening");
        close(_serverSocket);
        exit(EXIT_FAILURE);
    }

    _pollFds.push_back({_serverSocket,POLLIN,0});

}

void ProxyServer::Start(){
    while(_isRun){
        ProcessPollEvents();
    }
}

void ProxyServer::Stop(){
    _isRun = false;
    close(_serverSocket);

}

void ProxyServer::AcceptNewConnection(){
    struct sockaddr_in clientAddress;
    socklen_t clientLen = sizeof(clientAddress);
    int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddress, &clientLen);
    
    if (clientSocket < 0){
        perror("Accept filed");
        return;
    }

    _pollFds.push_back({clientSocket,POLLIN | POLLOUT ,0});

}

void ProxyServer::HandleCommunication(int clientSocket, int dbSocket){
    char buffer[4096];

    while(_isRun){

        ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer));
        if (bytesRead <= 0){
            break;
        }

        std::string sqlQuery = PostgreSQLParser::parse(buffer,bytesRead);
        if (!sqlQuery.empty()){
            _logger.log(sqlQuery);
        }

        write(dbSocket, buffer, sizeof(buffer));

        bytesRead = read(dbSocket,buffer,sizeof(buffer));
        if (bytesRead <= 0){
            break;
        }

        write(clientSocket, buffer, bytesRead);

    }

    close(clientSocket);
    close(dbSocket);

}

void ProxyServer::ProcessPollEvents(){

    int res = poll(_pollFds.data(), _pollFds.size(), -1);

    if (res < 0){
        perror("Poll failed");
        return;
    }

    for (auto &pfd : _pollFds){
        if (pfd.revents & POLLIN){
            if (pfd.fd == _serverSocket){
                AcceptNewConnection();
            }
            else{
                int clientSocket = pfd.fd;
                int dbSock = socket(AF_INET, SOCK_STREAM, 0);
                if (dbSock < 0){
                    perror("Failde to create DB socket \n");
                    continue;
                }

                struct sockaddr_in dbAddr;
                memset(&dbAddr,0,sizeof(dbAddr));
                dbAddr.sin_family = AF_INET;
                dbAddr.sin_port = htons(_dbPort);

                if (inet_pton(AF_INET, _dbHost.c_str(), &dbAddr.sin_addr) <= 0){
                    perror("Invalid DB host address");
                    close(dbSock);
                    continue;
                }

                if (connect(dbSock, (struct sockaddr*)&dbAddr, sizeof(dbAddr)) < 0){
                    perror("BD connect failed");
                    close(dbSock);
                    continue;
                }

                HandleCommunication(clientSocket,dbSock);
            }
        }
    }
}
