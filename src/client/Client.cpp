#include "Client.h"

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
#include <ncurses.h>

Client::Client(std::string s, std::string p){
    server = s;
    port = p;
}
void Client::set_handler(Handler* h){
    handler = h;
}

void Client::receive(){        //spaghetti
    

        ssize_t count = read(sockfd, buf + position, 2*MAXPACKETSIZE - position);

        if (count == -1){
            endwin();
        printf("\e[ q");
            exit(1);
        }
        else if (count == 0){
            endwin();
        printf("\e[ q");
            exit(0);
        }

        // received some data

        position = position + count;
        int i = 0;
        unsigned int packet_length = buf[0];

        while(position - i - 1 >= packet_length){// I have more data than the packet length, so full packet

            //give packet to handler    
            handler->handle_packet(packet_length, buf + i + 1);

            i = i + packet_length + 1;
             // If there is at least one more byte, get the next packet length, else stop the loop
            if(i < position)
                packet_length = buf[i];
            else                        //No more data
                break;
        }
        //if have read at least one packet move buffer to front
        
        if(i != 0){
            for(int j = 0; j < position - i; j++)
                buf[j] = buf[i + j];
                
            position = position - i;
        }
}
void* Client::get_in_addr(struct sockaddr *sa){

    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

bool Client::connect_server(){
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(server.c_str(), port.c_str(), &hints, &servinfo)) != 0) {
        //fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return false;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1){
            //perror("client: socket");
            continue;
        }

        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1){
            close(sockfd);
            //perror("client: connect");
            continue;
        }

        break;
    }

    if (p == NULL) {
        //fprintf(stderr, "client: failed to connect\n");
        return false;
    }

    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s, sizeof s);
    //printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    return true;
}

void Client::send_packet(int packet_length, unsigned char* data){ // spaghetti
    if(packet_length > 255)
        return;

    int total = 0;        // how many bytes we've sent
    int bytesleft = packet_length; // how many we have left to send
    int n;
    unsigned char p  = (unsigned char)packet_length;

    send(sockfd, &p, 1, 0);
    while(total < packet_length) {
        n = send(sockfd, data+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }
}