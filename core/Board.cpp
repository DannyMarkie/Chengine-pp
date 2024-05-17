#include "Board.h"

std::unordered_map<int, int> pawn_directions = {
    {8, -8},
    {16, 8}
};

Board::Board()
    : render(false), fen_string(START_FEN)
{
    this->preprocess_data();
    load_fen(this->fen_string);
}

Board::Board(string fen_string, bool render) 
    : render(render), fen_string(fen_string)
{
    this->preprocess_data();
    load_fen(this->fen_string);
    if (render == true) {
        this->setup_render();
    }
}

void Board::preprocess_data() {
    this->preprocess_steps_in_direction_squares();
}

void Board::preprocess_steps_in_direction_squares() {
    for (int square=0; square<64; square++) {
        int rank = (int)(square / 8);
        int file = square % 8;
        // Orthogonal directions
        for (int i=0; i<4; i++) {
            int direction = orthogonal_directions[i];
            int steps = 1;
            int next_square = square + (direction*steps);
            int next_rank = (int)(next_square / 8);
            int next_file = next_square % 8;
            while ((next_rank < 8 && next_rank >= 0 && next_file < 8 && next_file >=0) && ((abs(next_file - file) == steps || abs(next_rank - rank) == steps) && (abs(next_file - file) + abs(next_rank - rank) == steps)) && (next_square >= 0 && next_square < 64)) {
                steps++;
                next_square = square + (direction*steps);
                next_rank = (int)(next_square / 8);
                next_file = next_square % 8;
            }
            this->steps_in_direction[square][i] = steps - 1;
        }

        // Diagonal directions
        for (int i=DIAGONAL_OFFSET; i<(DIAGONAL_OFFSET+4); i++) {
            int direction = diagonal_directions[i-4];
            int steps = 1;
            int next_square = square + (direction*steps);
            int next_rank = (int)(next_square / 8);
            int next_file = next_square % 8;
            while ((next_rank < 8 && next_rank >= 0 && next_file < 8 && next_file >=0) && (abs(next_file - file) == steps && abs(next_rank - rank) == steps) && (next_square >= 0 && next_square < 64)) {
                steps++;
                next_square = square + (direction*steps);
                next_rank = (int)(next_square / 8);
                next_file = next_square % 8;
            }
            this->steps_in_direction[square][i] = steps - 1;
        }

        // Knight directions
        for (int i=KNIGHT_OFFSET; i<(KNIGHT_OFFSET+8); i++) {
            int direction = knight_directions[i-8];
            int steps = 0;
            int next_square = square + direction;
            int next_file = next_square % 8;
            int next_rank = (int)(next_square / 8);
            int rank_difference = (int)((CENTRAL_SQUARE + direction) / 8) - (int)(CENTRAL_SQUARE / 8);
            int file_difference = ((CENTRAL_SQUARE + direction) % 8) - (CENTRAL_SQUARE % 8);
            if ((next_file - file == file_difference) && (next_rank - rank == rank_difference) && (next_square >= 0 && next_square < 64)) {
                steps++;
            }
            this->steps_in_direction[square][i] = steps;
        }
    }
}

int Board::turn_to_attacked_map(int color) {
    if (color == 0) {
        cout << "whut" << endl;
    }
    return ((color / 8) - 1);
}

array<int, 64> Board::get_board() {
    return this->board;
}

string Board::get_fen() {
    string fen = "";
    string symbols_from_piece[6] = {"p", "b", "n", "r", "q", "k"};
    int empty_squares_rank = 0;
    for (int rank=0; rank<8; rank++) {
        empty_squares_rank = 0;
        for (int file=0; file<8; file++) {
            int piece = this->board[(rank * 8) + file];
            if ((piece & COLOR_MASK) != 0) {
                if (empty_squares_rank > 0) {
                    fen += to_string(empty_squares_rank);
                    empty_squares_rank = 0;
                }
                string str = symbols_from_piece[(piece & PIECE_MASK)-1];
                if ((piece & COLOR_MASK) == WHITE) {
                    for (auto & c: str) c = toupper(c);
                }
                fen += str;
            } else {
                empty_squares_rank++;
            }
        }
        if (empty_squares_rank > 0) {
            fen += to_string(empty_squares_rank);
            empty_squares_rank = 0;
        }
        if (rank != 7) {
            fen += "/";
        }
    }
    fen += ((this->turn & COLOR_MASK) == WHITE) ? " w" : " b";
    fen += " ";
    fen += (this->white_castle_kingside) ? "K" : "";
    fen += (this->white_castle_queenside) ? "Q" : "";
    fen += (this->black_castle_kingside) ? "k" : "";
    fen += (this->black_castle_queenside) ? "q" : "";
    return fen;
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
                file += symbol - '0'; // Subtract '0' because characters are stored as ASCII values
            } else {
                int piece_color = (isupper(symbol) ? WHITE : BLACK);
                int piece_type = this->piece_type_from_symbol.at(tolower(symbol));
                this->board[(rank * 8) + file] = piece_color | piece_type;
                file++;
            }
        }
    }
    return;
}

