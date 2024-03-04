#ifndef HANDLER_H
#define HANDLER_H

#include "UserPool.h"

class Server;
class Handler{

public:
    void set_server(Server* s);
    void handle_packet(User* user, int packet_length, unsigned char* data);

    User* handle_login_packet(int id, int packet_length, unsigned char* data);
    void handle_logout(User* user, int packet_length, unsigned char* data);
    void handle_join_game(User* user, int packet_length, unsigned char* data);
    void handle_message(User* user, int packet_length, unsigned char* data);

    void send_board(User* user);
    void send_game_data(User* user);
    void handle_move(User* user, int packet_length, unsigned char* data);

    enum reply : unsigned char { WAITING_FOR_OPPONENT = 48, BOARD, ERROR, BAD_MOVE, YOUR_TURN, OPPONENT_TURN, LOGGED_IN, OPPONENT_NAME, GAME_STATUS, NEW_MESSAGE};
    void send_reply(User* user, reply r, unsigned char* data, int size);
    

private:
    enum action : unsigned char {LOGIN = 48, JOIN_GAME, MOVE, MESSAGE, ABORT_GAME};
    
    Server* server;

    UserPool users;

};



#endif