#include "NegamaxV1.h"

Move NegamaxV1::get_move(Board& board) {
    int isWhite = (board.turn == WHITE) ? 1 : -1;
    array<Move, 255> moves = board.get_pseudolegal_moves(Move(), board.board, board.turn);
    int* best_move_and_eval = this->search(board, 3, Move(), isWhite); // index 0 is the move index, index 1 is the eval.
    cout << best_move_and_eval[1] * isWhite << endl;
    return moves[best_move_and_eval[0]];
}

int* NegamaxV1::search(Board& board, int depth, Move last_move, int isWhite) {
    int* arr = new int[2];
    // If last depth, return the evaluation for the current position. 
    // If the player is black, reverse the evaluation score for negamax.
    if (depth == 0) {
        arr[0] = 0;
        arr[1] = (isWhite == 1) ? this->evaluate(board) : -this->evaluate(board);
        return arr;
    }
    int value = -2147483648;

    // Loop over all moves in this position.
    array<Move, 255> moves = board.get_pseudolegal_moves(last_move, board.board, board.turn);
    Move move;
    for (int i=0; i<255; i++) {
        move = moves[i];
        // If end of actual moves are reached, end loop.
        if (move.moved_piece == EMPTY) break;

        // If the move is not legal, skip it.
        if (!board.move_is_legal(move)) continue;

        // Make the move and continue the search at 1 depth deeper.
        board.move_piece(move);
        int* next = this->search(board, depth-1, move, -isWhite);
        board.undo_move(move);

        // Compare the evaluation for the current move with the best move found so far.
        // If current move is better, make it the best move.
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

    // Get the amount of legal moves for both white and black to get the mobility score. 
    // More legal moves means a higher mobility score, which means the position is better.
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

    // If its checkmate, return a near infinite value. (in this case the max for an int)
    if (board.is_checkmate()) {
        if (board.turn == WHITE) return -2147483648;
        else return 2147483648;
    }

    // Return 0 if it is stalemate.
    if (board.turn == WHITE && n_white_moves == 0) return 0;
    if (board.turn == BLACK && n_black_moves == 0) return 0;

    // Calculate mobility and scale the mobility score down a bit to make it weigh in less to the total score.
    // Mobility sometimes can cause weird moves which are just meant for mobility.
    int mobility = (int)((n_white_moves - n_black_moves) / 10);

    // Get all the values for each piece
    for (int i=0; i<64; i++) {
        int piece = board.board[i];
        int offset = ((piece & COLOR_MASK) == WHITE) ? 1 : -1;
        evaluation += (offset * get_piece_value(piece));
    }

    // Give a penalty for king mobility to give the bot a sense of king safety.
    int white_king_square;
    int black_king_square;
    for (int i=0; i<64; i++) {
        if (board.board[i] == (WHITE | KING)) {
            white_king_square = i;
        } else if (board.board[i] == (BLACK | KING)) {
            black_king_square = i;
        }
    }
    int black_king_mobility = board.square_mobility(black_king_square, BLACK);
    int white_king_mobility = board.square_mobility(white_king_square, WHITE);
    mobility -= (int)((white_king_mobility - black_king_mobility) / 6);

    evaluation += mobility;

    return evaluation;
}