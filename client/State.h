#ifndef STATE_H
#define STATE_H
#include <string>

class State{
public:
enum menu : int {LOGIN, MAIN, GAME, INFO};


std::string server;
std::string port;


std::string username;

menu current_menu = LOGIN;

bool logged_in = false;
bool waiting_for_opponent = false;
bool in_game = false;

bool bad_move;
bool turn;
std::string board;
std::string opponent_name;

};

#endif