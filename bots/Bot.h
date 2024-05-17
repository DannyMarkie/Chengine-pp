#ifndef BOT_H
#define BOT_H
#include "../core/Move.h"
#include "../core/Board.h"

class Bot {
    public:
        virtual Move get_move(Board& board) = 0;

        virtual int* search(Board& board, int depth, Move last_move, int isWhite) = 0;

        virtual int evaluate(Board& board) = 0;
};

#endif