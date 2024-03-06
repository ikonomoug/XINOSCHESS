#ifndef USERPOOL_H
#define USERPOOL_H


#include <string>
#include <unordered_map>
#include <iostream>

#include "../protocol_definitions.h"
#include "chessv2/game.h"

class User {
private:
		

		

public:
		int fd;
		bool connected;

		std::string username;

		game_status status;

		

		
		bool white;
		Game* game = nullptr;
		bool offers_draw = false;
		User* opponent = nullptr;

		User(std::string name, int fd){
				this->fd = fd;
				username = name;
				status = NOT_IN_GAME;
				connected = true;
		}
		std::string get_username(){
			return username;
		}
		int get_fd(){
			return fd;
		}
		bool online(){
			return connected;
		}
		bool in_queue(){
			return status == IN_QUEUE;
		}
		bool in_game(){//
			return !(status == NOT_IN_GAME) && !(status == IN_QUEUE);
		}
		bool in_active_game(){
			return (status == YOUR_TURN) || (status == OPPONENT_TURN);
		}
		bool turn(){
			return status == YOUR_TURN;
		}
		void login(int fd){
			connected = true;
			this->fd = fd;
		}

		void logout(){
        	connected = false;
        	fd = -1;
			if(status == IN_QUEUE)
				status == NOT_IN_GAME;
    	}

};

class UserPool {
private:
	std::unordered_map<std::string, User*> data;


	

public:
	bool someone_waiting_match = false;
	User* user_waiting = nullptr;

	void add_user(User* user){
		data[user->get_username()] = user;
		std::cout << "Server: New user " << user->get_username() << '\n';
	}


	User* get_user_from_username(std::string username){

		auto search = data.find(username);
        if(search != data.end())
            return search->second;
            
        return nullptr;
        
    }

};

#endif