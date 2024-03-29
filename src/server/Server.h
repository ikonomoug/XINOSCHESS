

#ifndef SERVER_H
#define SERVER_H

#include "UserPool.h"
#define PORT "1312"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

#define MAXEVENTS 64 //max epoll events

#define MAXPACKETSIZE 256 // max packet size including packet length byte

class Handler;

class Server {
private:
	int server_socket; // listen on server_socket
	int efd; // epoll file descriptor

	int s; //error int

    Handler* handler;

    struct epoll_event *events;

	struct socket_data_struct{
        int fd;
        User* user = nullptr;
		int position = 0;
		unsigned char buffer[2*MAXPACKETSIZE];
	};

    int make_socket_non_blocking (int sfd);
    int bind_server();
    void main_loop();

    void handle_epoll_events(int n);
    void handle_new_connections();
    void handle_data_from_socket(const struct epoll_event& event);
    void handle_socket_error(const struct epoll_event& event);

    bool is_listener_event(const struct epoll_event& event);
    bool is_error_event(const struct epoll_event& event);

    void disconnect(socket_data_struct* socket_data);

public:
    void set_handler(Handler* h);

	void start_server();

    void send_packet(int id, int packet_length, unsigned char* data);

	

};

#endif