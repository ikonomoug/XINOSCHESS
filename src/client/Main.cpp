
#include <iostream>
#include <unistd.h>

#include <string>
#include <thread>

#include "UIRenderer.h"
#include "Handler.h"
#include "Client.h"
#include "State.h"





int main(int argc, char** argv){

    State s;
    
    s.server = SERVER;
    s.port = PORT;

    if(argc > 1){
        if(argc == 2){
            s.server = argv[1];
        }
        else if(argc == 3){
            s.server = argv[1];
            s.port = argv[2];
        }
        else{
            return 0;
        }
    }
    
    UIRenderer ui(&s);
    Handler handler(&s, &ui);
    Client client(s.server, s.port);

    handler.set_client(&client);
    client.set_handler(&handler);



    ui.print_centered(banner);
    sleep(1);

    if(!client.connect_server()){
        ui.print_centered("Error: Failed to connect\n");
        sleep(1);
        system("clear");
        return 2;
    }
    std::thread client_thread(&Client::receiver, client);   
    
    
    ui.login_screen();
    std::string username;
    std::cin >> username;
    s.username = username;
    //ui.hide_cursor();
    handler.login();

    //sleep(1);

    std::thread input_thread(&Handler::input_loop, handler);
    //client login, start input handler



    client_thread.join();
    input_thread.join();

    system("clear");

}