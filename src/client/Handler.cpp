#include "Handler.h"
#include "State.h"
#include "UIRenderer.h"
#include <iostream>


Handler::Handler(State* state, UIRenderer* uir){
    s = state;
    ui = uir;
}
void Handler::set_client(Client* c){
    client = c;
}

void Handler::input_loop(){
    while(true){
        std::string move;

        std::cin >> move;

        unsigned char* data = new unsigned char[move.length()+1];
        data[0] = (unsigned char )MOVE;
        for(int i = 0; i < move.length(); i++){
            data[1+i] = move.c_str()[i];
        }
        
        client->send_packet(move.length()+1, data);
    }
}

void Handler::handle_packet(int packet_length, unsigned char* data){
//TODO: CLEANUP, FUNCTION FOR EACH REPLY ETC
    reply r = reply(data[0]);

    switch(r)
    {   
        case WAITING_FOR_OPPONENT : s->waiting_for_opponent = true; break;
        case LOGGED_IN     : s->logged_in = true; break;
        case BOARD         : s->board.assign((char*)(data + 1), packet_length - 1) ;s->waiting_for_opponent = false; break;
        case YOUR_TURN     : s->turn = true;  s->in_game = true; s->waiting_for_opponent = false; break;
        case OPPONENT_TURN : s->turn = false; s->in_game = true; s->waiting_for_opponent = false; break;
        case OPPONENT_NAME : s->opponent_name.assign((char*)(data + 1), packet_length - 1); s->waiting_for_opponent = false; break;
        case BAD_MOVE      : s->bad_move = true; break;
        case BAD_LOGIN     : ui->print_centered("Error: Bad Login\n"); sleep(1); exit(1); break;
        case DUPLICATE_SESSION : ui->print_centered("Error: User already logged in\n"); sleep(1); exit(1); break;

    }
    ui->render();
}

void Handler::login(){
    int packet_length = s->username.length() + 1;

    unsigned char* message = new unsigned char[packet_length];

	message[0] = (unsigned char)LOGIN;

	for(int i = 0; i < packet_length - 1; i++){
		message[i + 1] = s->username.c_str()[i];
	}

	client->send_packet(packet_length, message);
    unsigned char m = (unsigned char)JOIN_GAME;
    client->send_packet(1, &m);
	delete[] message;
}