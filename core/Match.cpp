#include "Match.h"

Match::Match(Bot* bot1, Bot* bot2)
: bot1(bot1), bot2(bot2)
{

}

int Match::play() {
    // Render the starting position and wait 2 seconds to start.
    if (this->board.render) {
        printf("Game starting in 2 seconds.\n");
        this->board.update_render();
        sleep_for(2s);
        sf::Event event;
        while (this->board.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                board.window.close();
            }
        }
    }
    // While nobody has won and there are legal moves, get the next move from the current player.
    while (!this->winner && this->board.has_legal_moves() && (this->moves_since_capture < 51 && this->moves_since_pawn_push < 51)) {
        Board board_clone(this->board.get_fen(), false);
        Move move;
        // Get the best move from the player.
        if (this->board.turn == WHITE) {
            move = this->bot1->get_move(board_clone);
            this->board.move_piece(move);
            if (this->board.is_checkmate()) {
                this->winner = WHITE;
                break;
            }
        } else if (this->board.turn == BLACK) {
            move = this->bot2->get_move(board_clone);
            this->board.move_piece(move);
            if (this->board.is_checkmate()) {
                this->winner = BLACK;
                break;
            }
        }

        // Increment moves since a piece has last been captured if no capture has been made.
        // Otherwise reset the counter to 0.
        if (move.captured_piece == EMPTY) this->moves_since_capture++;
        else this->moves_since_capture = 0;

        // Increment moves since a pawn has last been pushed if no pawn is pushed.
        // Otherwise reset the counter to 0.
        if ((move.moved_piece & PIECE_MASK) == PAWN) this->moves_since_pawn_push = 0;
        else this->moves_since_pawn_push++;

        // Update the render
        if (this->board.render) {
            this->board.update_render();
            sf::Event event;
            while (this->board.window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    board.window.close();
                }
            }
        }
    }
    // When the game is over, print the winner and show the final position for 10 more seconds.
    cout << this->winner << endl;
    if (this->board.render) {
        this->board.update_render();
        sleep_for(10s);
        sf::Event event;
        while (this->board.window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                board.window.close();
            }
        }
    }
    return this->winner;
}