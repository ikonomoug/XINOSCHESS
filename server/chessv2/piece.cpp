#include "piece.h"

//mess
Piece::Piece(char a, int c, int d) {
    value = a;
    x = c;
    y = d;
    ingame = true;
}

void Piece::movepiece(const Move& move) {
    x = move.x2;
    y = move.y2;
}

void Piece::promote(char newtype) {
    if (getcolour() != 0) {
        if (newtype == 'Q' || newtype == 'R' || newtype == 'B' || newtype == 'N') value = newtype;
        else {
            cout << "incorrect input, enter again: ";
            cin >> newtype;
            promote(newtype);
        }
    }
    else {
        if (newtype == 'q' || newtype == 'r' || newtype == 'b' || newtype == 'n') value = newtype;
        else {
            cout << "incorrect input, enter again: ";
            cin >> newtype;
            promote(newtype);
        }
    }
}

int Piece::getcolour() const{
    return isupper(value);
}

char Piece::gettype() const {
    if (isupper(value)) return value+32;
    else return value;
}

void  Piece::getdiagonalmoves(vector<Move> &movelist,const char(&board)[8][8]) const {
    for (int i = 1; x + i < 8 && y+i<8; ++i) {
        if (board[x + i][y + i] == ' ') {
            movelist.push_back(Move(x, y, x + i, y + i));
        }
        else if (getcolour() != isupper(board[x + i][y + i])) { 
            movelist.push_back(Move(x, y, x + i, y + i));
            break; 
        }
        else break;
    }
    for (int i = 1; x + i < 8 && y - i>=0; ++i) {
        if (board[x + i][y - i] == ' ') {
            movelist.push_back(Move(x, y, x + i, y - i));
        }
        else if (getcolour() != isupper(board[x + i][y - i])) {
            movelist.push_back(Move(x, y, x + i, y - i));
            break;
        }
        else break;
    }
    for (int i = 1; x - i >=0 && y + i < 8; ++i) {
        if (board[x - i][y + i] == ' ') {
            movelist.push_back(Move(x, y, x - i, y + i));
        }
        else if(getcolour() != isupper(board[x - i][y + i])) {
            movelist.push_back(Move(x, y, x - i, y + i));
            break;
        }
        else break;
    }
    for (int i = 1; x - i >= 0 && y - i >= 0; ++i) {
        if (board[x - i][y - i] == ' ') {
            movelist.push_back(Move(x, y, x - i, y - i));
        }
        else if (getcolour() != isupper(board[x - i][y - i])) {
            movelist.push_back(Move(x, y, x - i, y - i));
            break;
        }
        else break;
    }
}

void  Piece::gethorizontalmoves(vector<Move>& movelist, const char(&board)[8][8]) const {

    for (int i = 1; x + i < 8; ++i) {
        if (board[x + i][y] == ' ') {
            movelist.push_back(Move(x, y, x + i, y));
        }
        else if (getcolour() != isupper(board[x + i][y])) {
            movelist.push_back(Move(x, y, x + i, y));
            break;
        }
        else break;
    }
 
    for (int i = 1; y + i< 8; ++i) {
        if (board[x][y+i] == ' ') {
            movelist.push_back(Move(x, y, x , y + i));
        }
        else if (getcolour() != isupper(board[x][y + i])) {
            movelist.push_back(Move(x, y, x, y + i));
            break;
        }
        else break;
    }
    
    for (int i = 1; y - i >=0; ++i) {
        if (board[x][y - i] == ' ') {
            movelist.push_back(Move(x, y, x, y - i));
        }
        else if (getcolour() != isupper(board[x][y - i])) {
            movelist.push_back(Move(x, y, x, y - i));
            break;
        }
        else break;
    }
    
    for (int i = 1; x - i >= 0; ++i) {
        if (board[x-i][y] == ' ') {
            movelist.push_back(Move(x, y, x-i, y));
        }
        else if (getcolour() != isupper(board[x - i][y])) {
            movelist.push_back(Move(x, y, x - i, y));
            break;
        }
        else break;
    }
}

