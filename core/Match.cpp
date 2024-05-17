#include "Match.h"

Match::Match(Bot* bot1, Bot* bot2)
: bot1(bot1), bot2(bot2)
{

}

int Match::play() {
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
    while (!this->winner && this->board.has_legal_moves() && (this->moves_since_capture < 51 && this->moves_since_pawn_push < 51)) {
        Board board_clone(this->board.get_fen(), false);
        Move move;
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

        if (move.captured_piece == EMPTY) this->moves_since_capture++;
        else this->moves_since_capture = 0;

        if ((move.moved_piece & PIECE_MASK) == PAWN) this->moves_since_pawn_push = 0;
        else this->moves_since_pawn_push++;

        if (this->board.render) {
            this->board.update_render();
            sleep_for(500ms);
            sf::Event event;
            while (this->board.window.pollEvent(event)) {
                if (event.type == sf::Event::Closed) {
                    board.window.close();
                }
            }
        }
    }
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