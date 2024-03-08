#include "UIRenderer.h"
#include "../protocol_definitions.h"

#include <iostream>
#include <sys/ioctl.h>
#include <unistd.h>

#include <ncurses.h>
#include <string.h>



UIRenderer::UIRenderer(State* state){
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    console_heigth = w.ws_row;
    console_width = w.ws_col;

    s = state;
}
void UIRenderer::render(){

    if(!s->logged_in || s->status == NOT_IN_GAME)
        return;
    if(s->status == IN_QUEUE){
        erase();
        print_centered("Waiting for opponent...\n");
        refresh();
        return;
    }
    erase();
    if(s->status == YOU_WIN){
        addstr("You won!\n");
    }
    if(s->status == OPPONENT_WIN){
        addstr("You lost...\n");
    }
    if(s->status == DRAW){
        addstr("Game ended in draw.\n");
    }


    addstr(s->opponent_name.c_str());
    if(s->message!=""){
        addstr(": ");
        addstr(s->message.c_str());
    }

    addch('\n');
    addstr(s->board.c_str());
    addch('\n');
    addstr(s->username.c_str());
    addstr("\n\n");
    addstr(s->server_message.c_str());
    addstr("\n\n");
    for(int i = 0; i< s->pos; i ++)
        addch(s->input_buffer[i]);
    refresh();
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

void UIRenderer::print_centered(const char* text, bool in_x, bool in_y, bool cl, int x, int y){
    if(cl)
        erase();

    int start_x = x;
    int start_y = y;
    if(in_x)
        start_x  = (COLS   - get_text_width(text) )/2 + x;
    if(in_y)
        start_y  = (LINES  - get_text_height(text))/2 + y;


    char* t = strdup(text);
    char *line = strtok(t, "\n");
    while(line != nullptr){
        mvprintw(start_y++, start_x, "%s", line);
        line = strtok(nullptr, "\n");
    }

}

void UIRenderer::gotoxy(int x, int y) {
        printf("\033[%d;%dH", y, x);
        cursor_x = x;
        cursor_y = y;
}

void UIRenderer::show_cursor() {
    std::cout << "\033[?25h"; // ANSI escape sequence to show cursor
}
void UIRenderer::login_screen(){
    print_centered(small_banner, true, false);

    std::string info  = "Connected to " + s->server + ':' + s->port;
    int y;
    int x;
    getyx(stdscr, y, x);
    mvprintw(y + 1, x - 38, info.c_str());
    //print_centered(info.c_str(), true, false, false, -20);
    mvprintw(LINES/2, COLS/2 - COLS*1/3, "Enter username: ");
    refresh();

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