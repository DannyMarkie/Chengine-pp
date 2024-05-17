#ifndef MATCH_H
#define MATCH_H
#include "../bots/Bot.h"
#include "Board.h"
#include <thread>
#include <chrono>

using namespace std;
using namespace std::this_thread;
using namespace std::chrono_literals;

class Match {
    public:
        Match(Bot* bot1, Bot* bot2);

        int play();

    private:
        Bot* bot1;
        Bot* bot2;
        Board board = Board(START_FEN, true);
        bool render;
        int winner;
        int moves_since_pawn_push = 0;
        int moves_since_capture = 0;
};

#endif