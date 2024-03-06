#ifndef UIRENDERER_H
#define UIRENDERER_H

#include "State.h"


//Class responsible for dispaying the information in State using the render method

class UIRenderer{
public:
    UIRenderer(State* state);
    void render();
    
    void gotoxy(int x, int y);
    void hide_cursor();
    void show_cursor();

    int get_text_height(const char* text);
    int get_text_width(const char* text);
    void print_centered(const char* text, bool in_x = true, bool in_y = true, bool clear = true , int x = 0, int y = 0);

    void login_screen();
    
    int console_width;
    int console_heigth;
    int cursor_x;
    int cursor_y;

    State* s;
};

extern const char* small_banner;
extern const char* banner;

#endif