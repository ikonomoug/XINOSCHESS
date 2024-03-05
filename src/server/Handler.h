#ifndef HANDLER_H
#define HANDLER_H

#include "../protocol_definitions.h"
#include "UserPool.h"

class Server;

class Handler{
private:
    
    Server* server;

    UserPool users;

public:
    void set_server(Server* s);

    bool handle_packet(User* user, int packet_length, unsigned char* data);

    bool handle_login_packet(int fd, int packet_length, unsigned char* data, User* &user);
    void handle_join_game(User* user, int packet_length, unsigned char* data);
    void handle_move(User* user, int packet_length, unsigned char* data);
    void handle_message(User* user, int packet_length, unsigned char* data);

    void send_board(User* user);
    void send_game_data(User* user);

    
    void send_reply(User* user, reply r, unsigned char* data, int size);
    
    void disconnect(User* user);


};



#endif