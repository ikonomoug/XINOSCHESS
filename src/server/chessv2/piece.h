#pragma once

#include "move.h"
#include<vector>
#include<string>
using namespace std;

class Piece {
public:
    Piece(char a, int c, int d);
    void movepiece(const Move& move);
    void promote(char newtype);//promotion of pawns
    int getcolour() const;
    char gettype() const;
    void  getdiagonalmoves(vector<Move>& , const char (&board)[8][8]) const;//pushes back all the diagonal moves(bishop style) that a piece can execute before finding friendly piece,border, or eating an enemys piece
    void  gethorizontalmoves(vector<Move>& , const char(&board)[8][8]) const;//same as diagonal except rook style
    void  possiblemoves(vector<Move>&,  const char (&board)[8][8],int) const;//all moves current piece can do disregarding illegalities
    void mypieceassert(bool expr, const char(&board)[8][8],string message) const;
public:
    char value;//K for white king, n for black knight etc
    int  x, y;//0 to 7
    bool ingame;//is the piece still on the board?
};
