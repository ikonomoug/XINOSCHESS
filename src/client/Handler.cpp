#include "Handler.h"
#include "State.h"
#include "UIRenderer.h"
#include <iostream>
#include <ncurses.h>


Handler::Handler(State* state, UIRenderer* uir){
    s = state;
    ui = uir;
}
void Handler::set_client(Client* c){
    client = c;
}

void Handler::input(){
    int c;
    while( (c = getch() )!= ERR){
        if(c != '\n'){
            int y;
            int x;
            getyx(stdscr, y, x);

            if(c == KEY_BACKSPACE){
                if(s->pos <= 0){
                    beep();
                    continue;
                }
                    
                
                s->pos--;
                
                move(y,x-1);
                addch(' ');
                ui->render();
                continue;
            }
            s->input_buffer[s->pos] = c;
            s->pos++;
            addch(c);
            move(y,x+1);
            ui->render();
            //refresh();
            continue;
        }
        
        
        std::string move(s->input_buffer, s->pos);
        s->pos = 0;
        ui->render();

        if(move == "exit"){

            endwin();
            printf("\e[ q");
            exit(0);

        }
        if(move == "info"){
            ui->print_centered("\"e2e4\" to make a move\n\"resign\" to resign\n\"draw\" to offer/accept draw\n\"cdraw\" to cancel offer\n\"newgame\" to start a new game after\n\"message your message\" to send a message\n");
            refresh();
            sleep(6);
            if(s->server_message == "Type \"info\" for information\n")
                s->server_message = "";
            
            ui->render();
            continue;
        }
        if(move == "newgame"){
            s->message ="";
            s->server_message = "";
            
            unsigned char data = JOIN_GAME;
            client->send_packet(1, &data);
            continue;
        }
        if(move == "resign"){
            unsigned char data = RESIGN;
            client->send_packet(1, &data);
            continue;
        }
        if(move == "draw"){
            unsigned char data = OFFER_DRAW;
            client->send_packet(1, &data);
            continue;
        }
        if(move == "cdraw"){
            unsigned char data = CANCEL_DRAW;
            client->send_packet(1, &data);
            continue;
        }
        if(move.substr(0, 7) == "message"){
            move = move.substr(8);

            int length = move.length() + 1 > 254?254:move.length()+1;
            unsigned char data[length];
            data[0] = SEND_MESSAGE;
            for(int i = 0; i < length; i++)
                data[i+1] = move.c_str()[i];
            client->send_packet(length, data);
            ui->render();
            continue;
        }
        int length = move.length() + 1 > 255?255:move.length()+1;
        unsigned char* data = new unsigned char[length];
        data[0] = (unsigned char )MOVE;
        for(int i = 0; i < length; i++){
            data[1+i] = move.c_str()[i];
        }
        
        client->send_packet(length, data);

    }
}

void Handler::handle_packet(int packet_length, unsigned char* data){
    reply r = reply(data[0]);
    game_data gd;
    login_reply lr;


    switch(r){
        case ERROR : break;

        case GAME_STATUS :

        s->status = game_status(data[1]); 
        break;

        case GAME_DATA : 
            gd = game_data(data[1]);
            switch(gd){
                case BOARD          : s->board.assign((char*)(data + 2), packet_length - 2) ; break;
                case OPPONENT_NAME  : s->opponent_name.assign((char*)(data + 2), packet_length - 2); break;
                case MESSAGE        : s->message.assign((char*)(data + 2), packet_length - 2); break;
                case SERVER_MESSAGE : s->server_message.assign((char*)(data + 2), packet_length - 2); break;
            }
        
        break;

        case LOGIN_REPLY : 
            lr = login_reply(data[1]);
            switch(lr){
                case LOGGED_IN         : 

                s->logged_in = true; 
                if(s->status == NOT_IN_GAME){
                    unsigned char m = (unsigned char)JOIN_GAME;
                    client->send_packet(1, &m);
                }
                break;
                
                case BAD_LOGIN         : 
                ui->print_centered("Error: Bad Login\n");
                refresh(); 
                sleep(1);
                endwin();
                printf("\e[ q");
                exit(1); 
                break;
                case DUPLICATE_SESSION : 
                ui->print_centered("Error: User already logged in\n");
                 refresh(); 
                sleep(1);
                endwin();
                printf("\e[ q");
                exit(1); 
                 break;
            }
        
        
        break;
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
    
	delete[] message;
}