int Board::square_mobility(int square, int color) {
    int n_moves = 0;
    // // Pawn attacked places
    // int pawn_direction = (color == WHITE) ? -8 : 8;
    // int pawn_attacking_directions[] = {pawn_direction+1, pawn_direction-1};
    // int target_rank = (int)(square / 8);
    // for (auto direction : pawn_attacking_directions) {
    //     int next_square = square + direction;
    //     int next_rank = (int)(next_square / 8);
    //     if (((this->board[next_square] & PIECE_MASK) == PAWN) && ((this->board[next_square] & COLOR_MASK) == (~color & COLOR_MASK)) && ((next_rank - target_rank) == (pawn_direction / 8))) {
    //         n_moves++;
    //     }
    // }

    // Knight attacked places
    for (int i=0; i<8; i++) {
        int direction = knight_directions[i];
        int next_square = square + direction;
        if ((this->steps_in_direction[square][i+KNIGHT_OFFSET] == 1) && ((this->board[next_square] & COLOR_MASK) != color) && (next_square >= 0 && next_square < 64)) {
            n_moves++;
        }
    }

    // Orthogonally attacked places
    for (int i=0; i<4; i++) {
        int direction = orthogonal_directions[i];
        int next_square = square + direction;
        int steps = 0;
        while ((this->steps_in_direction[square][i] >= steps) && ((this->board[next_square] & COLOR_MASK) != color)) {
            steps++;
            if ((this->board[next_square] & COLOR_MASK) != color) {
                n_moves++;
            }
            next_square = square + (direction * steps);
        }
    }

    // Diagonally attacked places
    for (int i=0; i<4; i++) {
        int direction = diagonal_directions[i];
        int next_square = square + direction;
        int steps = 0;
        while ((this->steps_in_direction[square][i+DIAGONAL_OFFSET] >= steps) && (next_square >= 0 && next_square < 64) && ((this->board[next_square] & COLOR_MASK) != color)) {
            steps++;
            if ((this->board[next_square] & COLOR_MASK) != color) {
                n_moves++;
            }
            next_square = square + (direction * steps);
        }
    }
    return n_moves;
}

bool Board::target_is_attacked(int target, int color) {
    // Pawn attacked places
    int pawn_direction = (color == WHITE) ? -8 : 8;
    int pawn_attacking_directions[] = {pawn_direction+1, pawn_direction-1};
    int target_rank = (int)(target / 8);
    for (auto direction : pawn_attacking_directions) {
        int next_square = target + direction;
        int next_rank = (int)(next_square / 8);
        if (((this->board[next_square] & PIECE_MASK) == PAWN) && ((this->board[next_square] & COLOR_MASK) == (~color & COLOR_MASK)) && ((next_rank - target_rank) == (pawn_direction / 8))) {
            return true;
        }
    }

    // Knight attacked places
    for (int i=0; i<8; i++) {
        int direction = knight_directions[i];
        int next_square = target + direction;
        if ((this->steps_in_direction[target][i+KNIGHT_OFFSET] == 1) && ((this->board[next_square] & COLOR_MASK) == (~color & COLOR_MASK) && (this->board[next_square] & PIECE_MASK) == KNIGHT) && (next_square >= 0 && next_square < 64)) {
            return true;
        }
    }

    // Orthogonally attacked places
    for (int i=0; i<4; i++) {
        int direction = orthogonal_directions[i];
        int next_square = target + direction;
        int steps = 0;
        while ((this->steps_in_direction[target][i] >= steps) && ((this->board[next_square] & COLOR_MASK) != color)) {
            steps++;
            if ((this->board[next_square] & COLOR_MASK) == (~color & COLOR_MASK)) {
                if ((this->board[next_square] & PIECE_MASK) == QUEEN || (this->board[next_square] & PIECE_MASK) == ROOK || ((this->board[next_square] & PIECE_MASK) == KING && steps == 1)) {
                    return true;
                }
                break;
            }
            next_square = target + (direction * steps);
        }
    }

    // Diagonally attacked places
    for (int i=0; i<4; i++) {
        int direction = diagonal_directions[i];
        int next_square = target + direction;
        int steps = 0;
        while ((this->steps_in_direction[target][i+DIAGONAL_OFFSET] >= steps) && (next_square >= 0 && next_square < 64) && ((this->board[next_square] & COLOR_MASK) != color)) {
            steps++;
            if ((this->board[next_square] & COLOR_MASK) == (~color & COLOR_MASK)) {
                if ((this->board[next_square] & PIECE_MASK) == QUEEN || (this->board[next_square] & PIECE_MASK) == BISHOP || ((this->board[next_square] & PIECE_MASK) == KING && steps == 1)) {
                    return true;
                }
                break;
            }
            next_square = target + (direction * steps);
        }
    }
    return false;
}

