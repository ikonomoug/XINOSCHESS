

#ifndef SERVER_H
#define SERVER_H

#include "UserPool.h"
#define PORT "1312"  // the port users will be connecting to

#define BACKLOG 10   // how many pending connections queue will hold

#define MAXEVENTS 64 //max epoll events

#define MAXPACKETSIZE 256 // max packet size including packet length byte

class Handler;

class Server {
public:
    void set_handler(Handler* h);

	int bind_server();

    void main_loop();

	void start_server();

    void disconnect(int id);

    void send_packet(int id, int packet_length, unsigned char* data);
	
private:
	int server_socket; // listen on server_socket
	int efd; // epoll file descriptor

	int s; //error int

    Handler* handler;

    struct epoll_event *events;

	struct rec_struct{
        int fd;
        User* user = nullptr;
		int position = 0;
		unsigned char buffer[2*MAXPACKETSIZE];
        bool done = false;
	};

    int make_socket_non_blocking (int sfd);

    void handle_epoll_events(int n);
    void handle_new_connections();
    void handle_data_from_socket(const struct epoll_event& event);
    void handle_socket_error(const struct epoll_event& event);

    bool is_listener_event(const struct epoll_event& event);
    bool is_error_event(const struct epoll_event& event);


    
};

#endif