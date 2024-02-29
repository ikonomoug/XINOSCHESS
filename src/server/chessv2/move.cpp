#include "move.h"
#include "utilities.h"
#include "piece.h"
#include<string>

Move::Move(int a, int b, int c, int d) :x1(a),x2(c),y2(d),allowed(true)
{
    y1 = b;
    coords = util::numstocoords(a, b, c, d);
    if (b >10) {
       coords[1] = 'x';//for enpassant
       y1 = b / 10;
    }
}

Move::Move(const string& temp) {
    coords = temp;
    allowed = true;
    if (coords == "O-O") {
        y1 = y2 = 0;
        x1 = 4;
        x2 = 6;
    }
    else if (coords == "O--O") {
        y1 = y2 = 0;
        x1 = 4;
        x2 = 2;
    }
    else if (coords == "o-o") {
        y1 = y2 = 7;
        x1 = 4;
        x2 = 6;
    }
    else if (coords == "o--o") {
        y1 = y2 = 7;
        x1 = 4;
        x2 = 2;
    }
    else {
        x1 = x2 = y1 = y2 = -1;
    }
}

Move& Move::operator=(const Move& other) {
    if (this == &other) {
        return *this;
    }
    x1 = other.x1;
    x2 = other.x2;
    y1 = other.y1;
    y2 = other.y2;
    coords = other.coords;
    return *this;
}
