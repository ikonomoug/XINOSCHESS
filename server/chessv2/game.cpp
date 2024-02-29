#include "game.h"
#include<algorithm>
#include <sstream>


Game::Game(string position , int a, int b) {
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            skakiera[j][i] = ' ';
        }
    }
    int rows = 7, columns = 0, temp = 0;
    while (position[temp] != ' ') {
        if (util::isnumber(position[temp])) {
            columns += util::numberise(position[temp]);
        }
        else if (position[temp] == '/') {
            --rows;
            columns = 0;
        }
        else{
            if (isupper(position[temp])) whitepieces.push_back(Piece(position[temp], columns , rows ));
            else blackpieces.push_back(Piece(position[temp], columns, rows));
        skakiera[columns][rows] = position[temp];
        columns++;
        }
        ++temp;
    }
    temp += 3;
    K = 2;
    k = 2;
    Q = 2;
    q = 2;
    while (position[temp] != ' ') {
        switch (position[temp]) {
        case 'Q':
            Q = true;
            break;
        case 'K':
            K = true;
            break;
        case 'q':
            q = true;
            break;
        case 'k':
            k = true;
            break;
        case '-':
            break;
        }
        ++temp;
    }
    ++temp;
    if (position[temp] == '-') {
        ++temp;
        currentenpassant = 0;
    }
    else {
        currentenpassant = 10 * (position[temp] + util::numberise(position[temp + 1]));
        ++temp;
    }
    movenum = a;
    fullmovenum = b;
    movelog.push_back(Move(-1, -1, -1, -1));
}

Game::Game(const Game& other) : movenum(other.movenum), fullmovenum(other.fullmovenum), currentenpassant(other.currentenpassant) {
    // Copy the skakiera (chessboard)
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            skakiera[i][j] = other.skakiera[i][j];
        }
    }

    // Copy the vectors
    whitepieces = other.whitepieces;
    blackpieces = other.blackpieces;
    currentpossiblemoves = other.currentpossiblemoves;
    currentblackmoves = other.currentblackmoves;
    currentwhitemoves = other.currentwhitemoves;
    movelog = other.movelog;

    // Copy the castling rights
    K = other.K;
    Q = other.Q;
    k = other.k;
    q = other.q;
}


string Game::draw(bool white) const {
    stringstream output;
    
    for (int i = 7; i >=0; --i) {
        if(white)
            output << i+1 << " ";
        else
            output << 8-i << " ";

         for (int j = 0; j < 8; ++j) {
            if(white)
                output << '[' << skakiera[j][i] << ']';
            else
                output << '[' << skakiera[7-j][7-i] << ']';
        }
        output << '\n';
    }
    output << "  ";
    char temp;
    for (int i = 0; i < 8; ++i) {
        if(white)
            temp = 97 + i;
        else
            temp = 97 + 7-i;
        output << ' ' << temp << ' ';
    }
    output << '\n';
    
    return output.str();
}

int Game::lastmovedoublepush() const {
    int a = movelog[getmovenum()].x1;
    int b = movelog[getmovenum()].y1;
    int c = movelog[getmovenum()].x2;
    int d = movelog[getmovenum()].y2;
    if (a == c && abs(d - b) == 2) {
        if (getmovenum()%2 != 0) {
            if (skakiera[c][d] == 'P') return c;
        }
        else if (skakiera[c][d] == 'p') return c;
    }
    return -2;
}

bool Game::check_check(int temp) {
    if (getmovenum() % 2 == 0) {
        updateblackmoves(temp);
        for (auto& element : currentblackmoves) {
            if (skakiera[element.x2][element.y2] == 'K') return true;
        }
    }
    else {
        updatewhitemoves(temp);
        for (auto& element : currentwhitemoves) {
            if (skakiera[element.x2][element.y2] == 'k') return true;
        }
    }
    return false;
}

