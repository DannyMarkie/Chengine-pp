#include "RandomBot.h"

RandomBot::RandomBot() {}

Move RandomBot::get_move(Board &board) {
    array<Move, 255> moves = board.get_pseudolegal_moves(Move(), board.board, board.turn);
    int n_moves = 0;
    for (Move move : moves) {
        if (move.moved_piece != EMPTY) n_moves++;
        else break;
    }
    int move_index = rand() % n_moves;
    Move move = moves[move_index];
    while (!board.move_is_legal(move) && move.moved_piece != EMPTY) {
        move_index = rand() % n_moves;
        move = moves[move_index];
    }
    return move;
}