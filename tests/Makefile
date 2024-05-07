# Compiler
CC = g++ -m64

# Compiler Flags
CFLAGS = -g -Wall -IC:\SFML-2.6.1\include -LC:\SFML-2.6.1\lib

# All statically linked libraries
LIBS = -lsfml-graphics-s-d -lsfml-window-s-d -lsfml-system-s-d -lopengl32 -lfreetype -lwinmm -lgdi32

# Targets needed to bring the executable up to date
test_board: test_board.o Board.o Pieces.o Move.o
	$(CC) -o test_board test_board.o Board.o Pieces.o Move.o $(CFLAGS) $(LIBS)

test_board.o: test_board.cpp ../core/Board.h
	$(CC) -c test_board.cpp $(CFLAGS) $(LIBS)

Board.o: ../core/Board.cpp ../core/Pieces.h
	$(CC) -c ../core/Board.cpp $(CFLAGS) $(LIBS)

Pieces.o: ../core/Pieces.cpp
	$(CC) -c ../core/Pieces.cpp $(CFLAGS) $(LIBS)

Move.o: ../core/Move.cpp
	$(CC) -c ../core/Move.cpp $(CFLAGS) $(LIBS)

clean:
	$(RM) test_board.o Board.o Pieces.o Move.o 