#include "Board.h"

std::unordered_map<int, int> pawn_directions = {
    {8, -8},
    {16, 8}
};

std::unordered_map<int, int> turn_to_attacked_map = {
    {8, 0},
    {16, 1}
};

Board::Board()
    : render(false), fen_string(START_FEN)
{
    load_fen(this->fen_string);
}

Board::Board(string fen_string, bool render) 
    : render(render), fen_string(fen_string)
{
    load_fen(this->fen_string);
    if (render == true) {
        this->setup_render();
    }
}

array<int, 64> Board::get_board() {
    return this->board;
}

string Board::get_fen() {
    return this->fen_string;
}

void Board::load_fen(string fen_string) {
    for (unsigned int i=0; i < this->board.size(); i++) {
        this->board[i] = EMPTY;
    }
    string board_fen = fen_string.substr(0, fen_string.find(' '));
    fen_string.erase(0, fen_string.find(' ')+1);
    string turn_string = fen_string.substr(0, fen_string.find(' '));
    this->turn = (turn_string.find('w') != std::string::npos) ? WHITE : BLACK;
    fen_string.erase(0, fen_string.find(' ')+1);
    string castling_rights = fen_string;

    this->white_castle_kingside = (castling_rights.find('K') != string::npos);
    this->white_castle_queenside = (castling_rights.find('Q') != string::npos);
    this->black_castle_kingside = (castling_rights.find('k') != string::npos);
    this->black_castle_queenside = (castling_rights.find('q') != string::npos);
    
    this->removed_white_castle_kingside = !this->white_castle_kingside;
    this->removed_white_castle_queenside = !this->white_castle_queenside;
    this->removed_black_castle_kingside = !this->black_castle_kingside;
    this->removed_black_castle_queenside = !this->black_castle_queenside;

    int file = 0;
    int rank = 0;

    for (auto symbol : board_fen) {
        if (symbol == '/') {
            file = 0;
            rank++;
        } else {
            if (isdigit(symbol)) {
                file += (int)symbol;
            } else {
                int piece_color = (isupper(symbol) ? WHITE : BLACK);
                int piece_type = this->piece_type_from_symbol.at(tolower(symbol));
                this->board[rank * 8 + file] = piece_color | piece_type;
                file++;
            }
        }
    }
    return;
}

void Board::move_piece(Move move) {
    // Promote
    if ((move.flag & Move::Flag::promote) == Move::Flag::promote) {
        this->board[move.end_square] = (this->board[move.start_square] & COLOR_MASK) | move.moved_piece;
        this->board[move.start_square] = EMPTY;
        return;
    }

    this->board[move.end_square] = this->board[move.start_square];
    this->board[move.start_square] = EMPTY;
}

void Board::undo_move(Move move) {
    // Promote
    if ((move.flag & Move::Flag::promote) == Move::Flag::promote) {
        this->board[move.start_square] = (this->board[move.end_square] & COLOR_MASK) | PAWN;
        this->board[move.end_square] = move.captured_piece;
        return;
    }

    this->board[move.start_square] = this->board[move.end_square];
    this->board[move.end_square] = move.captured_piece;
}

bool Board::move_is_legal(Move move) {
    return true;
}

