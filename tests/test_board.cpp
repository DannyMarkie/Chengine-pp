#include "../core/board.h"
#include <chrono>
#include <thread>

using namespace std::chrono_literals;
using namespace std::chrono;
using namespace std::this_thread;

unordered_map<int, string> file_chars = {
    {0, "a"},
    {1, "b"},
    {2, "c"},
    {3, "d"},
    {4, "e"},
    {5, "f"},
    {6, "g"},
    {7, "h"}
};

int generate_move_sequences(int depth, Board &board, Move last_move) {
    int n_moves = 0;
    if (depth == 1) {
        array<Move, 255> moves = board.get_pseudolegal_moves(last_move, board.get_board(), board.turn);
        for (Move move : moves) {
            if (move.moved_piece == EMPTY) break;
            if (!board.move_is_legal(move)) continue;

            if (board.render) {
                board.move_piece(move);
                board.update_render();
                sleep_for(100ms);
                board.undo_move(move);
                sf::Event event;
                while (board.window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed)
                        board.window.close();
                }
            }
            n_moves++;
        }
        return n_moves;
    }

    array<Move, 255> moves = board.get_pseudolegal_moves(last_move, board.get_board(), board.turn);
    for (Move move : moves) {
        if (move.moved_piece == EMPTY) break;
        if (!board.move_is_legal(move)) continue;

        if (board.render) {
            board.move_piece(move);
            board.update_render();
            sleep_for(100ms);
            board.undo_move(move);
            sf::Event event;
            while (board.window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    board.window.close();
            }
        }
        if (!board.is_checkmate()) {
            board.move_piece(move);
            n_moves += generate_move_sequences(depth-1, board, move);
            board.undo_move(move);
        }
    }
    return n_moves;
}

int perft(int depth, string fen_string, bool render, bool verbose) {
    int n_moves = 0;
    depth--;
    array<int, 255> total;
    Board board(fen_string, render);
    array<Move, 255> moves = board.get_pseudolegal_moves(Move(), board.get_board(), board.turn);
    for (int i=0; i<(int)(sizeof(moves) / sizeof(moves[0])); i++) {
        Move move = moves[i];
        if (move.moved_piece == EMPTY) break;
        if (!board.move_is_legal(move)) {
            continue;
        }
        if (depth == 0) n_moves++;
        board.move_piece(move);
        if (render) {
            board.update_render();
            sleep_for(100ms);
            sf::Event event;
            while (board.window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    board.window.close();
            }
        }
        int n_continuations = 0;
        if (depth > 0) {
            n_continuations = generate_move_sequences(depth, board, move);
            n_moves += n_continuations;
        }
        if (verbose) {
            int start_file = move.start_square % 8;
            int start_rank = (int)(move.start_square / 8);
            int end_file = move.end_square % 8;
            int end_rank = (int)(move.end_square / 8);
            string start_file_char = file_chars.at(start_file);
            string start_rank_char = to_string(8 - start_rank);
            string end_file_char = file_chars.at(end_file);
            string end_rank_char = to_string(8 - end_rank);
            string move_string = start_file_char + start_rank_char + end_file_char + end_rank_char;
            cout << move_string << " (" << move.start_square << "," << move.end_square << ")" << ": " << n_continuations << endl;
            // printf("%s: %i\n", move_string, n_continuations);
        }
        total[i] = n_continuations;
        board.undo_move(move);
    }
    if (verbose) {
        int len;
        for (int i=0; i<255; i++) {
            if (total[i] == 0) {
                len = i;
            }
        }
        printf("\nNodes searched: %i", len);
    }
    return n_moves;
}

void test_move_generation_pos1(bool render, int depth) {
    string fen_string = START_FEN;
    bool failed = false;
    string status = "Passed";
    long long int legal_moves_ply[] = {20, 400, 8'902, 197'281, 4'865'609, 119'060'324, 3'195'901'860, 84'998'978'956};
    for (int ply=1; ply<=depth; ply++) {
        if (failed) break;
        auto start_time = high_resolution_clock::now();
        int n_moves = perft(ply, fen_string, render, false);
        auto stop_time = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop_time - start_time);
        if (n_moves != legal_moves_ply[ply-1]) {
            failed = true;
            status = "Failed";
        }
        printf("Position: 1\t Depth: %i ply\tLegal moves generated: %i \t\tExpected output: %lli\t\tTime: %llims\t\t", ply, n_moves, legal_moves_ply[ply-1], duration.count());
        cout << status << endl;
    }
}

