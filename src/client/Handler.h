#ifndef HANDLER_H
#define HANDLER_H
#include "../protocol_definitions.h"
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

};


#endif