void Board::update_attacked_tables() {
    for (int index=0; index<(int)this->board.size(); index++) {
        int piece = board[index];
        if (piece == EMPTY) continue;
        int color = piece & COLOR_MASK;
        int file = index % 8;
        int rank = (int)(index / 8);
        // Pawn attacked places
        if ((piece & PIECE_MASK) == PAWN) {
            int direction = pawn_directions.at(color);
            this->attacked_squares[turn_to_attacked_map.at(color)][index + direction + 1] = true;
            this->attacked_squares[turn_to_attacked_map.at(color)][index + direction - 1] = true;
        }
        // Knight attacked places
        if ((piece & PIECE_MASK) == KNIGHT) {
            int knight_move_amount = sizeof(knight_directions) / sizeof(knight_directions[0]);
            for (int i=0; i<knight_move_amount; i++) {
                int direction = knight_directions[i]; 
                int next_square = index + direction;
                int next_file = next_square % 8;
                int next_rank = (int)(next_square / 8);
                int rank_difference = (int)((CENTRAL_SQUARE + direction) / 8) - (int)(CENTRAL_SQUARE / 8);
                int file_difference = ((CENTRAL_SQUARE + direction) % 8) - (CENTRAL_SQUARE % 8);
                if ((next_file - file == file_difference) && next_rank - rank == rank_difference && (next_square >= 0 && next_square < 64)) {
                    this->attacked_squares[turn_to_attacked_map.at(color)][next_square] = true;
                }
            }
            continue;
        }

        // Orthogonally attacked places
        if (((piece & PIECE_MASK) == ROOK || (piece & PIECE_MASK) == QUEEN || (piece & PIECE_MASK) == KING)) {
            int orthogonal_move_amount = sizeof(orthogonal_directions) / sizeof(orthogonal_directions[0]);
            for (int i=0; i<orthogonal_move_amount; i++) {
                int direction = orthogonal_directions[i];
                int steps = 1;
                int next_square = index + (direction*steps);
                int next_rank = (int)(next_square / 8);
                int next_file = next_square % 8;
                while ((next_rank < 8 && next_rank >= 0 && next_file < 8 && next_file >=0) && (abs(next_file - file) == steps && abs(next_rank - rank) == steps) && (next_square >= 0 && next_square < 64)) {
                    this->attacked_squares[turn_to_attacked_map.at(color)][next_square] = true;
                    steps++;
                    if ((board[next_square] != EMPTY) || board[index] == KING) {
                        break;
                    }
                    next_square = index + (direction*steps);
                    next_rank = (int)(next_square / 8);
                    next_file = next_square % 8;
                }
            }
        }

        // Diagonally attacked places
        if (((piece & PIECE_MASK) == BISHOP || (piece & PIECE_MASK) == QUEEN || (piece & PIECE_MASK) == KING)) {
            int diagonal_move_amount = sizeof(diagonal_directions) / sizeof(diagonal_directions[0]);
            for (int i=0; i<diagonal_move_amount; i++) {
                int direction = diagonal_directions[i];
                int steps = 1;
                int next_square = index + (direction*steps);
                int next_rank = (int)(next_square / 8);
                int next_file = next_square % 8;
                while ((next_rank < 8 && next_rank >= 0 && next_file < 8 && next_file >=0) && (abs(next_file - file) == steps && abs(next_rank - rank) == steps) && (next_square >= 0 && next_square < 64)) {
                    this->attacked_squares[turn_to_attacked_map.at(color)][next_square] = true;
                    steps++;
                    if ((board[next_square] != EMPTY) || board[index] == KING) {
                        break;
                    }
                    next_square = index + (direction*steps);
                    next_rank = (int)(next_square / 8);
                    next_file = next_square % 8;
                }
            }
        }
    }
}

