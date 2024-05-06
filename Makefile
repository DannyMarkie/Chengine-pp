# Compiler
CC = g++ -m64

# Compiler Flags
CFLAGS = -g -Wall -IC:\SFML-2.6.1\include -LC:\SFML-2.6.1\lib

# All statically linked libraries
LIBS = -lsfml-graphics-s-d -lsfml-window-s-d -lsfml-system-s-d -lopengl32 -lfreetype -lwinmm -lgdi32

# Targets needed to bring the executable up to date
main: main.o Board.o Pieces.o Move.o
	$(CC) -o main main.o Board.o Pieces.o Move.o $(CFLAGS) $(LIBS)

main.o: main.cpp core/Board.h core/Pieces.h
	$(CC) -c main.cpp $(CFLAGS) $(LIBS)

Board.o: core/Board.cpp core/Pieces.h
	$(CC) -c core/Board.cpp $(CFLAGS) $(LIBS)

Pieces.o: core/Pieces.cpp
	$(CC) -c core/Pieces.cpp $(CFLAGS) $(LIBS)

Move.o: core/Move.cpp
	$(CC) -c core/Move.cpp $(CFLAGS) $(LIBS)

clean:
	$(RM) main.o Board.o Pieces.o