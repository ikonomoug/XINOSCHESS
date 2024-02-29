#ifndef HANDLER_H
#define HANDLER_H

#include "State.h"
#include "UIRenderer.h"
#include "Client.h"

class Client;
class Handler{
public:
    Handler(State* state, UIRenderer* uir);
    void set_client(Client* c);
    void input_loop();

    void handle_packet(int packet_length, unsigned char* data);

    void login();

    State* s;
    UIRenderer* ui;
    Client* client;

    enum reply : unsigned char {WAITING_FOR_OPPONENT =48, BOARD, ERROR, BAD_MOVE, YOUR_TURN, OPPONENT_TURN, LOGGED_IN, OPPONENT_NAME, GAME_STATUS, NEW_MESSAGE};
    enum action : unsigned char {LOGIN = 48, LOGOUT, JOIN_GAME, MOVE, MESSAGE, ABORT_GAME};
};


#endif