#include "tcp-client.h"

int getTcpClient(const char* ip, int port)
{
    int tcpSocket;
    tcpSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    iprintf("Connecting to: %s\n", ip);
    connect(tcpSocket, (struct sockaddr *)&server, sizeof(server));

    return tcpSocket;
}

void receiveAll(int tcpSocket, void* buffer, int size)
{
    int bytesLeft = size;
    while(bytesLeft > 0)
        bytesLeft = bytesLeft - recv(tcpSocket, buffer + (size - bytesLeft), bytesLeft, 0);
}