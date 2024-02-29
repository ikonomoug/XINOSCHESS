





#include <iostream>


#include "Server.h"	
#include "Handler.h"



int main(){
	Handler handler;
	Server server;
	handler.set_server(&server);
	server.set_handler(&handler);

	server.start_server();

}