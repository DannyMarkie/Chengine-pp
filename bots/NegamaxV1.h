#ifndef NEGAMAX_V1
#define NEGAMAX_V1
#include "Bot.h"
#include "../core/Pieces.h"

class NegamaxV1 : public Bot {
    public:
        NegamaxV1() {}

        /**
         * Uses the search function in order to get the best move at a depth of 3.
         * 
         * This version uses no iterative deepening or alpha beta pruning.
         * 
         * @return Move the best move found.
        */
        Move get_move(Board &board);

        /**
         * Uses negamax to search for the best move at a certain depth.
         * 
         * @return Integer array with 2 values, index 0 has the index of the best move and index 1 has the evaluation for the current position.
        */
        int* search(Board& board, int depth, Move last_move, int isWhite);

        /**
         * Calculates the evaluation for the current position based on how many pieces each player has and how much mobility.
         * 
         * @return Integer evaluation, would need to be divided by 1000 to get proper eval but it's kept this high to avoid floating point operations.
        */
        int evaluate(Board& board);
};

#endif