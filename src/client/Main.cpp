
#include <iostream>
#include <unistd.h>

#include <string>

#include <ncurses.h>
#include <locale.h>

#include <sys/epoll.h>

#include "UIRenderer.h"
#include "Handler.h"
#include "Client.h"
#include "State.h"

#define MAX_EVENTS 5
#define READ_SIZE 10



int main(int argc, char** argv){
    setlocale(LC_ALL, "");
    initscr();

    State s;
    
    s.server = SERVER;
    s.port = PORT;

    if(argc > 1){
        if(argc == 2){
            s.server = argv[1];
        }
        else if(argc == 3){
            s.server = argv[1];
            s.port = argv[2];
        }
        else{
            return 0;
        }
    }
    
    UIRenderer ui(&s);
    Handler handler(&s, &ui);
    Client client(s.server, s.port);

    handler.set_client(&client);
    client.set_handler(&handler);


    curs_set(0);

    ui.print_centered(banner);
    refresh();

    sleep(1);

    if(!client.connect_server()){
        ui.print_centered("Error: Failed to connect\n");
        refresh();
        sleep(1);

        endwin();
        printf("\e[ q");
        return 2;
    }


    ui.login_screen();
    curs_set(2);
    char name[17];

    getnstr(name, 16);

    s.username = name;
    handler.login();

    nodelay(stdscr,true);
    noecho();
    keypad(stdscr, TRUE);

    int  event_count;
	struct epoll_event event,event1, events[MAX_EVENTS];

    int epoll_fd = epoll_create1(0);
    event.events = EPOLLIN;
	event.data.fd = 0;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event);
    event1.events = EPOLLIN;
	event1.data.fd = client.sockfd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client.sockfd, &event1);

    while (true) {
		event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
		for (int i = 0; i < event_count; i++) {

            if(events[i].data.fd == 0)
                handler.input();
            else
                client.receive();
		}
    }

    

    endwin();
    return 0;
}