bool Board::is_in_check(int color) {
    int king_square;
    for (int i=0; i<64; i++) {
        if (this->board[i] == (color | KING)) {
            king_square = i;
        }
    }
    return this->target_is_attacked(king_square, color);
    // return this->attacked_squares[this->turn_to_attacked_map(opposite_color)][king_square];
}

bool Board::is_checkmate() {
    int n_moves = 0;
    array<Move, 255> moves = this->get_pseudolegal_moves(Move(), this->get_board(), this->turn);
    for (Move move : moves) {
        if (move.moved_piece == EMPTY) break;
        if (!this->move_is_legal(move)) continue;
        n_moves++;
    }
    return (this->is_in_check(this->turn) && n_moves == 0);
}

bool Board::has_legal_moves() {
    array<Move, 255> moves = this->get_pseudolegal_moves(Move(), this->board, this->turn);
    int n_illegal_moves = 0;
    for (int i=0; i<255; i++) {
        Move move = moves[i];
        if (move.moved_piece == EMPTY) return ((i-n_illegal_moves)>0);
        if (!this->move_is_legal(move)) n_illegal_moves++;
    }
    return false;
}

void Board::move_piece(Move move) {
    this->turn = (~turn & COLOR_MASK);
    this->just_removed_white_castling_kingside = false;
    this->just_removed_white_castling_queenside = false;
    this->just_removed_black_castling_kingside = false;
    this->just_removed_black_castling_queenside = false;

    // Remove castling rights if rook gets taken
    if ((move.captured_piece & PIECE_MASK) == ROOK) {
        if ((move.captured_piece & COLOR_MASK) == WHITE) {
            // If castling rights are not yet removed, remove it
            if (move.end_square == 56 && !this->removed_white_castle_queenside) {
                this->white_castle_queenside = false;
                this->just_removed_white_castling_queenside = true;
            } else if (move.end_square == 63 && !this->removed_white_castle_kingside) {
                this->white_castle_kingside = false;
                this->just_removed_white_castling_kingside = true;
            }
        } else {
            // If castling rights are not yet removed, remove it
            if (move.end_square == 0 && !this->removed_black_castle_queenside) {
                this->black_castle_queenside = false;
                this->just_removed_black_castling_queenside = true;
            } else if (move.end_square == 7 && !this->removed_black_castle_kingside) {
                this->black_castle_kingside = false;
                this->just_removed_black_castling_kingside = true;
            }
        }
    }

    // Remove castling rights if rook moves
    if ((move.moved_piece & PIECE_MASK) == ROOK) {
        if ((move.moved_piece & COLOR_MASK) == WHITE) {
            if (move.start_square == 56 && this->white_castle_queenside) {
                this->white_castle_queenside = false;
                this->just_removed_white_castling_queenside = true;
            } else if (move.start_square == 63 && this->white_castle_kingside) {
                this->white_castle_kingside = false;
                this->just_removed_white_castling_kingside = true;
            }
        } else {
            if (move.start_square == 0 && this->black_castle_queenside) {
                this->black_castle_queenside = false;
                this->just_removed_black_castling_queenside = true;
            } else if (move.start_square == 7 && this->black_castle_kingside) {
                this->black_castle_kingside = false;
                this->just_removed_black_castling_kingside = true;
            }
        }
    }

    // Remove castling rights if king moves
    if ((move.moved_piece & PIECE_MASK) == KING) {
        if ((move.moved_piece & COLOR_MASK) == WHITE && (this->white_castle_kingside && this->white_castle_queenside)) {
            this->white_castle_kingside = false;
            this->white_castle_queenside = false;
            this->just_removed_white_castling_kingside = true;
            this->just_removed_white_castling_queenside = true;
        } else if ((move.moved_piece & COLOR_MASK) == BLACK && (this->black_castle_kingside && black_castle_queenside)) {
            this->black_castle_kingside = false;
            this->black_castle_queenside = false;
            this->just_removed_black_castling_kingside = true;
            this->just_removed_black_castling_queenside = true;
        }
    }

    // Castle
    if ((move.flag & Move::Flag::castle) == Move::Flag::castle) {
        if ((move.moved_piece & COLOR_MASK) == WHITE) {
            this->just_removed_white_castling_kingside = true;
            this->white_castle_kingside = false;
            this->just_removed_white_castling_queenside = true;
            this->white_castle_queenside = false;
        } else {
            this->just_removed_black_castling_kingside = true;
            this->black_castle_kingside = false;
            this->just_removed_black_castling_queenside = true;
            this->black_castle_queenside = false;
        }
        this->board[move.start_square] = EMPTY;
        this->board[move.end_square] = EMPTY;
        if (move.start_square > move.end_square) {
            this->board[move.start_square - 2] = move.moved_piece;
            this->board[move.start_square - 1] = (move.moved_piece & COLOR_MASK) | ROOK;
        } else {
            this->board[move.start_square + 2] = move.moved_piece;
            this->board[move.start_square + 1] = (move.moved_piece & COLOR_MASK) | ROOK;
        }
        return;
    }

    // Promote
    if ((move.flag & Move::Flag::promote) == Move::Flag::promote) {
        this->board[move.end_square] = (this->board[move.start_square] & COLOR_MASK) | move.moved_piece;
        this->board[move.start_square] = EMPTY;
        return;
    }

    // En passant
    if ((move.flag & Move::Flag::en_passant) == Move::Flag::en_passant) {
        int end_file = move.end_square % 8;
        int start_file = move.start_square % 8;
        this->board[move.end_square] = this->board[move.start_square];
        this->board[move.start_square] = EMPTY;
        this->board[move.start_square + end_file - start_file] = EMPTY;
        return;
    }

    this->board[move.end_square] = this->board[move.start_square];
    this->board[move.start_square] = EMPTY;
}

