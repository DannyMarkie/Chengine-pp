#include <iostream>
#include <unordered_map>

#ifndef PIECES_H
#define PIECES_H

/**
 * Define all different values for all the pieces and initialize all the pieces using bitwise OR
*/
#define PAWN_VALUE 100
#define BISHOP_VALUE 330
#define KNIGHT_VALUE 320
#define ROOK_VALUE 500
#define QUEEN_VALUE 900
#define KING_VALUE 20000

#define PIECE_MASK 0b00111
#define COLOR_MASK 0b11000

#define WHITE 8
#define BLACK 16

#define EMPTY 0
#define PAWN 1
#define BISHOP 2
#define KNIGHT 3
#define ROOK 4
#define QUEEN 5
#define KING 6

#define WHITE_PAWN (WHITE | PAWN)
#define WHITE_BISHOP (WHITE | BISHOP)
#define WHITE_KNIGHT (WHITE | KNIGHT)
#define WHITE_ROOK (WHITE | ROOK)
#define WHITE_QUEEN (WHITE | QUEEN)
#define WHITE_KING (WHITE | KING)

#define BLACK_PAWN (BLACK | PAWN)
#define BLACK_BISHOP (BLACK | BISHOP)
#define BLACK_KNIGHT (BLACK | KNIGHT)
#define BLACK_ROOK (BLACK | ROOK)
#define BLACK_QUEEN (BLACK | QUEEN)
#define BLACK_KING (BLACK | KING)

/**
 * Gives the value of a piece.
 * 
 * Piece does not need to be masked as this is done in the function. Doing so anyway will result in an extra masking operation slowing down the code.
 * Code makes use of the std::unordered_map<int, int>.
 * 
 * @param piece A given piece of which a value is required.
 * 
 * @return Value of the piece
*/
int get_piece_value(int piece);

#endif