bool Game::simulatemove(const Move &move) {
    bool allowed;
    char temp1 = skakiera[move.x1][move.y1];
    char temp2 = skakiera[move.x2][move.y2];
    skakiera[move.x2][move.y2] = temp1;
    skakiera[move.x1][move.y1] = ' ';
    int counter = -1;

    if (getmovenum() % 2 == 0) {
        for (auto& element : whitepieces) {
            if (element.x == move.x1 && element.y == move.y1) {
                element.x = move.x2;
                element.y = move.y2;
            }
        }
        if (temp2 != ' ') {
            for (auto& element : blackpieces) {
                ++counter;
                if (element.x == move.x2 && element.y == move.y2) break;
            }
        }
    }
    else {
        for (auto& element : blackpieces) {
            if (element.x == move.x1 && element.y == move.y1) {
                element.x = move.x2;
                element.y = move.y2;
            }
        }
        if (temp2 != ' ') {
            for (auto& element : whitepieces) {
                ++counter;
                if (element.x == move.x2 && element.y == move.y2) break;
            }
        }
    }
    if (check_check(counter)) { 
        allowed = false; 
    }
    else { 
        allowed = true; 
    }

    skakiera[move.x1][move.y1] = temp1;
    skakiera[move.x2][move.y2] = temp2;

    if (getmovenum() % 2 == 0) {
        for (auto& element : whitepieces) {
            if (element.x == move.x2 && element.y == move.y2 && element.value==temp1) {
                element.x = move.x1;
                element.y = move.y1;
            }
        }
    }
    else {
        for (auto& element : blackpieces) {
            if (element.x == move.x2 && element.y == move.y2 && element.value == temp1) {
                element.x = move.x1;
                element.y = move.y1;
            }
        }
    }
    return allowed;
}

void Game::updatepossiblemoves() {
    currentpossiblemoves.clear();
    if (movenum % 2 == 0) { //check moves for white
        if (K==0) currentpossiblemoves.push_back(Move("O-O"));//king side castling
        if (Q==0) currentpossiblemoves.push_back(Move("O--O"));//queen side castling
        for (int i = 0; i < whitepieces.size(); ++i) {
          //  cout<<"value: "<< whitepieces[i].value << " x: " << whitepieces[i].x << " y: " << whitepieces[i].y << " ingame: " << whitepieces[i].ingame << "\n";
            if (whitepieces[i].ingame)whitepieces[i].possiblemoves(currentpossiblemoves, skakiera, currentenpassant);
        }
    }
    else {
        if (k==0) currentpossiblemoves.push_back(Move("o-o"));
        if (q==0) currentpossiblemoves.push_back(Move("o--o"));
        for (int i = 0; i < blackpieces.size(); ++i) {
            //cout << "value: " << blackpieces[i].value << " x: " << blackpieces[i].x << " y: " << blackpieces[i].y <<" ingame: "<<blackpieces[i].ingame << "\n";
            if (blackpieces[i].ingame)blackpieces[i].possiblemoves(currentpossiblemoves, skakiera, currentenpassant);
        }
    }
    
    for (int i = 0; i < currentpossiblemoves.size(); ++i) {
        if (currentpossiblemoves[i].coords[0] != 'o' && currentpossiblemoves[i].coords[0] != 'O'){
            if (!simulatemove(currentpossiblemoves[i])) currentpossiblemoves[i].allowed = false;
        }
    }
      
}

void Game::updateblackmoves(int a) {
    currentblackmoves.clear();
    for (int i = 0; i < blackpieces.size(); ++i) {
        if (blackpieces[i].ingame && i!=a)blackpieces[i].possiblemoves(currentblackmoves, skakiera, currentenpassant);
    }
}

void Game::updatewhitemoves(int a) {
    currentwhitemoves.clear();
    for (int i = 0; i < whitepieces.size(); ++i) {
        if(whitepieces[i].ingame && i != a)whitepieces[i].possiblemoves(currentwhitemoves, skakiera, currentenpassant);
    }
}

