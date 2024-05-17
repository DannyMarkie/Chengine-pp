#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>
#include "core/Board.h"
#include "core/Match.h"
#include "bots/RandomBot.h"

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
    Match match(new RandomBot(), new RandomBot());
    match.play();
    return 0;
}