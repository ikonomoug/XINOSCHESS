#ifndef USERPOOL_H
#define USERPOOL_H


#include <string>
#include <condition_variable>
#include <unordered_map>
#include <iostream>

#include "chessv2/game.h"

class User {
private:
		

		

public:
		int fd;
		std::string username;

		bool in_game;
		bool waiting_for_opponent;
		bool turn = false;

		bool connected;

		
		bool white;
		Game* game = nullptr;
		User* opponent = nullptr;

		User(std::string name, int fd){
				this->fd = fd;
				username = name;
				in_game = false;
				waiting_for_opponent = false;
				connected = true;
		}
		std::string get_username(){
				return username;
		}
		int get_fd(){
				return fd;
		}
		bool is_connected(){
				return connected;
		}
		bool is_in_matchmaking(){
				return waiting_for_opponent;
		}
		bool is_in_game(){//
				return in_game;
		}
		void login(int fd){
			connected = true;
			this->fd = fd;
		}

		void logout(){
        	connected = false;
        	fd = -1;
			waiting_for_opponent = false;
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