void test_move_generation_pos2(bool render, int depth) {
    string fen_string = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -";
    bool failed = false;
    string status = "Passed";
    long long int legal_moves_ply[] = {48, 2'039, 97'862, 4'085'603, 193'690'690, 8'031'647'685};
    for (int ply=1; ply<=depth; ply++) {
        if (failed) break;
        auto start_time = high_resolution_clock::now();
        int n_moves = perft(ply, fen_string, render, false);
        auto stop_time = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop_time - start_time);
        if (n_moves != legal_moves_ply[ply-1]) {
            failed = true;
            status = "Failed";
        }
        printf("Position: 2\t Depth: %i ply\tLegal moves generated: %i \t\tExpected output: %lli\t\tTime: %llims\t\t", ply, n_moves, legal_moves_ply[ply-1], duration.count());
        cout << status << endl;
    }
}

void test_move_generation_pos3(bool render, int depth) {
    string fen_string = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -";
    bool failed = false;
    string status = "Passed";
    long long int legal_moves_ply[] = {14, 191, 2'812, 43'238, 674'624, 11'030'083, 178'633'661, 3'009'794'393};
    for (int ply=1; ply<=depth; ply++) {
        if (failed) break;
        auto start_time = high_resolution_clock::now();
        int n_moves = perft(ply, fen_string, render, false);
        auto stop_time = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop_time - start_time);
        if (n_moves != legal_moves_ply[ply-1]) {
            failed = true;
            status = "Failed";
        }
        printf("Position: 3\t Depth: %i ply\tLegal moves generated: %i \t\tExpected output: %lli\t\tTime: %llims\t\t", ply, n_moves, legal_moves_ply[ply-1], duration.count());
        cout << status << endl;
    }
}

void test_move_generation_pos4(bool render, int depth) {
    string fen_string = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
    bool failed = false;
    string status = "Passed";
    long long int legal_moves_ply[] = {6, 264, 9'467, 422'333, 15'833'292, 706'045'033};
    for (int ply=1; ply<=depth; ply++) {
        if (failed) break;
        auto start_time = high_resolution_clock::now();
        int n_moves = perft(ply, fen_string, render, false);
        auto stop_time = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop_time - start_time);
        if (n_moves != legal_moves_ply[ply-1]) {
            failed = true;
            status = "Failed";
        }
        printf("Position: 4\t Depth: %i ply\tLegal moves generated: %i \t\tExpected output: %lli\t\tTime: %llims\t\t", ply, n_moves, legal_moves_ply[ply-1], duration.count());
        cout << status << endl;
    }
}

void test_move_generation_pos5(bool render, int depth) {
    string fen_string = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
    bool failed = false;
    string status = "Passed";
    long long int legal_moves_ply[] = {44, 1'486, 62'379, 2'103'487, 89'941'194};
    for (int ply=1; ply<=depth; ply++) {
        if (failed) break;
        auto start_time = high_resolution_clock::now();
        int n_moves = perft(ply, fen_string, render, false);
        auto stop_time = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop_time - start_time);
        if (n_moves != legal_moves_ply[ply-1]) {
            failed = true;
            status = "Failed";
        }
        printf("Position: 5\t Depth: %i ply\tLegal moves generated: %i \t\tExpected output: %lli\t\tTime: %llims\t\t", ply, n_moves, legal_moves_ply[ply-1], duration.count());
        cout << status << endl;
    }
}

void test_move_generation_pos6(bool render, int depth) {
    string fen_string = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";
    bool failed = false;
    string status = "Passed";
    long long int legal_moves_ply[] = {46, 2'079, 89'890, 3'894'594, 164'075'551, 6'923'051'137, 287'188'994'746, 11'923'589'843'526};
    for (int ply=1; ply<=depth; ply++) {
        if (failed) break;
        auto start_time = high_resolution_clock::now();
        int n_moves = perft(ply, fen_string, render, false);
        auto stop_time = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(stop_time - start_time);
        if (n_moves != legal_moves_ply[ply-1]) {
            failed = true;
            status = "Failed";
        }
        printf("Position: 6\t Depth: %i ply\tLegal moves generated: %i \t\tExpected output: %lli\t\tTime: %llims\n", ply, n_moves, legal_moves_ply[ply-1], duration.count());
        cout << status << endl;
    }
}


int main() {
    // perft(3, START_FEN, false, true);

    // perft(2, "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", false, true);

    test_move_generation_pos1(false, 4);
    test_move_generation_pos2(false, 4);
    test_move_generation_pos3(false, 4);
    test_move_generation_pos4(false, 4);
    test_move_generation_pos5(false, 4);
    test_move_generation_pos6(false, 4);
    return 0;
}