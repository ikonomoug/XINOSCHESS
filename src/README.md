# Communication Protocol

The server works with a protocol over TCP, based on packets exchanged by the server and the client, defined in "./protocol_definitions.h".

The server sents **reply** packets and receives **action** packets.
The client sents **action** packets and receives **reply** packets.
 The first byte is always the packet length and the second byte defines the type of action/reply.
 # Action Packets
byte no./action|0 [packet length]|1 [action]|2-255 [additional data]
|--            |--               |--        |--                    |
|LOGIN         |1-16             | 0        |username              |
|JOIN_GAME|1   | 1 |null    |
|MOVE|1| 2 |null|
|RESIGN|1| 3 |null|
|OFFER_DRAW|1| 4 |null|
|CANCEL_DRAW|1| 5 |null|
|SEND_MESSAGE|2 - 254| 6 |message|

 # Reply Packets
byte no./reply|0 [packet length]|1 [reply]|2-255 [additional data]
|--            |--               |--        |--                    |
|ERROR         |1          | 0        |null              |

 - Sent when client does something illegal

byte no./reply|0 [packet length]|1 [reply]|2 [game status]| 3-255 [additional data]
|--            |--               |--        |--                  |--|
|GAME_STATUS         |2          | 1        |0-6              |null

 - Contains info about the game status: NOT_IN_GAME, IN_QUEUE, YOUR_TURN, OPPONENT_TURN, YOU_WIN, OPPONENT_WIN, DRAW.

byte no./reply|0 [packet length]|1 [reply]|2 [game DATA]| 3-255 [additional data]
|--            |--               |--        |--                  |--|
|GAME_DATA         |2-255          | 3        |0-3              |...

 - BOARD, OPPONENT_NAME, MESSAGE, SERVER_MESSAGE

byte no./reply|0 [packet length]|1 [reply]|2 [game status]| 3-255 [additional data]
|--            |--               |--        |--                  |--|
|LOGIN_REPLY         |2          | 1        |0-2              |null

 - Login response: LOGGED_IN, BAD_LOGIN, DUPLICATE_SESSION


