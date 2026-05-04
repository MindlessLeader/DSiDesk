#ifndef TCP_SERVER
#define TCP_SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef struct
{
    int serverSocket;
    int clientSocket;
}TcpServer;

TcpServer* getTcpServer(uint16_t port);
void sendAll(TcpServer* tcpServer, void* buffer, int size);
void closeTcpServer(TcpServer* tcpServer);

#endif