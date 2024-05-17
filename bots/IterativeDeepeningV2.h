#ifndef ITERATIVEDEEPENINGV2_H
#define ITERATIVEDEEPENINGV2_H
#include <chrono>
#include "Bot.h"
#include "../core/Pieces.h"

class IterativeDeepeningV2 : public Bot {
    private:
        std::chrono::high_resolution_clock::time_point search_start_time;
        int think_time_ms;
        bool think_timeout;
        int total_time_ms;
        Move best_move_last_iteration;
        int best_move_index;

    public:
        IterativeDeepeningV2();

        /**
         * Uses the search function in order to get the best move at a depth of 3.
         * 
         * This version uses no alpha beta pruning.
         * 
         * @return Move the best move found.
        */
        Move get_move(Board &board);

        /**
         * Uses negamax to search for the best move at a certain depth.
         * 
         * @return Integer array with 2 values, index 0 has the index of the best move and index 1 has the evaluation for the current position.
        */
        int* search(Board& board, int start_depth, int depth, Move last_move, int isWhite);

        /**
         * Calculates the evaluation for the current position based on how many pieces each player has and how much mobility.
         * 
         * @return Integer evaluation, would need to be divided by 1000 to get proper eval but it's kept this high to avoid floating point operations.
        */
        int evaluate(Board& board);

        int get_think_time();
};

#endif