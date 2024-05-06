#include "Pieces.h"

/**
 * Hashmap of all the piece values to avoid if else statements in get_piece_value.
*/
std::unordered_map<int, int> piece_values = {
    {EMPTY, 0},
    {PAWN, PAWN_VALUE},
    {BISHOP, BISHOP_VALUE},
    {KNIGHT, KNIGHT_VALUE},
    {ROOK, ROOK_VALUE},
    {QUEEN, QUEEN_VALUE},
    {KING, KING_VALUE}
};

int get_piece_value(int piece) {
    return piece_values.at((piece & PIECE_MASK));
}