void Piece::possiblemoves(vector<Move>& movelist, const char(&board)[8][8], int dp) const{
    switch (gettype()) {
        case 'k':
            for (int i = -1; i < 2; ++i) {
                for (int j = -1; j < 2; ++j) {
                    if (x + i >= 0 && x + i < 8 && y + j >= 0 && y + j < 8 && (board[x + i][y + j] == ' ' || getcolour() != isupper(board[x + i][y + j]))) {
                        movelist.push_back(Move(x, y, x + i, y + j));
                    }
                }
            }
            break;
        case 'q':
            getdiagonalmoves(movelist, board);
            gethorizontalmoves(movelist, board);
            break;
        case 'b':
            getdiagonalmoves(movelist,board);
            break;
        case 'r':
            gethorizontalmoves(movelist, board);
            break;
        case 'n':

            if (x>0) {
                if (y - 2 >= 0 && (board[x - 1][y - 2] == ' ' || isupper(board[x - 1][y - 2]) != getcolour()))movelist.push_back(Move(x, y, x - 1, y -2));
                if (y +2 < 8 && (board[x - 1][y + 2] == ' ' || isupper(board[x - 1][y + 2]) != getcolour()))movelist.push_back(Move(x, y, x - 1, y + 2));
            }
            
            if (x > 1) {
                if (y - 1 >= 0 && (board[x - 2][y - 1] == ' ' || isupper(board[x - 2][y - 1]) != getcolour()))movelist.push_back(Move(x, y, x - 2, y - 1));
                if (y + 1 < 8 && (board[x - 2][y + 1] == ' ' || isupper(board[x - 2][y + 1]) != getcolour()))movelist.push_back(Move(x, y, x - 2, y + 1));
            }
            if (x < 7) {
                if (y - 2 >= 0 && (board[x + 1][y - 2] == ' ' || isupper(board[x + 1][y - 2]) != getcolour()))movelist.push_back(Move(x, y, x + 1, y - 2));
                if (y + 2 < 8 && (board[x + 1][y + 2] == ' ' || isupper(board[x + 1][y + 2]) != getcolour()))movelist.push_back(Move(x, y, x + 1, y + 2));
            }
            if (x < 6) {
                if (y - 1 >= 0 && (board[x + 2][y - 1] == ' ' || isupper(board[x + 2][y - 1]) != getcolour()))movelist.push_back(Move(x, y, x + 2, y - 1));
                if (y + 1 < 8 && (board[x + 2][y + 1] == ' ' || isupper(board[x + 2][y + 1]) != getcolour()))movelist.push_back(Move(x, y, x + 2, y + 1));
            }
            break;
        case 'p':
            if (y == 0 || y == 7) {
                cout <<"x,y" << x << y << board[x][y] << gettype() << "\n"; 
            }
            if(getcolour()){
                if (x == 0) {
                    if(board[x + 1][y + 1] !=' '&& getcolour() != isupper(board[x + 1][y + 1])) movelist.push_back(Move(x, y, x + 1, y + 1));
                }
                else if (x == 7) {
                    if (board[x - 1][y + 1] != ' ' && getcolour() != isupper(board[x - 1][y + 1])) movelist.push_back(Move(x, y, x - 1, y + 1));
                }
                else {
                 //   mypieceassert(x < 8 && y < 8, board,"188");
                    if (board[x + 1][y + 1] != ' ' && getcolour() != isupper(board[x + 1][y + 1])) {
                        movelist.push_back(Move(x, y, x + 1, y + 1)); 
                    }
                    if (board[x - 1][y + 1] != ' ' && getcolour() != isupper(board[x - 1][y + 1])) movelist.push_back(Move(x, y, x - 1, y + 1));
                }
                if (y == 1) {
                    if(board[x][y+1]==' ' && board[x ][y+2] == ' ') movelist.push_back(Move(x, y, x , y+2));
                }
                if (y < 7) {
                    if (board[x ][y+1] == ' ')  movelist.push_back(Move(x, y, x , y+1));
                }
                if (y == 4 && abs(x - dp) == 1 && board[dp][5]==' ') {
                    movelist.push_back(Move(x, 10*y, dp, y + 1));
                }
            }
            else {
                if (x == 0) {
                    if (board[x + 1][y - 1] != ' ' && getcolour() != isupper(board[x + 1][y - 1])) movelist.push_back(Move(x, y, x + 1, y - 1));
                }
                else if (x == 7) {
                    if (board[x - 1][y - 1] != ' ' && getcolour() != isupper(board[x - 1][y - 1])) movelist.push_back(Move(x, y, x - 1, y - 1));
                }
                else {
                //    mypieceassert(y > 0, board,to_string(y));
                    if (board[x + 1][y - 1] != ' ' && getcolour() != isupper(board[x + 1][y - 1])) movelist.push_back(Move(x, y, x + 1, y - 1));
                    if (board[x - 1][y - 1] != ' ' && getcolour() != isupper(board[x - 1][y - 1])) movelist.push_back(Move(x, y, x - 1, y - 1));
                }
                if (y == 6) {
                    if (board[x][y-1] == ' ' && board[x][y-2] == ' ') movelist.push_back(Move(x, y, x, y-2));
                }
                if (y >0) {
                    if (board[x][y-1] == ' ')  movelist.push_back(Move(x, y, x , y-1));
                }
                if (y == 3 && abs(x - dp) == 1 && board[dp][2]==' ') {
                    movelist.push_back(Move(x, 10*y, dp, y - 1));
                }
            }
            break;
    }
}

void Piece::mypieceassert(bool expr, const char(&board)[8][8],string message) const {
    if (!expr) {
        for (int i = 7; i >= 0; --i) {
            cout << i + 1 << " ";
            for (int j = 0; j < 8; ++j) {
                cout << '[' << board[j][i] << ']';
            }
            cout << '\n';
        }
        cout << "  ";
        for (int i = 0; i < 8; ++i) {
            char temp = 97 + i;
            cout << ' ' << temp << ' ';
        }
        cout << '\n';

        cerr << "mypieceassert called"<<message;
        abort();
    }
}