void Board::undo_move(Move move) {
    this->turn = (~this->turn & COLOR_MASK);
    // Return castling rights
    if (this->just_removed_white_castling_kingside) {
        this->white_castle_kingside = true;
        this->just_removed_white_castling_kingside = false;
    }
    if (this->just_removed_white_castling_queenside) {
        this->white_castle_queenside = true;
        this->just_removed_white_castling_queenside = false;
    }
    if (this->just_removed_black_castling_kingside) {
        this->black_castle_kingside = true;
        this->just_removed_black_castling_kingside = false;
    }
    if (this->just_removed_black_castling_queenside) {
        this->black_castle_queenside = true;
        this->just_removed_black_castling_queenside = false;
    }

    // Castle
    if ((move.flag & Move::Flag::castle) == Move::Flag::castle) {
        if (move.start_square > move.end_square) {
            this->board[move.start_square - 2] = EMPTY;
            this->board[move.start_square - 1] = EMPTY;
        } else {
            this->board[move.start_square + 2] = EMPTY;
            this->board[move.start_square + 1] = EMPTY;
        }
        this->board[move.start_square] = move.moved_piece;
        this->board[move.end_square] = (move.moved_piece & COLOR_MASK) | ROOK;
        return;
    }

    // Promote
    if ((move.flag & Move::Flag::promote) == Move::Flag::promote) {
        this->board[move.start_square] = ((this->board[move.end_square] & COLOR_MASK) | PAWN);
        this->board[move.end_square] = move.captured_piece;
        return;
    }

    // En passant
    if ((move.flag & Move::Flag::en_passant) == Move::Flag::en_passant) {
        this->board[move.start_square] = this->board[move.end_square];
        this->board[move.end_square] = EMPTY;
        int offset = ((this->turn & COLOR_MASK) == WHITE) ? 1 : -1;
        this->board[move.end_square + (offset * 8)] = (~this->turn & COLOR_MASK) | PAWN;
        return;
    }

    this->board[move.start_square] = this->board[move.end_square];
    this->board[move.end_square] = move.captured_piece;
}

