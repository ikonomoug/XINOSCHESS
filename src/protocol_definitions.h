#ifndef PROTOCOL_DEF_H
#define PROTOCOL_DEF_H

enum action : unsigned char {LOGIN, JOIN_GAME, MOVE, MESSAGE, ABORT_GAME};
enum reply : unsigned char { WAITING_FOR_OPPONENT, BOARD, ERROR, BAD_MOVE, YOUR_TURN, OPPONENT_TURN, LOGGED_IN, OPPONENT_NAME, NEW_MESSAGE, BAD_LOGIN, DUPLICATE_SESSION};

#endif

// TODO: IMPLEMENT
// enum action      : unsigned char {LOGIN, JOIN_GAME, GAME_ACTION};
// enum game_action : unsigned char {MOVE, RESIGN, OFFER_DRAW, CANCEL_DRAW, SEND_MESSAGE};
// enum reply       : unsigned char {ERROR, GAME_STATUS, GAME_DATA};
// enum game_data   : unsigned char {BOARD, OPPONENT_NAME, MESSAGE, SERVER_MESSAGE};
// enum game_status : unsigned char {NOT_IN_GAME, IN_QUEUE, YOUR_TURN, OPPONENT_TURN, YOU_WIN, OPPONENT_WIN, DRAW};
// enum login_reply : unsigned char {LOGGED_IN, BAD_LOGIN, DUPLICATE_SESSION};