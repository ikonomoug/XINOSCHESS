#include "Handler.h"
#include "Server.h"
#include "UserPool.h"

#include <iostream>







void Handler::set_server(Server* s){
    server = s;
}

void Handler::send_game_data(User* user){
	if(user->in_game){
			if(user->turn)
				send_reply(user, YOUR_TURN, nullptr, 0);
			else{
				send_reply(user, OPPONENT_TURN, nullptr, 0);
			}
			std::string opponent_name = user->opponent->get_username();
			send_reply(user, OPPONENT_NAME, (unsigned char*)opponent_name.c_str(), opponent_name.length());
			send_board(user);
		}
}

User* Handler::handle_login_packet(int id, int packet_length, unsigned char* data){
// TODO : disconnect currently connected user
	action test = (action)data[0];
	if(test == LOGIN && packet_length > 1 && packet_length <= 17){

		std::string username((char *)(data + 1), packet_length - 1);

		User* u = users.get_user_from_username(username);
		if( u == nullptr){
			u = new User(username, id);

			users.add_user(u);
		}else{
			u->login(id);
		}

		send_game_data(u);
		send_reply(u, LOGGED_IN, nullptr, 0);
		return u;
		
	}else{
		//TODO: SEND LOGIN ERROR
		return nullptr;
	}
}


void Handler::send_reply(User* user, Handler::reply r, unsigned char* data, int size){
	
	unsigned char* message = new unsigned char[size + 1];
	message[0] = (unsigned char)r;

	for(int i = 0; i < size; i++){
		message[i + 1] = data[i];
	}

	server->send_packet(user->get_id(), size + 1, message);
	delete[] message;
}

void Handler::handle_join_game(User* user, int packet_length, unsigned char* data){

	if(users.someone_waiting_match){
		
		User* opponent;
		user->opponent = opponent = users.user_waiting;
		opponent->opponent = user;

		users.user_waiting = nullptr;
		
		users.someone_waiting_match = false;
		opponent->waiting_for_opponent = false;

		user->game = opponent->game = new Game();
		user->in_game = opponent->in_game = true;

		user->turn = false;
		user->white = false;
		opponent->turn = true;
		opponent->white = true;

		send_game_data(user);
		send_game_data(opponent);
	}
	else{
		users.someone_waiting_match = true;
		users.user_waiting = user;
		user->waiting_for_opponent = true;

		send_reply(user, WAITING_FOR_OPPONENT, nullptr, 0);
	}
}

void Handler::send_board(User* user){

	std::cout << "Server: board to " << user->get_username()<< '\n';
	if(user->is_in_game()){
		std::string board = user->game->draw(user->white);

		send_reply(user, BOARD, (unsigned char*)board.c_str(), board.length());
	}
	else{
		send_reply(user, ERROR, nullptr, 0);
	}
}

void Handler::handle_move(User* user, int packet_length, unsigned char* data){

	if(user->is_in_game() && user->turn){

		std::string move((char*)data, packet_length);

		user->game->updatepossiblemoves();
		if(user->game->receivemove(move)){
			user->game->incrmovenum();

			user->turn = false;
			user->opponent->turn = true;

			send_game_data(user);
			send_game_data(user->opponent);
		}
		else{
			send_reply(user, BAD_MOVE, nullptr, 0);
		}
		
	}
	else{
		send_reply(user, ERROR, nullptr, 0);
	}
}

void Handler::handle_message(User* user, int packet_length, unsigned char* data){
	if(user->in_game && user->opponent->is_connected()){
		send_reply(user->opponent, NEW_MESSAGE, data, packet_length);
	}
}


void Handler::handle_packet(User* user, int packet_length, unsigned char* data){


	std::cout << user->get_username() <<" :"<< (unsigned int)data[0] << " ";
    for(int i = 1; i < packet_length; i++){
        std::cout << data[i];
    }
    std::cout << std::endl;

	//interpret
	if(packet_length >=1){
		action command = (action)data[0];

		switch(command)
    {
		case JOIN_GAME : handle_join_game(user, packet_length - 1, data + 1);    break;
		case MOVE      : handle_move     (user, packet_length - 1, data + 1);    break;
		case MESSAGE   : handle_message  (user, packet_length - 1, data + 1);    break;
    }
	}

}