bool Board::move_is_legal(Move move) {
    int color = (this->board[move.start_square] & COLOR_MASK);
    this->move_piece(move);
    bool is_legal = (!this->is_in_check(color));
    this->undo_move(move);
    return is_legal;
}

void Board::update_attacked_tables() {
    this->attacked_squares[0].fill(false);
    this->attacked_squares[1].fill(false);
    for (int index=0; index<(int)this->board.size(); index++) {
        int piece = board[index];
        if (piece == EMPTY) continue;
        int color = piece & COLOR_MASK;
        // Pawn attacked places
        if ((piece & PIECE_MASK) == PAWN) {
            int direction = (color == WHITE) ? -8 : 8;
            this->attacked_squares[this->turn_to_attacked_map(color)][index + direction + 1] = true;
            this->attacked_squares[this->turn_to_attacked_map(color)][index + direction - 1] = true;
        }
        // Knight attacked places
        if ((piece & PIECE_MASK) == KNIGHT) {
            for (int i=0; i<8; i++) {
                int direction = knight_directions[i];
                int next_square = index + direction;
                if ((this->steps_in_direction[index][i+KNIGHT_OFFSET] == 1) && ((board[next_square] & COLOR_MASK) != turn) && (next_square >= 0 && next_square < 64)) {
                    this->attacked_squares[this->turn_to_attacked_map(color)][next_square] = true;
                }
            }
            continue;
        }

        // Orthogonally attacked places
        if (((piece & PIECE_MASK) == ROOK || (piece & PIECE_MASK) == QUEEN || (piece & PIECE_MASK) == KING)) {
            for (int i=0; i<4; i++) {
                int direction = orthogonal_directions[i];
                int next_square = index + direction;
                int steps = 1;
                while ((this->steps_in_direction[index][i] >= steps) && ((board[next_square] & COLOR_MASK) != turn)) {
                    this->attacked_squares[this->turn_to_attacked_map(color)][next_square] = true;
                    steps++;
                    if ((board[next_square] != EMPTY) || board[index] == KING) {
                        break;
                    }
                    next_square = index + (direction*steps);
                }
            }
        }

        // Diagonally attacked places
        if (((piece & PIECE_MASK) == BISHOP || (piece & PIECE_MASK) == QUEEN || (piece & PIECE_MASK) == KING)) {
            for (int i=0; i<4; i++) {
                int direction = diagonal_directions[i];
                int next_square = index + direction;
                int steps = 1;
                while ((this->steps_in_direction[index][i+DIAGONAL_OFFSET] >= steps) && (next_square >= 0 && next_square < 64)) {
                    this->attacked_squares[this->turn_to_attacked_map(color)][next_square] = true;
                    steps++;
                    if ((board[next_square] != EMPTY) || board[index] == KING) {
                        break;
                    }
                    next_square = index + (direction*steps);
                }
            }
        }
    }
}

