#pragma once

#include<vector>
#include<string>
#include "piece.h"
#include "move.h"
#include<cmath>
#include"utilities.h"
#include <cstdlib>
#include <ctime>
#include<assert.h>

using namespace std;

class Game {
public:
    /// @brief 
    /// @param position in fen notation
    /// @param a movenum
    /// @param b fullmovenum
    Game(string position = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq -", int a = 0, int b = 0);
    Game(const Game& other);
    string draw(bool white = true) const;
    bool check_check(int);//agnoeitai to i-osto kommati kata ton elegxo
    int lastmovedoublepush() const;//to xw xrhsimopoihsei? hahah
    bool simulatemove(const Move&);//true if allowed, paizei proswrina thn ypo syzhthsh kinhsh
    void updatepossiblemoves();//kalei thn possiblemoves gia ola ta kommatia opoiou exei seira kai diagrafei ta illegal
    void updateblackmoves(int);//to idio gia ta mavra akoma koai an mhn einai h seira tous
    void updatewhitemoves(int);//to idio gia ta aspa
    void updatecastlingbools();
    bool receivemove(string&);
    void change(const Move&);
    Piece getblackking() const;
    Piece getwhiteking() const;
    int getmovenum() const;
    void incrmovenum();

    Game* play(const Move&);    //used in MCTS
    int ended();   //returns outcome, used in MCTS
    Move choose_random_move() const; // used in MCTS

    void mygameassert(bool expr,string message)const;//used for debugging
public:
    char skakiera[8][8];
    vector<Piece> whitepieces;
    vector<Piece> blackpieces;
    vector<Move> currentpossiblemoves;
    vector<Move> currentblackmoves;
    vector<Move> currentwhitemoves;
    vector<Move> movelog;   
    int  K, Q, k, q;//castling rights(0->allowed,1->not in this round,2->lost forever utopia on top
    int movenum, fullmovenum, currentenpassant;
};

  
/*    

x kai y ta coords tou boart[8][8]  
skakiera:                          
                                 0                                   
    7_ | 8 [_][_][_][_][_][_][_][_] 0
    6_ | 7 [_][_][_][_][_][_][_][_]
    5_ | 6 [_][_][_][_][_][_][_][_]
    4_ | 5 [_][_][_][_][_][_][_][_]
    3_ | 4 [_][_][_][_][_][_][_][_]
    2_ | 3 [_][_][_][_][_][_][_][_]
    1_ | 2 [_][_][_][_][_][_][_][_]
    0_ | 1 [_][_][_][_][_][_][_][_]
       |    a  b  c  d  e  f  g  h 
    y/  ---------------------------
    /x     |0||1||2||3||4||5||6||7|
                            
 */
 //updatevlackmoves kai updatewhitemoves xrhsimopoiountai ston elegxo gia check 
 /*paizei o aspros px :
   -vazoume se ena vector ola ta pithana moves, kai ta illegal
   -gia kathe ena apo auta ta moves:
   -to kanoume simulate opou:
       -ekteloume updateblackmoves
       -elegxoume an opoiohdpote apo ta currentblackmoves apeilei ton vasilia
   -an nai diagrafoume to ypo syzhthsh move tou leukou apo th lista\
   -epanalamvanoume!
 */
