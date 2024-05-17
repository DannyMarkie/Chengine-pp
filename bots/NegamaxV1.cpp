#include "NegamaxV1.h"

NegamaxV1::NegamaxV1() {}

Move NegamaxV1::get_move(Board& board) {
    int isWhite = (board.turn == WHITE) ? 1 : -1;
    array<Move, 255> moves = board.get_pseudolegal_moves(Move(), board.board, board.turn);
    int* best_move_and_eval = this->search(board, 3, Move(), isWhite);
    cout << best_move_and_eval[1] * isWhite << endl;
    return moves[best_move_and_eval[0]];
}

int* NegamaxV1::search(Board& board, int depth, Move last_move, int isWhite) {
    int* arr = new int[2];
    if (depth == 0) {
        arr[0] = 0;
        arr[1] = (isWhite == 1) ? this->evaluate(board) : -this->evaluate(board);
        return arr;
    }
    int value = -2147483648;
    array<Move, 255> moves = board.get_pseudolegal_moves(last_move, board.board, board.turn);
    Move move;
    for (int i=0; i<255; i++) {
        move = moves[i];
        if (move.moved_piece == EMPTY) break;
        if (!board.move_is_legal(move)) continue;
        board.move_piece(move);
        int* next = this->search(board, depth-1, move, -isWhite);
        board.undo_move(move);
        value = max(value, -next[1]);
        if (value == -next[1]) {
            arr[0] = i;
            arr[1] = -next[1];
        }
    }
    return arr;
}

int NegamaxV1::evaluate(Board& board) {
    int evaluation = 0;
    array<Move, 255> white_moves = board.get_pseudolegal_moves(Move(), board.board, WHITE);
    array<Move, 255> black_moves = board.get_pseudolegal_moves(Move(), board.board, BLACK);

    int n_white_moves = 0;
    for (int i=0; i<255; i++) {
        if (white_moves[i].moved_piece == EMPTY) break;
        if (board.move_is_legal(white_moves[i])) n_white_moves++;
    }

    int n_black_moves = 0;
    for (int i=0; i<255; i++) {
        if (black_moves[i].moved_piece == EMPTY) break;
        if (board.move_is_legal(black_moves[i])) n_black_moves++;
    }

    if (board.is_checkmate()) {
        if (board.turn == WHITE) return -2147483648;
        else return 2147483648;
    }
    if (board.turn == WHITE && n_white_moves == 0) return 0;
    if (board.turn == BLACK && n_black_moves == 0) return 0;

    int mobility = (int)((n_white_moves - n_black_moves) / 10);

    for (int i=0; i<64; i++) {
        int piece = board.board[i];
        int offset = ((piece & COLOR_MASK) == WHITE) ? 1 : -1;
        evaluation += (offset * get_piece_value(piece));
    }

    evaluation += mobility;

    return evaluation;
}