array<Move, 255> Board::get_pseudolegal_moves(Move last_move, array<int, 64> board, int turn) {
    array<Move, 255> moves;
    int n_moves = 0;
    int opposite_color = ~turn & COLOR_MASK;
    // Castling
    if (turn == WHITE) {
        if ((this->white_castle_kingside == true) && (board[63] == WHITE_ROOK) && (board[60] == WHITE_KING) && (board[62] == EMPTY) && (board[61] == EMPTY)) {
            if (this->attacked_squares[turn_to_attacked_map.at(opposite_color)][60] == false && this->attacked_squares[turn_to_attacked_map.at(opposite_color)][61] == false && this->attacked_squares[turn_to_attacked_map.at(opposite_color)][62] == false) {
                Move move(60, 63, WHITE_KING, EMPTY, Move::Flag::castle);
                moves[n_moves] = move;
                n_moves++;
            }
        }
        if (this->white_castle_queenside == true && board[56] == WHITE_ROOK && board[60] == WHITE_KING && board[59] == EMPTY && board[58] == EMPTY && board[57] == EMPTY) {
            if (this->attacked_squares[turn_to_attacked_map.at(opposite_color)][60] == false && this->attacked_squares[turn_to_attacked_map.at(opposite_color)][59] == false && this->attacked_squares[turn_to_attacked_map.at(opposite_color)][58] == false) {
                Move move(60, 56, WHITE_KING, EMPTY, Move::Flag::castle);
                moves[n_moves] = move;
                n_moves++;
            }
        }
    } else {
        if (this->black_castle_kingside == true && board[7] == BLACK_ROOK && board[4] == BLACK_KING && board[5] == EMPTY && board[6] == EMPTY) {
            if (this->attacked_squares[turn_to_attacked_map.at(opposite_color)][4] == false && this->attacked_squares[turn_to_attacked_map.at(opposite_color)][5] == false && this->attacked_squares[turn_to_attacked_map.at(opposite_color)][6] == false) {
                Move move(4, 7, BLACK_KING, EMPTY, Move::Flag::castle);
                moves[n_moves] = move;
                n_moves++;
            }
        }
        if (this->black_castle_queenside == true && board[0] == WHITE_ROOK && board[4] == WHITE_KING && board[3] == EMPTY && board[2] == EMPTY && board[1] == EMPTY) {
            if (this->attacked_squares[turn_to_attacked_map.at(opposite_color)][4] == false && this->attacked_squares[turn_to_attacked_map.at(opposite_color)][3] == false && this->attacked_squares[turn_to_attacked_map.at(opposite_color)][2] == false) {
                Move move(4, 0, BLACK_KING, EMPTY, Move::Flag::castle);
                moves[n_moves] = move;
                n_moves++;
            }
        }
    }

    for (int index=0; index<(int)board.size(); index++) {
        int piece = board[index];
        if (piece == EMPTY) continue;
        int file = index % 8;
        int rank = (int)(index / 8);
        // Pawn moves
        if ((piece & PIECE_MASK) == PAWN && (piece & COLOR_MASK) == turn) {
            int direction = pawn_directions.at(turn);
            int next_square = index + direction;
            int next_rank = (int)(next_square / 8);
            int next_file = next_square % 8;
            // Check if square in front is empty
            if (board[next_square] == EMPTY) {
                // Check if moving the pawn would promote
                if (next_rank == 0 || next_rank == 7) {
                    for (auto promoted_piece : promotion_pieces) {
                        Move move(index, next_square, promoted_piece, board[next_square], Move::Flag::promote);
                        moves[n_moves] = move;
                        n_moves++;
                    }
                } else {
                    Move move(index, next_square, piece, board[next_square], 0);
                    moves[n_moves] = move;
                    n_moves++;
                }
                // If pawn hasn't moved yet, check for 2 squares ahead
                if ((index > 47 && index < 56 && turn == WHITE && board[index + (direction*2)] == EMPTY) || (index > 7 && index < 16 && turn == BLACK && board[index + (direction*2)] == EMPTY)) {
                    Move move(index, index + (direction*2), piece, board[index + (direction*2)], 0);
                    moves[n_moves] = move;
                    n_moves++;
                }
                // Check for captures
                int capture_directions[2] = {direction+1, direction-1};
                for (auto capture_direction : capture_directions) {
                    int capture_square = index + capture_direction;
                    if (capture_square == EMPTY || (capture_square & COLOR_MASK) == turn) {
                        continue;
                    }
                    int capture_rank = (int)(capture_square / 8);
                    if (capture_rank != next_rank) {
                        continue;
                    }
                    // Check if pawn can promote with capture
                    if (capture_rank == 0 || capture_rank == 7){
                        for (auto promoted_piece : promotion_pieces) {
                            Move move(index, capture_square, promoted_piece, board[capture_square], Move::Flag::promote);
                            moves[n_moves] = move;
                            n_moves++;
                        }
                    }
                    else {
                        Move move(index, capture_square, piece, board[capture_square], 0);
                        moves[n_moves] = move;
                        n_moves++;
                    }
                }
                // En passant
                if (last_move.moved_piece != EMPTY) {
                    if (last_move.moved_piece == PAWN && (last_move.end_square == index-1 || last_move.end_square == index+1) && ((last_move.start_square == last_move.end_square-16 && turn == BLACK) || (last_move.start_square == last_move.end_square+16 && turn == WHITE)) && (int)(index/8) == (int)(last_move.end_square/8)) {
                        Move move(index, index + direction + (last_move.end_square - index), board[index], board[index + direction + (last_move.end_square - index)], Move::Flag::en_passant);
                        moves[n_moves] = move;
                        n_moves++;
                    }
                }
            }
            continue;
        }

        // Knight moves
        if ((piece & PIECE_MASK) == KNIGHT && (piece & COLOR_MASK) == turn) {
            int knight_move_amount = sizeof(knight_directions) / sizeof(knight_directions[0]);
            for (int i=0; i<knight_move_amount; i++) {
                int direction = knight_directions[i]; 
                int next_square = index + direction;
                int next_file = next_square % 8;
                int next_rank = (int)(next_square / 8);
                int rank_difference = (int)((CENTRAL_SQUARE + direction) / 8) - (int)(CENTRAL_SQUARE / 8);
                int file_difference = ((CENTRAL_SQUARE + direction) % 8) - (CENTRAL_SQUARE % 8);
                if ((next_file - file == file_difference) && next_rank - rank == rank_difference && (board[next_square] & COLOR_MASK) != turn && (next_square >= 0 && next_square < 64)) {
                    Move move(index, next_square, piece, board[next_square], 0);
                    moves[n_moves] = move;
                    n_moves++;
                }
            }
            continue;
        }
        // Diagonal moves
        if (((piece & PIECE_MASK) == BISHOP || (piece & PIECE_MASK) == QUEEN || (piece & PIECE_MASK) == KING) && (piece & COLOR_MASK) == turn) {
            int diagonal_move_amount = sizeof(diagonal_directions) / sizeof(diagonal_directions[0]);
            for (int i=0; i<diagonal_move_amount; i++) {
                int direction = diagonal_directions[i];
                int steps = 1;
                int next_square = index + (direction*steps);
                int next_rank = (int)(next_square / 8);
                int next_file = next_square % 8;
                while ((next_rank < 8 && next_rank >= 0 && next_file < 8 && next_file >=0) && (abs(next_file - file) == steps && abs(next_rank - rank) == steps) && (next_square >= 0 && next_square < 64) && ((board[next_square] & COLOR_MASK) != turn)) {
                    Move move(index, next_square, board[index], board[next_square], 0);
                    moves[n_moves] = move;
                    n_moves++;
                    steps++;
                    if ((board[next_square] & COLOR_MASK) == (~turn & COLOR_MASK) || board[index] == KING) {
                        break;
                    }
                    next_square = index + (direction*steps);
                    next_rank = (int)(next_square / 8);
                    next_file = next_square % 8;
                }
            }
        }

        // Orthogonal moves
        if (((piece & PIECE_MASK) == ROOK || (piece & PIECE_MASK) == QUEEN || (piece & PIECE_MASK) == KING) && (piece & COLOR_MASK) == turn) {
            int orthogonal_move_amount = sizeof(orthogonal_directions) / sizeof(orthogonal_directions[0]);
            for (int i=0; i<orthogonal_move_amount; i++) {
                int direction = diagonal_directions[i];
                int steps = 1;
                int next_square = index + (direction*steps);
                int next_rank = (int)(next_square / 8);
                int next_file = next_square % 8;
                while ((next_rank < 8 && next_rank >= 0 && next_file < 8 && next_file >=0) && (abs(next_file - file) == steps && abs(next_rank - rank) == steps) && (next_square >= 0 && next_square < 64) && ((board[next_square] & COLOR_MASK) != turn)) {
                    Move move(index, next_square, board[index], board[next_square], 0);
                    moves[n_moves] = move;
                    n_moves++;
                    steps++;
                    if ((board[next_square] & COLOR_MASK) == (~turn & COLOR_MASK) || board[index] == KING) {
                        break;
                    }
                    next_square = index + (direction*steps);
                    next_rank = (int)(next_square / 8);
                    next_file = next_square % 8;
                }
            }
        }
    }
    return moves;
}

