#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>
#include "core/Board.h"
#include "core/Match.h"
#include "bots/RandomBot.h"
#include "bots/NegamaxV1.h"
#include "bots/IterativeDeepeningV2.h"
#include "bots/AlphaBetaPruningV3.h"

int main() {
    Match match(new AlphaBetaPruningV3(), new IterativeDeepeningV2());
    match.play();
    return 0;
}