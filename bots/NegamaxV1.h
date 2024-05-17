#ifndef NEGAMAX_V1
#define NEGAMAX_V1
#include "Bot.h"
#include "../core/Pieces.h"

class NegamaxV1 : public Bot {
    public:
        NegamaxV1();

        Move get_move(Board &board);

        int* search(Board& board, int depth, Move last_move, int isWhite);

        int evaluate(Board& board);
};

#endif