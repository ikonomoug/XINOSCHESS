
#include <iostream>
#include <unistd.h>

#include <string>
#include <thread>

#include "UIRenderer.h"
#include "Handler.h"
#include "Client.h"
#include "State.h"





int main(){

    State s;
    
    s.server = SERVER;
    s.port = PORT;
    
    UIRenderer ui(&s);
    Handler handler(&s, &ui);
    Client client(SERVER, PORT);

    handler.set_client(&client);
    client.set_handler(&handler);



    ui.print_centered(banner);
    sleep(1);

    if(!client.connect_server()){
        std::string e = "Error: Failed to connect\n";
        ui.print_centered(e.c_str());
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