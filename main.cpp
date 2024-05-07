#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>
#include "core/Board.h"

using namespace std;
using namespace std::this_thread;
using namespace std::chrono_literals;

int get_length(array<Move, 255> moves) {
    for (int i=0; i<(int)(sizeof(moves) / sizeof(moves[0])); i++) {
        if (moves[i] == Move()) {
            return i;
        }
    }
    return 999999;
}

int main() {
    Board board(START_FEN, true);
    array<Move, 255> moves = board.get_pseudolegal_moves(Move(), board.board, board.turn);
    int n_moves = get_length(moves);
    
    printf("Amount of pseudolegal moves: %i\n", n_moves);

    for (int i=0; i<n_moves; i++) {
        board.move_piece(moves[i]);
        board.update_render();
        sleep_for(300ms);
        board.undo_move(moves[i]);
        board.update_render();
        sleep_for(50ms);
    }
    return 0;
}