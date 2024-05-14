#include <iostream>
#include <array>
#define SFML_STATIC
#include <SFML/Graphics.hpp>
#include "Pieces.h"
#include "Move.h"
using namespace std;
#ifndef BOARD_H
#define BOARD_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 800
#define SQUARE_WIDTH SCREEN_WIDTH / 8
#define SQUARE_HEIGHT SCREEN_HEIGHT / 8
#define WINDOW_NAME "Chengine++"
#define CENTRAL_SQUARE 35
#define KNIGHT_OFFSET 8
#define DIAGONAL_OFFSET 4

const int knight_directions[] = {-17, -15, -10, -6, 6, 10, 15, 17};
const int orthogonal_directions[] = {-8, -1, 1, 8};
const int diagonal_directions[] = {-9, -7, 7, 9};
const int white_pawn_direction = -8;
const int white_pawn_capture_directions[] = {-9, -7};
const int black_pawn_direction = 8;
const int black_pawn_capture_directions[] = {9, 7};
const int promotion_pieces[] = {BISHOP, KNIGHT, ROOK, QUEEN};

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

/**
 * Implementation of a chess board with all the required rules.
*/
class Board {
    public: 
        array<int, 64> board;
        array<array<int, 16>, 64> steps_in_direction;
        int turn;
        bool render;
        array<array<bool, 64>, 2> attacked_squares;
        sf::RenderWindow window;

    private:
        string fen_string;
        bool white_castle_queenside;
        bool white_castle_kingside;
        bool black_castle_queenside;
        bool black_castle_kingside;
        bool removed_white_castle_kingside;
        bool removed_white_castle_queenside;
        bool removed_black_castle_kingside;
        bool removed_black_castle_queenside;
        bool just_removed_white_castling_kingside;
        bool just_removed_white_castling_queenside;
        bool just_removed_black_castling_kingside;
        bool just_removed_black_castling_queenside;
        unordered_map<char, int> piece_type_from_symbol = {
            {'k', KING},
            {'p', PAWN},
            {'n', KNIGHT},
            {'b', BISHOP},
            {'r', ROOK},
            {'q', QUEEN}
        };

        /**
         * @brief board from Fen string.
         * 
         * @param fen_string Fen string in the form of a std::string.
        */
        void load_fen(string fen_string);

        /**
         * @brief Sets up the display for the board.
        */
        void setup_render();

        /**
         * @brief Calls all the preprocess functions. 
         */
        void preprocess_data();

        /**
         * @brief Does preprocessing for the attacked squares.
         * 
         * Writes all possible attacked squares to a 2d array for each possible square and each possible direction.
         */
        void preprocess_steps_in_direction_squares();

    public:
        /**
         * @brief Constructor which uses standard start fen and no rendering.
        */
        Board();

        /**
         * @brief Constructor which requires a fen string to load and whether or not to render.
         * 
         * @param fen_string Fen string in the form of a std::string.
         * @param render Whether or not to render the board. True to render, false to not render.
        */
        Board(string fen_string, bool render);

        /**
         * @brief Gets the current fen string of the board.
         * 
         * @return Fen string of the current board in the form of a std::string.
        */
        string get_fen();

        /**
         * @brief Returns a boolean on whether or not a certain index is attacked by opposing pieces.
         * 
         * @param target The index of the target which needs to be checked.
         * @param color The color of the attacked piece.
         * @return True if the target is attacked and False if the target is not attacked.
         */
        bool target_is_attacked(int target, int color);

        /**
         * @brief Returns the index of the attacked map for a specific color.
         * 
         * @param color color of the map needed.
         * @return int index of the attacked map for that color.
         */
        int turn_to_attacked_map(int color);

        /**
         * @brief Gets the current position on the board.
         * 
         * @return The board array.
        */
        array<int, 64> get_board();

        /**
         * @brief Updates the renderwindow with the new board position.
        */
        void update_render();

        /**
         * @brief Checks if the given color's king is in check.
         * 
         * @param color The color to check if they are in check.
        */
        bool is_in_check(int color);

        /**
         * @brief Checks if the board is in a checkmate position.
        */
        bool is_checkmate();

        /**
         * @brief Moves a piece on the board.
         * 
         * @param move The move to make.
        */
        void move_piece(Move move);

        /**
         * @brief Puts a board back to the position it was in before a move was made.
         * 
         * @param move The move to undo.
        */
        void undo_move(Move move);

        /**
         * @brief Checks if a given move is legal on the board.
         * 
         * @param move The given move to be checked if it is legal.
        */
        bool move_is_legal(Move move);

        /**
         * @brief Updates tables for what squares are attacked by both sides.
        */
        void update_attacked_tables();

        /**
         * @brief Generates all pseudolegal moves
         * 
         * This function also returns moves that would put the user of the move in check.
         * This is required for faster move generation for bots as to not have to see if the move puts the user in check on pruned moves.
         * 
         * @param last_move Previous move played on the board.
         * @param board The board array to generate legal moves for.
         * @param turn Bool for which turn it is.
         * @return Array of Moves.
        */
        array<Move, 255> get_pseudolegal_moves(Move last_move, array<int, 64> board, int turn);
};

#endif