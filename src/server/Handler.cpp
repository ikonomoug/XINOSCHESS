#include "Handler.h"
#include "Server.h"
#include "UserPool.h"

#include <iostream>







void Handler::set_server(Server* s){
    server = s;
}

void Handler::disconnect(User* user){
	if(user != nullptr){
        std::cout << user->username <<": logout\n";
		
		if(user->in_queue()){
			users.someone_waiting_match = false;
			users.user_waiting = nullptr;
		}
			
		user->logout();
    }
}

void Handler::send_reply(User* user, reply r, unsigned char* data, int size){
	if(!user->online())
		return;
	
	unsigned char* message = new unsigned char[size + 1];
	message[0] = (unsigned char)r;

	for(int i = 0; i < size; i++){
		message[i + 1] = data[i];
	}

	server->send_packet(user->get_fd(), size + 1, message);
	delete[] message;
}


void Handler::send_game_status(User* user){
	unsigned char r = user->status;
	
	send_reply(user, GAME_STATUS, &r, 1);
}

void Handler::send_board(User* user){

	std::cout << "Server: board to " << user->get_username()<< '\n';
	if(user->in_game()){
		std::string board = user->game->draw(user->white);
		unsigned char data[board.length() + 1];
		data[0] = BOARD;
		for(int i = 0; i < board.length(); i++)
			data[i + 1] = (unsigned char)board.c_str()[i];

		send_reply(user, GAME_DATA, data, board.length() + 1);
	}
	else{
		send_reply(user, ERROR, nullptr, 0);
	}
}

void Handler::send_opponent_name(User* user){
	std::string opponent_name = user->opponent->get_username();

	unsigned char data[opponent_name.length() + 1];
	data[0] = OPPONENT_NAME;
	for(int i = 0; i < opponent_name.length(); i++)
		data[i + 1] = (unsigned char)opponent_name.c_str()[i];
	
	send_reply(user, GAME_DATA, data, opponent_name.length() + 1);
}

void Handler::send_server_message(User* user, int length, unsigned char* data){
	unsigned char message[length + 1];
	message[0] = SERVER_MESSAGE;

	for(int i = 0; i < length; i++)
		message[i + 1] = data[i];

	send_reply(user, GAME_DATA, message, length + 1);
}





bool Handler::handle_login_packet(int fd, int packet_length, unsigned char* data, User* &user){
// if its a login packet, the username at most 16 chars and the user is not connected,
// user is logged in, returns true and assigns the user
	unsigned char r[2];
	r[0] = LOGIN_REPLY;

	action test = action(data[0]);
	if(test == LOGIN && packet_length > 1 && packet_length <= 17){

		std::string username((char *)(data + 1), packet_length - 1);
		User* u = users.get_user_from_username(username);


		if( u == nullptr){
			u = new User(username, fd);
			users.add_user(u);
		}
		else if(u->online()){
			
			r[1] = DUPLICATE_SESSION;
			server->send_packet(fd, 2, r);
			

			user = nullptr;
			return false;
		}
		else{
			u->login(fd);
		}

		send_game_status(u);

		if(u->in_game()){
			send_board(u);
			send_opponent_name(u);
		}
		

		r[1] = LOGGED_IN;
		server->send_packet(fd, 2, r);

		user = u;
		return true;
		
	}
	else{
		r[1] = BAD_LOGIN;
		server->send_packet(fd, 2, r);

		user = nullptr;
		return false;
	}
}

bool Handler::handle_packet(User* user, int packet_length, unsigned char* data){

	//print packet in terminal
	std::cout << user->get_username() <<": "<< (unsigned int)data[0] << " ";
    for(int i = 1; i < packet_length; i++){
        std::cout << data[i];
    }
    std::cout << std::endl;

	//interpret
	if(packet_length < 1)
		return false;

	action command = action(data[0]);
	switch(command){

	case JOIN_GAME    : return handle_join_game   (user, packet_length - 1, data + 1); break;
	case MOVE         : return handle_move        (user, packet_length - 1, data + 1); break;
	case RESIGN       : return handle_resign      (user, packet_length - 1, data + 1); break;
	case OFFER_DRAW   : return handle_offer_draw  (user, packet_length - 1, data + 1); break;
	case CANCEL_DRAW  : return handle_cancel_draw (user, packet_length - 1, data + 1); break;
	case SEND_MESSAGE : return handle_send_message(user, packet_length - 1, data + 1); break;
	default           : return false;
	}

}

