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
    void disconnect(User* user);

    void send_reply(User* user, reply r, unsigned char* data, int size);

    void send_game_status(User* user);

    void send_board(User* user);
    void send_opponent_name(User* user);
    void send_server_message(User* user, int length, unsigned char* data);



    bool handle_login_packet(int fd, int packet_length, unsigned char* data, User* &user);

    bool handle_packet(User* user, int packet_length, unsigned char* data);
        bool handle_join_game   (User* user, int length, unsigned char* data);
        bool handle_move        (User* user, int length, unsigned char* data);
        bool handle_resign      (User* user, int length, unsigned char* data);
        bool handle_offer_draw  (User* user, int length, unsigned char* data);
        bool handle_cancel_draw (User* user, int length, unsigned char* data);
        bool handle_send_message(User* user, int length, unsigned char* data);
    

    

};



#endif