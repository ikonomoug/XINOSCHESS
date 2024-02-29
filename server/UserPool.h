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

};

class UserPool {
private:
	std::unordered_map<std::string, User*> data;
    std::unordered_map<int, User*> id_to_user;

	std::mutex data_mutex_;

	

public:
	bool someone_waiting_match = false;
	User* user_waiting = nullptr;

	void add_user(User* user){
		std::lock_guard<std::mutex> lock(data_mutex_);
		data[user->get_username()] = user;
        id_to_user[user->session_id] = user;
		std::cout << user->get_username() << " was added to playerbase\n";
	}


    void set_online(std::string username,int id){
        User* u = data[username];
        u->connected = true;
        u->session_id = id;
    }
    User* get_user_from_id(int id){
        if(id_to_user.contains(id))
            return id_to_user[id];
            
        return nullptr;
        
    }
	User* get_user_from_username(std::string username){
        if(data.contains(username))
            return data[username];
            
        return nullptr;
        
    }

    void logout(User* u){
        u->connected = false;
        id_to_user.erase(u->session_id);
        u->session_id = -1;
    }

};

#endif