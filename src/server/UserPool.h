#ifndef USERPOOL_H
#define USERPOOL_H


#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <iostream>

#include "chessv2/game.h"

class User {
private:
		

		

public:
		int session_id;
		std::string username;
		bool in_game;
		bool waiting_for_opponent;
		bool connected;
		bool game_ended;
		
		bool white;
		Game* game = nullptr;
		User* opponent = nullptr;
		bool turn = false;

		friend class UserPool;
		User(std::string name, int id){
				session_id = id;
				username = name;
				in_game = false;
				waiting_for_opponent = false;
				game_ended = false;
				connected = true;
		}
		std::string get_username(){
				return username;
		}
		int get_id(){
				return session_id;
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
			session_id = fd;
		}

		void logout(){
        connected = false;
        session_id = -1;
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
		std::cout << user->get_username() << " was added to playerbase\n";
	}


	User* get_user_from_username(std::string username){

		auto search = data.find(username);
        if(search != data.end())
            return search->second;
            
        return nullptr;
        
    }

};

#endif