#ifndef RANDOMBOT_H
#define RANDOMBOT_H
#include "Bot.h"
#include <random>

class RandomBot : public Bot {
    public:
        RandomBot();

        Move get_move(Board &board);

        int* search(Board board, int depth, Move last_move) {}

        int evaluate(Board board) {}
};

#endif