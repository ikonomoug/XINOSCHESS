#ifndef PROTOCOL_DEF_H
#define PROTOCOL_DEF_H

enum action      : unsigned char {LOGIN, JOIN_GAME, MOVE, RESIGN, OFFER_DRAW, CANCEL_DRAW, SEND_MESSAGE};

enum reply       : unsigned char {ERROR, GAME_STATUS, GAME_DATA, LOGIN_REPLY};
enum game_data   : unsigned char {BOARD, OPPONENT_NAME, MESSAGE, SERVER_MESSAGE};
enum game_status : unsigned char {NOT_IN_GAME, IN_QUEUE, YOUR_TURN, OPPONENT_TURN, YOU_WIN, OPPONENT_WIN, DRAW};
enum login_reply : unsigned char {LOGGED_IN, BAD_LOGIN, DUPLICATE_SESSION};
#endif
