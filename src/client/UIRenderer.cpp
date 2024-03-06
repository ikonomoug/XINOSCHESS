#include "UIRenderer.h"
#include "../protocol_definitions.h"

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>



UIRenderer::UIRenderer(State* state){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    console_heigth = w.ws_row;
    console_width = w.ws_col;
    hide_cursor();

    s = state;
}
void UIRenderer::render(){
// simple hacky version
// TODO: MULTIMENU/ MAYBE USING NCURSES
    if(!s->logged_in)
        return;
    if(s->status == IN_QUEUE){
        hide_cursor();
        print_centered("Waiting for opponent...\n");
        
        return;
    }
    system("clear");
    if(s->status == YOU_WIN){
        std::cout << "You won!\n";
    }
    if(s->status == OPPONENT_WIN){
        std::cout << "You lost...\n";
    }
    if(s->status == DRAW){
        std::cout << "Game ended in draw.\n";
    }

    show_cursor();

    std::cout << s->opponent_name;
    if(s->message!="") 
        std::cout << ": " << s->message;
    std::cout <<'\n';
    std::cout << s->board << '\n';
    std::cout << s->username << "\n\n";
    std::cout << s->server_message <<"\n\n";
}

int UIRenderer::get_text_width(const char* text){
    int max_width = 0;
    int i = 0;
    while(*text != '\0'){

        if(*text == '\n'){
            if(i > max_width)
                max_width = i;
            i = 0;
        }
        if ((*text & 0x80) == 0 || (*text & 0xc0) == 0xc0)
            i++;

        text++;

    }
    return max_width;
}

int UIRenderer::get_text_height(const char* text){
    int height = 1;
    while(*text != '\0'){
        if(*text == '\n')
            height++;
        text++;
    }
    return height;
}

void UIRenderer::print_centered(const char* text, bool in_x, bool in_y, bool clear, int x, int y){
    if(clear)
        system("clear");
    
    int side_space = (console_width   - get_text_width(text) )/2 + x;
    int top_space  = (console_heigth  - get_text_height(text))/2 + y;

    if(in_y){
        for(int i = 0; i < top_space; i++)
            std::cout << '\n';
    }
    
    
    const char* p = text;
    while(*p != '\0'){
        if(in_x && (p == text || *(p - 1) == '\n')){
            for(int i = 0; i < side_space; i++)
                std::cout << ' ';
        }
        std::cout << *p;
        p++;
    }
    std::cout << std::flush;
}

void UIRenderer::gotoxy(int x, int y) {
        printf("\033[%d;%dH", y, x);
        cursor_x = x;
        cursor_y = y;
}

void UIRenderer::hide_cursor() {
    std::cout << "\033[?25l"; // ANSI escape sequence to hide cursor
}
void UIRenderer::show_cursor() {
    std::cout << "\033[?25h"; // ANSI escape sequence to show cursor
}
void UIRenderer::login_screen(){
    print_centered(small_banner, true, false);
    std::cout << '\n';
    std::string info  = "Connected to " + s->server + ':' + s->port;
    print_centered(info.c_str(), true, false, false, -20);
    gotoxy(console_width/2 - console_width*1/3, console_heigth/2);
    std::cout << "Enter username: ";
    show_cursor();
}


const char* banner = R"(          ⠀⠀⠀⠀   ⢀⣠⣤⣄⡀⠀⠀                                                   _:_
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣿⣿⣏⣹⣿⠄⠀   ⠀██╗  ██╗██╗███╗   ██╗ ██████╗ ███████╗        '-.-' 
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⠿⠋⢠⣷⣦   ╚██╗██╔╝██║████╗  ██║██╔═══██╗██╔════╝       __.'.__
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ ⢸⣿⣿⡇⠀        ╚███╔╝ ██║██╔██╗ ██║██║   ██║███████╗      |_______|
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀ ⢸⣿⣿⣧⠀⠀       ██╔██╗ ██║██║╚██╗██║██║   ██║╚════██║       \=====/
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⠀⣿⣿⣿⣆⠀⠀     ██╔╝ ██╗██║██║ ╚████║╚██████╔╝███████║        )___(
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⠀⢀⣀⣿⣿⣿⣿⡆⠀⠀⠀ ⠀⠀╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚══════╝       /_____\
⠀⠀⠀⠀⠀⠀⠀⠀⠀  ⠀⢀⣤⣶⣿⣿⣿⠛⣿⣿⣿⣧⠀⠀ ⠀                                               |   |
⠀⠀⠀⠀⠀⠀  ⠀⢀⣠⣾⣿⣿⣿⣿⣿⣿⡇⢸⣿⣿⣿⠀⠀                                                 |   |
⠀⠀⠀⠀  ⠀⣠⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⠇⢸⣿⣿⡿⠀⠀   ██████╗██╗  ██╗███████╗███████╗███████╗       |   |
⠀⠀ ⢀⣠⣴⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠿⠋⣠⣿⣿⣿⠇   ⠀██╔════╝██║  ██║██╔════╝██╔════╝██╔════╝       |   |
⠰⢾⣿⣿⣿⡟⠿⠿⣿⣿⠿⠿⠛⠋⣁⣴⣾⣿⣿⠿⠋      ██║     ███████║█████╗  ███████╗███████╗       |   |
 ⠀⠉⠛⠻⠷⣶⣤⣤⣤⣤⣶⣾⣿⡿⠿⠛⠉⠀⠀ ⠀⠀⠀  ⠀██║     ██╔══██║██╔══╝  ╚════██║╚════██║      /_____\
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⢀⣶⠀⠀⠀⠀ ⠀⠀⠀⠀⠀⠀ ⠀╚██████╗██║  ██║███████╗███████║███████║     (=======)
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠘⠛⠛⠛⠛⠛⠂ ⠀         ╚═════╝╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝⠀⠀⠀ (_________))";
const char* small_banner = R"(██╗  ██╗██╗███╗   ██╗ ██████╗ ███████╗
╚██╗██╔╝██║████╗  ██║██╔═══██╗██╔════╝
 ╚███╔╝ ██║██╔██╗ ██║██║   ██║███████╗
 ██╔██╗ ██║██║╚██╗██║██║   ██║╚════██║
██╔╝ ██╗██║██║ ╚████║╚██████╔╝███████║
╚═╝  ╚═╝╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚══════╝)";