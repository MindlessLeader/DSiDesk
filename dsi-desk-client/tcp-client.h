#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <nds.h>
#include <stdio.h>

int getTcpClient(const char* ip, int port);
void receiveAll(int tcpSocket, void* buffer, int size);

#endif