array<Move, 255> Board::get_pseudolegal_moves(Move last_move, array<int, 64> board, int turn) {
    array<Move, 255> moves;
    moves.fill(Move());
    int n_moves = 0;
    // Castling
    if (turn == WHITE) {
        if ((this->white_castle_kingside == true) && (board[63] == WHITE_ROOK) && (board[60] == WHITE_KING) && (board[62] == EMPTY) && (board[61] == EMPTY)) {
            if (this->target_is_attacked(60, turn) == false && this->target_is_attacked(61, turn) == false && this->target_is_attacked(62, turn) == false) {
            // if (this->attacked_squares[this->turn_to_attacked_map(opposite_color)][60] == false && this->attacked_squares[this->turn_to_attacked_map(opposite_color)][61] == false && this->attacked_squares[this->turn_to_attacked_map(opposite_color)][62] == false) {
                Move move(60, 63, WHITE_KING, EMPTY, Move::Flag::castle);
                moves[n_moves] = move;
                n_moves++;
            }
        }
        if (this->white_castle_queenside == true && board[56] == WHITE_ROOK && board[60] == WHITE_KING && board[59] == EMPTY && board[58] == EMPTY && board[57] == EMPTY) {
            if (this->target_is_attacked(60, turn) == false && this->target_is_attacked(59, turn) == false && this->target_is_attacked(58, turn) == false) {
            // if (this->attacked_squares[this->turn_to_attacked_map(opposite_color)][60] == false && this->attacked_squares[this->turn_to_attacked_map(opposite_color)][59] == false && this->attacked_squares[this->turn_to_attacked_map(opposite_color)][58] == false) {
                Move move(60, 56, WHITE_KING, EMPTY, Move::Flag::castle);
                moves[n_moves] = move;
                n_moves++;
            }
        }
    } else {
        if (this->black_castle_kingside == true && board[7] == BLACK_ROOK && board[4] == BLACK_KING && board[5] == EMPTY && board[6] == EMPTY) {
            if (this->target_is_attacked(4, turn) == false && this->target_is_attacked(5, turn) == false && this->target_is_attacked(6, turn) == false) {
            // if (this->attacked_squares[this->turn_to_attacked_map(opposite_color)][4] == false && this->attacked_squares[this->turn_to_attacked_map(opposite_color)][5] == false && this->attacked_squares[this->turn_to_attacked_map(opposite_color)][6] == false) {
                Move move(4, 7, BLACK_KING, EMPTY, Move::Flag::castle);
                moves[n_moves] = move;
                n_moves++;
            }
        }
        if (this->black_castle_queenside == true && board[0] == BLACK_ROOK && board[4] == BLACK_KING && board[3] == EMPTY && board[2] == EMPTY && board[1] == EMPTY) {
            if (this->target_is_attacked(4, turn) == false && this->target_is_attacked(3, turn) == false && this->target_is_attacked(2, turn) == false) {
            // cout << this->turn_to_attacked_map(opposite_color) << endl;
            // cout << this->attacked_squares[this->turn_to_attacked_map(opposite_color)][3] << endl;
            // if (this->attacked_squares[this->turn_to_attacked_map(opposite_color)][4] == false && this->attacked_squares[this->turn_to_attacked_map(opposite_color)][3] == false && this->attacked_squares[this->turn_to_attacked_map(opposite_color)][2] == false) {
                Move move(4, 0, BLACK_KING, EMPTY, Move::Flag::castle);
                moves[n_moves] = move;
                n_moves++;
            }
        }
    }

    for (int index=0; index<(int)board.size(); index++) {
        int piece = board[index];
        if (piece == EMPTY) continue;
        // Pawn moves
        if ((piece & PIECE_MASK) == PAWN && (piece & COLOR_MASK) == turn) {
            int direction = (turn == WHITE) ? -8 : 8;
            int next_square = index + direction;
            int next_rank = (int)(next_square / 8);
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
            }
            // Check for captures
            int capture_directions[] = {direction+1, direction-1};
            for (auto capture_direction : capture_directions) {
                int capture_square = index + capture_direction;
                int capture_rank = (int)(capture_square / 8);
                // printf("Start square: %i\tCapture square: %i\tNext rank: %i\tCapture rank: %i\n", index, capture_square, 8-next_rank, 8-capture_rank);
                if ((board[capture_square] == EMPTY) || ((board[capture_square] & COLOR_MASK) == turn)) {
                    continue;
                }
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
                if ((last_move.moved_piece & PIECE_MASK) == PAWN && (last_move.end_square == index-1 || last_move.end_square == index+1) && ((last_move.end_square == last_move.start_square-16 && turn == BLACK) || (last_move.end_square == last_move.start_square+16 && turn == WHITE)) && (int)(index/8) == (int)(last_move.end_square/8)) {
                    Move move(index, index + direction + (last_move.end_square - index), board[index], board[index + direction + (last_move.end_square - index)], Move::Flag::en_passant);
                    moves[n_moves] = move;
                    n_moves++;
                }
            }
            continue;
        }

        // Knight moves
        if ((piece & PIECE_MASK) == KNIGHT && (piece & COLOR_MASK) == turn) {
            for (int i=0; i<8; i++) {
                int direction = knight_directions[i];
                int next_square = index + direction;
                if ((this->steps_in_direction[index][i+KNIGHT_OFFSET] == 1) && ((board[next_square] & COLOR_MASK) != turn) && (next_square >= 0 && next_square < 64)) {
                    Move move(index, next_square, piece, board[next_square], 0);
                    moves[n_moves] = move;
                    n_moves++;
                }
            }
            continue;
        }
        // Diagonal moves
        if (((piece & PIECE_MASK) == BISHOP || (piece & PIECE_MASK) == QUEEN || (piece & PIECE_MASK) == KING) && (piece & COLOR_MASK) == turn) {
            for (int i=0; i<4; i++) {
                int direction = diagonal_directions[i];
                int next_square = index + direction;
                int steps = 1;
                while ((this->steps_in_direction[index][i+DIAGONAL_OFFSET] >= steps) && ((board[next_square] & COLOR_MASK) != turn) && (next_square >= 0 && next_square < 64)) {
                    steps++;
                    Move move(index, next_square, piece, board[next_square], 0);
                    moves[n_moves] = move;
                    n_moves++;
                    if ((piece & PIECE_MASK) == KING || (board[next_square] & COLOR_MASK) == (~turn & COLOR_MASK)) break;
                    next_square = index + (direction * steps);
                }
            }
        }

        // Orthogonal moves
        if (((piece & PIECE_MASK) == ROOK || (piece & PIECE_MASK) == QUEEN || (piece & PIECE_MASK) == KING) && (piece & COLOR_MASK) == turn) {
            for (int i=0; i<4; i++) {
                int direction = orthogonal_directions[i];
                int next_square = index + direction;
                int steps = 1;
                while ((this->steps_in_direction[index][i] >= steps) && ((board[next_square] & COLOR_MASK) != turn)) {
                    steps++;
                    Move move(index, next_square, piece, board[next_square], 0);
                    moves[n_moves] = move;
                    n_moves++;
                    if ((piece & PIECE_MASK) == KING || ((board[next_square] & COLOR_MASK) == (~turn & COLOR_MASK))) break;
                    next_square = index + (direction * steps);
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
                texture.loadFromFile("C:/Users/Danny/Documents/Programming/Git Projects/Chengine++/Chengine-pp/assets/whitePawn.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+12.5, y+12.5));
                break;
            case WHITE_BISHOP:
                texture.loadFromFile("C:/Users/Danny/Documents/Programming/Git Projects/Chengine++/Chengine-pp/assets/whiteBishop.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+10, y+10));
                break;
            case WHITE_KNIGHT:
                texture.loadFromFile("C:/Users/Danny/Documents/Programming/Git Projects/Chengine++/Chengine-pp/assets/whiteKnight.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+12.5, y+12.5));
                break;
            case WHITE_ROOK:
                texture.loadFromFile("C:/Users/Danny/Documents/Programming/Git Projects/Chengine++/Chengine-pp/assets/whiteRook.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+12.5, y+12.5));
                break;
            case WHITE_QUEEN:
                texture.loadFromFile("C:/Users/Danny/Documents/Programming/Git Projects/Chengine++/Chengine-pp/assets/whiteQueen.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+7.5, y+7.5));
                break;
            case WHITE_KING:
                texture.loadFromFile("C:/Users/Danny/Documents/Programming/Git Projects/Chengine++/Chengine-pp/assets/whiteKing.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+10, y+10));
                break;
            case BLACK_PAWN:
                texture.loadFromFile("C:/Users/Danny/Documents/Programming/Git Projects/Chengine++/Chengine-pp/assets/blackPawn.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+12.5, y+12.5));
                break;
            case BLACK_BISHOP:
                texture.loadFromFile("C:/Users/Danny/Documents/Programming/Git Projects/Chengine++/Chengine-pp/assets/blackBishop.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+10, y+10));
                break;
            case BLACK_KNIGHT:
                texture.loadFromFile("C:/Users/Danny/Documents/Programming/Git Projects/Chengine++/Chengine-pp/assets/blackKnight.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+12.5, y+12.5));
                break;
            case BLACK_ROOK:
                texture.loadFromFile("C:/Users/Danny/Documents/Programming/Git Projects/Chengine++/Chengine-pp/assets/blackRook.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+12.5, y+12.5));
                break;
            case BLACK_QUEEN:
                texture.loadFromFile("C:/Users/Danny/Documents/Programming/Git Projects/Chengine++/Chengine-pp/assets/blackQueen.png");
                sprite.setTexture(texture);
                sprite.setPosition(sf::Vector2f(x+7.5, y+7.5));
                break;
            case BLACK_KING:
                texture.loadFromFile("C:/Users/Danny/Documents/Programming/Git Projects/Chengine++/Chengine-pp/assets/blackKing.png");
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