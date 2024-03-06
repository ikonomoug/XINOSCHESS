#ifndef STATE_H
#define STATE_H
#include <string>
#include "../protocol_definitions.h"

class State{
public:
enum menu : int {LOGIN, MAIN, GAME, INFO};
menu current_menu = LOGIN;


std::string server;
std::string port;
std::string username;


bool logged_in = false;
game_status status = NOT_IN_GAME;

std::string board;
std::string opponent_name;
std::string message ="";
std::string server_message ="Type \"info\" for information\n";

};

#endif