void Game::updatecastlingbools() {
    if (getmovenum() % 2 == 0) {
        if (Q == 1) Q = 0;
        if (K == 1) K = 0;
        if (check_check(-1)) {
            if (K != 2)K = 1;
            if (Q != 2) Q = 1;
            return;
        }
        Move s(4, 0, 5, 0);
        Move d(4, 0, 6, 0);
        if (skakiera[5][0] != ' ' || skakiera[6][0] != ' ') {
            if (K != 2)K = 1;
        }
        else if (!simulatemove(s) || !simulatemove(d)) {
            if (K != 2)K = 1;
        }
        s = Move(4, 0, 3, 0);
        d =  Move(4, 0, 2, 0);
        if (skakiera[3][0] != ' ' || skakiera[2][0] != ' ' || skakiera[1][0] != ' ' ) {
            if (Q!= 2) Q= 1;
        }
        else if (!simulatemove(s) || !simulatemove(d)) {
            if (Q != 2)Q = 1;
        }
    }
    else {
        if (q == 1) q = 0;
        if (k == 1) k = 0;
        if (check_check(-1)) {
            if (k != 2)k = 1;
            if (q != 2) q = 1;
            return;
        }
        Move s(4, 7, 5, 7);
        Move d(4, 7, 6, 7);
        if (skakiera[5][7] != ' ' || skakiera[6][7] != ' ') {
            if (k != 2)k = 1;
        }
        else if (!simulatemove(s) || !simulatemove(d)) {
            if (k != 2)k = 1;
        }
        s = Move(4, 7, 3, 7);
        d = Move(4, 7, 2, 7);
        if (skakiera[3][7] != ' ' || skakiera[2][7] != ' ' || skakiera[1][7] != ' ') {
            if (q != 2) q = 1;
        }
        else if (!simulatemove(s) || !simulatemove(d)) {
            if (q != 2) q = 1;
        }
    }
}

bool Game::receivemove(string& move) {
    bool flag=false;
    if (move == "end") {
        return true;
    }
    else{
        for (auto& element : currentpossiblemoves) {
            if (element.coords == move && element.allowed == true) {
                change(element);
                flag = true;
                return true;
                break;
            }
        }
        if (!flag) {
            return false;
        }
    }

}

