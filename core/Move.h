#ifndef MOVE_H
#define MOVE_H

#define NULL_MOVE Move()

class Move {
    public:
        class Flag{
            public:
                const static int en_passant = 1;
                const static int castle = 2;
                const static int promote = 4;
        };
        Move(int start_square, int end_square, int moved_piece, int captured_piece, int flag);

        Move();

        int start_square;
        int end_square;
        int moved_piece;
        int captured_piece;
        int flag;

    public:
        /**
         * Overloading the equality operator to be able to compare moves and see if they are equal.
        */
        bool operator == (const Move &m);
};

#endif