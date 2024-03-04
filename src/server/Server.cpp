#include "Server.h"
#include "Handler.h"
#include "UserPool.h"

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>

#include <iostream>

#include <assert.h>



void Server::set_handler(Handler* h){
    handler = h;
}

int  Server::make_socket_non_blocking (int sfd){
	int flags;

	flags = fcntl (sfd, F_GETFL, 0);

	if (flags == -1){
		perror ("fcntl");
		return -1;
	}

	flags |= O_NONBLOCK;
	s = fcntl (sfd, F_SETFL, flags);
	if (s == -1){
		perror ("fcntl");
		return -1;
	}

	return 0;
}

int Server::bind_server(){
	std::cout << "Binding server...\n";
	struct addrinfo hints, *servinfo, *p;
	int yes = 1;
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

	if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	// loop through all the results and bind to the first we can
	for(p = servinfo; p != NULL; p = p->ai_next) {
		if ((server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("server: socket");
			continue;
		}

		if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}

		if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1){
			close(server_socket);
			perror("server: bind");
			continue;
		}

		break;
	}

	freeaddrinfo(servinfo); // all done with this structure

	if (p == NULL) {
		fprintf(stderr, "server: failed to bind\n");
		exit(1);
	}


	return server_socket;
}

inline void Server::handle_new_connections(){
    while (true){
        struct sockaddr in_addr;
        socklen_t in_len;
        int infd;
        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

        in_len = sizeof in_addr;
        infd = accept (server_socket, &in_addr, &in_len);
        if (infd == -1){
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                    /* We have processed all incoming
                    connections. */
                    break;
            }
            else{
                perror ("accept");
                break;
            }
        }

        s = getnameinfo (&in_addr, in_len, hbuf, sizeof hbuf, sbuf, sizeof sbuf, NI_NUMERICHOST | NI_NUMERICSERV);
        if (s == 0){
            printf("Accepted connection on descriptor %d "
                    "(host=%s, port=%s)\n", infd, hbuf, sbuf);
        }

    s = make_socket_non_blocking (infd);
    if (s == -1)
        abort ();


    // add new socket to epoll loop
    struct epoll_event event;

    socket_data_struct* socket_data = new socket_data_struct;
    event.data.ptr = socket_data;
    socket_data->fd = infd;

    event.events = EPOLLIN | EPOLLET;
    s = epoll_ctl (efd, EPOLL_CTL_ADD, infd, &event);

    if (s == -1){
        perror ("epoll_ctl");
        abort ();
    }

    }
}

inline bool Server::is_error_event(const struct epoll_event& event){
    return (event.events & EPOLLERR) || (event.events & EPOLLHUP) || (!(event.events & EPOLLIN));
}
inline bool Server::is_listener_event(const struct epoll_event& event){
    return server_socket == event.data.fd;
}

inline void Server::handle_socket_error(const struct epoll_event& event){
    socket_data_struct* socket_data = (socket_data_struct*)event.data.ptr;
    int fd = socket_data->fd;
    fprintf (stderr, "epoll error\n");
    
    socket_data->user->logout();
    delete socket_data;
    close(fd);
}


inline void Server::handle_data_from_socket(const struct epoll_event& event){
        /* We have data on the fd waiting to be read. Read and
        display it. We must read whatever data is available
        completely, as we are running in edge-triggered mode
        and won't get a notification again for the same data. */

    
    bool done = false;
    socket_data_struct* socket_data = (socket_data_struct*)event.data.ptr;

    int fd = socket_data->fd;
    int &position = socket_data->position;
    unsigned char* buf = socket_data->buffer;
    User* &user = socket_data->user;

    while (true){
        if (done){
            printf ("Closed connection on descriptor %d\n", fd);

            user->logout();
            delete socket_data;
            close(fd);

            break;
        }   	

        ssize_t count = read(fd, buf + position, 2*MAXPACKETSIZE - position);

        if (count == -1){
            /* If errno == EAGAIN, that means we have read all data. So go back to the main loop. */
            if (errno != EAGAIN){
                perror ("read");
                done = true;
                continue;
            }
            break;
        }
        else if (count == 0){
            // End of file. The remote has closed the connection.

            done = true;
            continue;
        }

        // received some data

        position = position + count;
        int i = 0;
        unsigned int packet_length = buf[0];

        while(position - i - 1 >= packet_length){// I have more data than the packet length, so full packet

            //give packet to handler    
            
            if(user == nullptr){
                user = handler->handle_login_packet(fd, packet_length, buf + i + 1);
            }
            else{
                handler->handle_packet(user, packet_length, buf + i + 1);
            }
            



            i = i + packet_length + 1;
             // If there is at least one more byte, get the next packet length, else stop the loop
            if(i < position)
                packet_length = buf[i];
            else                        //No more data
                break;
        }
        //if have read at least one packet move buffer to front
        
        if(i != 0 ){
            for(int j = 0; j < position - i; j++)
                buf[j] = buf[i + j];
                
            position = position - i;
        }
    }

}

inline void Server::handle_epoll_events(int n){

    for (int i = 0; i < n; i++){	
        epoll_event event = events[i];

        if (is_error_event(event))
           handle_socket_error(event);

        else if (is_listener_event(event))
            handle_new_connections();

        else
            handle_data_from_socket(event);
    }
}

void Server::main_loop(){
    events = new epoll_event[MAXEVENTS]();
	std::cout << "Waiting for connections...\n";

    while(true) {
		int n = epoll_wait(efd, events, MAXEVENTS, -1); 
        
		handle_epoll_events(n);
	}
}


void Server::start_server(){   

	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	char address_string[INET6_ADDRSTRLEN];

	server_socket = bind_server();

	if(make_socket_non_blocking (server_socket) == -1)
		abort();

	if(listen(server_socket, BACKLOG) == -1){
		perror("listen");
		exit(1);
	}


	efd = epoll_create1(0);

	if (efd == -1){
		perror ("epoll_create");
		abort ();
	}

    struct epoll_event event;

	event.data.fd = server_socket;
	event.events = EPOLLIN | EPOLLET;

	s = epoll_ctl (efd, EPOLL_CTL_ADD, server_socket, &event);
	if (s == -1){
		perror ("epoll_ctl");
		abort ();
	}


    main_loop();
	
}

void Server::send_packet(int id, int packet_length, unsigned char* data){//TODO: MAYBE IMPLEMENT IN EPOLL WITH EPOLLOUT, BUFFER

    int total = 0;        // how many bytes we've sent
    int bytesleft = packet_length; // how many we have left to send
    int n;
    unsigned char p  = (unsigned char)packet_length;

    n = send(id, &p, 1, 0);

    while(total < packet_length) {
        n = send(id, data + total, bytesleft, 0);

        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }
}