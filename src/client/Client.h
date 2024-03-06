#ifndef CLIENT_H
#define CLIENT_H

#include <string>
#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#include "Handler.h"

#define SERVER "localhost"
#define PORT "1312"
#define MAXPACKETSIZE 256


#define MAXDATASIZE 256

// Class responsible for the networking part, constructing packets and handing them to a handler

class Handler;
class Client{
public:
    Client(std::string server, std::string port);
    void set_handler(Handler* h);

    void *get_in_addr(struct sockaddr *sa);

    bool connect_server();
    void receiver();
    void send_packet(int packet_length, unsigned char* data);


    Handler* handler;


    std::string server;
    std::string port;
    int sockfd;

};

#endif