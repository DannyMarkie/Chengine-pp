#include <iostream>
#include <stdio.h>
#include <chrono>
#include <thread>
#include "core/Board.h"
#include "core/Match.h"
#include "bots/RandomBot.h"
#include "bots/NegamaxV1.h"

int main() {
    Match match(new NegamaxV1(), new NegamaxV1());
    match.play();
    return 0;
}