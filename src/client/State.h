#ifndef STATE_H
#define STATE_H
#include <string>
#include "../protocol_definitions.h"



// State contains all relevant information shared between Handler and UIRenderer

class State{
public:

std::string server;
std::string port;
std::string username;


bool logged_in = false;
game_status status = NOT_IN_GAME;

std::string board;
std::string opponent_name;
std::string message ="";
std::string server_message ="Type \"info\" for information\n";

char input_buffer[254];
int pos = 0;

};

#endif