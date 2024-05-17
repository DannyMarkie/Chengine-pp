#include "AlphaBetaPruningV3.h"

AlphaBetaPruningV3::AlphaBetaPruningV3() {
    this->total_time_ms = 60000;
    this->best_move_last_iteration = Move();
    this->best_move_index = 0;
}

Move AlphaBetaPruningV3::get_move(Board& board) {
    int isWhite = (board.turn == WHITE) ? 1 : -1;
    this->think_time_ms = this->get_think_time();
    array<Move, 255> moves = board.get_pseudolegal_moves(Move(), board.board, board.turn);
    int depth = 1;
    int* best_move_and_eval;
    this->search_start_time = std::chrono::high_resolution_clock::now();
    this->best_move_index = 0;
    this->best_move_last_iteration = Move();
    int alpha = -2147483647;
    int beta = 2147483647;
    // Think 1 depth deeper every iteration until total allowed think time has been reached.
    while (!this->think_timeout) {
        depth++;
        // cout << alpha << "\t" << beta << endl;
        // cout << "\n\n\n" << endl;
        // cout << depth << endl;
        int* current = this->search(board, depth, depth, Move(), isWhite, alpha, beta); // index 0 is the move index, index 1 is the eval.
        this->best_move_last_iteration = moves[current[0]];
        if (!this->think_timeout) { // For some reason the bot plays awfully when I keep the result of the unfinished iteration, this shouldnt happen.
            best_move_and_eval = current;
            this->best_move_index = best_move_and_eval[0];
        }
    }
    this->total_time_ms -= std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - this->search_start_time).count();
    this->think_timeout = false;
    cout << depth << endl;
    cout << best_move_and_eval[1] * isWhite << "\n" << endl;
    return moves[best_move_and_eval[0]];
}

int* AlphaBetaPruningV3::search(Board& board, int start_depth, int depth, Move last_move, int isWhite, int alpha, int beta) {
    // cout << alpha << "\t" << beta << endl;
    int* arr = new int[2];
    arr[0] = 255;
    arr[1] = -2147483647;
    // If last depth, return the evaluation for the current position. 
    // If the player is black, reverse the evaluation score for negamax.
    if (depth == 0) {
        arr[0] = 0;
        // arr[1] = this->evaluate(board);
        arr[1] = (isWhite == 1) ? this->evaluate(board) : -this->evaluate(board);
        return arr;
    }
    int value = -2147483647;

    array<Move, 255> moves = board.get_pseudolegal_moves(last_move, board.board, board.turn);
    Move move;
    // Search best move from previous iteration first.
    if (this->best_move_last_iteration.moved_piece != EMPTY && depth == start_depth) {
        move = this->best_move_last_iteration;
        // Make the move and continue the search at 1 depth deeper.
        board.move_piece(move);
        int* next = this->search(board, start_depth, depth-1, move, -isWhite, -beta, -alpha);
        next[1] = -next[1];
        board.undo_move(move);

        // Compare the evaluation for the current move with the best move found so far.
        // If current move is better, make it the best move.
        value = max(value, next[1]);
        alpha = max(alpha, value);
        if (value == next[1]) {
            arr[0] = this->best_move_index;
            arr[1] = next[1];
        }
    }

    // Loop over all moves in this position.
    for (int i=0; i<255; i++) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - this->search_start_time).count() > this->think_time_ms) {
            this->think_timeout = true;
            break;
        }
        move = moves[i];
        // If end of actual moves are reached, end loop.
        if (move.moved_piece == EMPTY) break;

        // If the move is not legal, skip it.
        if (!board.move_is_legal(move)) continue;

        // Make the move and continue the search at 1 depth deeper.
        board.move_piece(move);
        int* next = this->search(board, start_depth, depth-1, move, -isWhite, -beta, -alpha);
        next[1] = -next[1];
        board.undo_move(move);

        // Compare the evaluation for the current move with the best move found so far.
        // If current move is better, make it the best move.
        value = max(value, next[1]);
        alpha = max(alpha, value);
        if (value == next[1]) {
            arr[0] = i;
            arr[1] = next[1];
        }
        if (alpha > beta) break;
    }
    return arr;
}

int AlphaBetaPruningV3::evaluate(Board& board) {
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
        if (board.turn == WHITE) return -2147483647;
        else return 2147483647;
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

int AlphaBetaPruningV3::get_think_time() {
    return (this->total_time_ms / 30);
}