bool Handler::handle_join_game(User* user, int length, unsigned char* data){
	if(length != 0 || user->in_active_game()){
		send_reply(user, ERROR, nullptr, 0);
		return false;
	}

	
	//if the opponent of the previous game has moved on, delete the game
	if((user->opponent != nullptr) && (user->opponent->game != user->game))
		delete user->game;
	
	user->opponent = nullptr;
	user->game = nullptr;

	if(users.someone_waiting_match){
		
		User* opponent;

		user->opponent = opponent = users.user_waiting;
		opponent->opponent = user;

		users.user_waiting = nullptr;
		users.someone_waiting_match = false;

		opponent->status = YOUR_TURN;
		user->status = OPPONENT_TURN;
		opponent->white = true;
		user->white = false;
		

		user->game = opponent->game = new Game();



		send_game_status(user);
		send_board(user);
		send_opponent_name(user);

		send_game_status(opponent);
		send_board(opponent);
		send_opponent_name(opponent);
	}
	else{
		users.someone_waiting_match = true;
		users.user_waiting = user;

		user->status = IN_QUEUE;

		send_game_status(user);
	}
	return true;
}



bool Handler::handle_move(User* user, int length, unsigned char* data){
	//TODO: add game ending handling, draw, fix castling

	if(user->in_active_game() && user->turn()){

		std::string move((char*)data, length);

		user->game->updatepossiblemoves();
		if(user->game->receivemove(move)){
			user->game->incrmovenum();

			user->status = OPPONENT_TURN;
			user->opponent->status = YOUR_TURN;

			send_game_status(user);
			send_board(user);

			send_game_status(user->opponent);
			send_board(user->opponent);

		}
		return true;
		
	}
	else{
		send_reply(user, ERROR, nullptr, 0);
		return false;
	}
}

bool Handler::handle_resign(User* user, int length, unsigned char* data){
	if(length != 0 || !user->in_active_game())
		return false;
	
	User* opponent = user->opponent;

	user->status = OPPONENT_WIN;
	opponent->status = YOU_WIN;
	std::string message = "You resigned\n";
	send_server_message(user, message.length(), (unsigned char *)message.c_str());
	message = "Your opponent resigned\n";
	send_server_message(opponent, message.length(), (unsigned char *)message.c_str());

	send_game_status(user);
	send_game_status(opponent);

	return true;
}

bool Handler::handle_offer_draw(User* user, int length, unsigned char* data){
	if(length != 0 || !user->in_active_game())
		return false;
	
	if(user->opponent->offers_draw){
		user->opponent->offers_draw = false;

		user->status = DRAW;
		user->opponent->status = DRAW;

		send_game_status(user);
		send_game_status(user->opponent);
	}
	else{
		user->offers_draw = true;

		std::string message = "You offered a draw\n";
		send_server_message(user, message.length(), (unsigned char *)message.c_str());
		message = "Your opponent offered a draw\n";
		send_server_message(user->opponent, message.length(), (unsigned char *)message.c_str());

	}
	return true;
}

bool Handler::handle_cancel_draw(User* user, int length, unsigned char* data){
	if(length != 0 || !user->in_active_game())
		return false;

	if(user->offers_draw){
		user->offers_draw = false;
		std::string message = "You canceled your draw offer\n";
		send_server_message(user, message.length(), (unsigned char *)message.c_str());
		message = "Your opponent cancelled the draw offer\n";
		send_server_message(user->opponent, message.length(), (unsigned char *)message.c_str());
	}
	return true;
}

bool Handler::handle_send_message(User* user, int length, unsigned char* data){
	if( !user->in_game())
		return false;
	
	unsigned char message[length + 1];
	message[0] = MESSAGE;
	for(int i = 0; i < length; i++)
		message[i + 1] = data[i];
	
	send_reply(user->opponent, GAME_DATA, message, length + 1);
	return true;
}