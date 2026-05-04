#include "tcp-server.h"

TcpServer* getTcpServer(uint16_t port)
{
    TcpServer* tcpServer = malloc(sizeof(TcpServer));
    tcpServer->serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    address.sin_addr.s_addr = INADDR_ANY;

    bind(tcpServer->serverSocket, (struct sockaddr*)&address, sizeof(address));

    printf("Listening...\n");
    listen(tcpServer->serverSocket, 3);
    printf("Listened\n");

    socklen_t addrlen = sizeof(address);
    printf("Accepting...\n");
    tcpServer->clientSocket = accept(tcpServer->serverSocket, (struct sockaddr*)&address, &addrlen);
    printf("Accepted\n");

    return tcpServer;
}

void sendAll(TcpServer* tcpServer, void* buffer, int size)
{
    int bytesLeft = size;
    while(bytesLeft > 0)
        bytesLeft = bytesLeft - send(tcpServer->clientSocket, buffer + size - bytesLeft, bytesLeft, 0);
}

void closeTcpServer(TcpServer* tcpServer)
{
    close(tcpServer->clientSocket);
    close(tcpServer->serverSocket);
    free(tcpServer);
}