void Game::change(const Move &move) {
    //TAKING AWAY CATSLING RIGHTS OF WHITE
    if (move.x1 == 0 && move.y1 == 0 || move.x2 == 0 && move.y2 == 0) Q = 2;
    if (move.x1 == 7 && move.y1 == 0 || move.x2 == 7 && move.y2 == 0) K = 2;
    if (move.x1 == 4 && move.y1 == 0) Q = K = 2; 
    //TAKING AWAY CATSLING RIGHTS OF BLACK
    if (move.x1 == 0 && move.y1 == 7 || move.x2 == 0 && move.y2 == 7) q = 2;
    if (move.x1 == 7 && move.y1 == 7 || move.x2 == 7 && move.y2 == 7) k = 2;
    if (move.x1 == 4 && move.y1 == 7) q = k = 2;

    char temp = skakiera[move.x2][move.y2];
    skakiera[move.x2][move.y2] = skakiera[move.x1][move.y1];
    skakiera[move.x1][move.y1] = ' ';

    if (getmovenum() % 2 == 0) {
        for (auto& element : whitepieces) {
            if (element.ingame && element.x == move.x1 && element.y == move.y1) {
                element.x = move.x2;
                element.y = move.y2;
                //special pawn cases (promotion en passant)
                if (element.value == 'P'){
                    if (move.y2 == 7) {
                       // cout << "Promote to: ";
                        char t = 'Q';    ////////////////////////////////
                        element.promote(t);
                       // cout << element.value;
                        skakiera[move.x2][move.y2] = element.value;
                    }
                    else if (move.coords[1] == 'x') {
                        for (auto& element : blackpieces) {
                            if (element.x == currentenpassant && element.y == 4) {
                                skakiera[element.x][element.y] = ' ';
                                element.ingame = false;///////////////////////////
                                element.value = 'x';
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (temp != ' ') {
           for (auto& element : blackpieces) {
               if (element.ingame && element.x == move.x2 && element.y == move.y2) {
                    //blackpieces.erase(blackpieces.begin()+counter);
                    element.ingame = false;
                    element.value = 'x';
                    break;
               }
           }
        }
    }
    else {
        for (auto& element : blackpieces) {
            if (element.ingame && element.x == move.x1 && element.y == move.y1) {
                element.x = move.x2;
                element.y = move.y2;
                if (element.value == 'p') {
                    if (move.y2 == 0) {
                       // cout << "Promote to: ";
                        char t = 'q';
                        element.promote(t);
                        skakiera[move.x2][move.y2] = element.value;
                    }
                    else if (move.coords[1] == 'x') {
                        for (auto& element : whitepieces) {
                            if (element.x == currentenpassant && element.y == 3) {
                                element.ingame = false;
                                skakiera[element.x][element.y] = ' ';
                                element.value = 'x';
                                break;
                            }
                        }
                    }
                }
            }
        }
        if (temp != ' ') {
            for (auto& element : whitepieces) {
                if (element.ingame && element.x == move.x2 && element.y == move.y2) {
                    element.ingame=false;
                    element.value = 'x';
                    break;
                }
            }

        }
    }
    //rook movement during castling:
    Move extra = Move(-2, -1, -1, -1);
    if (move.coords == "O-O") {
        extra = Move(7, 0, 5, 0);
    }
    else if (move.coords == "O--O") {
        extra = Move(0, 0, 3, 0);
    }
    else if (move.coords == "o-o") {
        extra = Move(7, 7, 5, 7);
    }
    else if (move.coords == "o--o") {
        extra = Move(0, 7, 3, 7);
    }
    if (extra.x1 != -2) {
        change(extra);
    }

    movelog.push_back(move);
}

Piece Game::getwhiteking() const{
    for (auto& element : whitepieces) {
        if (element.gettype() == 'k') return element;
    }
}

Piece Game::getblackking() const{
    for (auto& element : blackpieces) {
        if (element.gettype() == 'k') return element;
    }
}

int Game::getmovenum() const{ 
    return movenum; 
}

void Game::incrmovenum() {
    movenum++; 
}



Game* Game::play(const Move& m) {
    mygameassert(m.x2 < 8 && m.y2 < 8, m.coords);

    Game *temp =new Game(*this);

    temp->change(m);

    temp->incrmovenum();
    temp->currentenpassant = lastmovedoublepush();
    temp->updatecastlingbools();
    temp->updatepossiblemoves();
    return temp;
}

int Game::ended() {
    if (movelog.size() > 100) return 0;
    bool checkmate = true;
    for (auto& element : currentpossiblemoves) {
        if (element.allowed == true)checkmate = false;
    }
    if (checkmate == true) {
        if (!check_check(-1)) {
            return 0;
        }
        else {
            return pow(-1,movenum+2);
        }
    }
    return 2;
}

Move Game::choose_random_move() const{
    vector<Move> temp;
    for (int i = 0; i < currentpossiblemoves.size(); ++i) if (currentpossiblemoves[i].allowed)temp.push_back(currentpossiblemoves[i]);
    if (temp.size() == 0) return Move("pain");
    return temp[rand() % temp.size()];
}

void Game::mygameassert(bool expr,string message) const{
    if (!expr) {
        draw();
        cout << '\n';
        for (int i = 0; i < movelog.size(); ++i) {
            cout << movelog[i].coords << ' '  << movelog[i].allowed;
        }
        cerr << "mygameassert called "<<message;
        abort();
    }
}
