#pragma once
#include<assert.h>
#include "utilities.h"
class Piece;

using namespace std;

class Move {
public:
    Move(int, int, int, int);
    Move(const string&);
    Move& operator=(const Move&);
public:
    int x1, y1, x2, y2;
    string coords;
    bool allowed;
};
