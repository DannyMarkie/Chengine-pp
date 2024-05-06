#include "Move.h"

Move::Move(int start_square, int end_square, int moved_piece, int captured_piece, int flag) 
: start_square(start_square), end_square(end_square), moved_piece(moved_piece), captured_piece(captured_piece), flag(flag) 
{
    return;
}

Move::Move()
: start_square(0), end_square(0), moved_piece(0), captured_piece(0), flag(0)
{
    return;
}

bool Move::operator== (const Move &m) {
    bool has_same_starting_position = (this->start_square == m.start_square);
    bool has_same_end_position = (this->end_square == m.end_square);
    bool captured_same_piece = (this->captured_piece == m.captured_piece);
    bool has_same_flag = (this->flag == m.flag);
    return (has_same_starting_position && has_same_end_position && captured_same_piece && has_same_flag);
}