void Board::setup_render() {
    this->window.create(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Chengine++");
    this->update_render();
}

void Board::update_render() {
    window.clear();
    int offset = 1;
    for (int i=0; i<(int)this->board.size(); i++) {
        if (i % 8 == 0) offset++;
        int x = (i % 8) * SQUARE_WIDTH;
        int y = (int)(i/8) * SQUARE_HEIGHT;
        sf::RectangleShape square(sf::Vector2f(SQUARE_WIDTH, SQUARE_HEIGHT));
        sf::Color color = ((offset + i) % 2 == 0) ? sf::Color(181, 151, 98) : sf::Color(128, 105, 66);
        square.setFillColor(color);
        square.setPosition(sf::Vector2f(x, y));
        window.draw(square);
        sf::RectangleShape piece(sf::Vector2f(SQUARE_WIDTH, SQUARE_HEIGHT));
        sf::Sprite sprite;
        sf::Texture texture;
        switch (this->board[i]) {
            case WHITE_PAWN:
                texture.loadFromFile("./assets/whitePawn.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+12.5, y+12.5));
                break;
            case WHITE_BISHOP:
                texture.loadFromFile("./assets/whiteBishop.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+10, y+10));
                break;
            case WHITE_KNIGHT:
                texture.loadFromFile("./assets/whiteKnight.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+12.5, y+12.5));
                break;
            case WHITE_ROOK:
                texture.loadFromFile("./assets/whiteRook.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+12.5, y+12.5));
                break;
            case WHITE_QUEEN:
                texture.loadFromFile("./assets/whiteQueen.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+7.5, y+7.5));
                break;
            case WHITE_KING:
                texture.loadFromFile("./assets/whiteKing.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+10, y+10));
                break;
            case BLACK_PAWN:
                texture.loadFromFile("./assets/blackPawn.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+12.5, y+12.5));
                break;
            case BLACK_BISHOP:
                texture.loadFromFile("./assets/blackBishop.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+10, y+10));
                break;
            case BLACK_KNIGHT:
                texture.loadFromFile("./assets/blackKnight.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+12.5, y+12.5));
                break;
            case BLACK_ROOK:
                texture.loadFromFile("./assets/blackRook.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+12.5, y+12.5));
                break;
            case BLACK_QUEEN:
                texture.loadFromFile("./assets/blackQueen.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+7.5, y+7.5));
                break;
            case BLACK_KING:
                texture.loadFromFile("./assets/blackKing.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+10, y+10));
                break;
            default:
                break;
        }
        window.draw(sprite);
    }
    window.display();
}