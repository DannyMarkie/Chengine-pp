# Compiler
CC = g++ -m64

# Compiler Flags
CFLAGS = -g -Wall -IC:\SFML-2.6.1\include -LC:\SFML-2.6.1\lib

# All statically linked libraries
LIBS = -lsfml-graphics-s-d -lsfml-window-s-d -lsfml-system-s-d -lopengl32 -lfreetype -lwinmm -lgdi32

# Targets needed to bring the executable up to date
main: main.o Board.o Pieces.o Move.o Match.o RandomBot.o NegamaxV1.o IterativeDeepeningV2.o AlphaBetaPruningV3.o
	$(CC) -o main main.o Board.o Pieces.o Move.o Match.o RandomBot.o NegamaxV1.o IterativeDeepeningV2.o AlphaBetaPruningV3.o $(CFLAGS) $(LIBS)

main.o: main.cpp core/Board.h core/Pieces.h
	$(CC) -c main.cpp $(CFLAGS) $(LIBS)

Board.o: core/Board.cpp core/Pieces.h
	$(CC) -c core/Board.cpp $(CFLAGS) $(LIBS)

Pieces.o: core/Pieces.cpp
	$(CC) -c core/Pieces.cpp $(CFLAGS) $(LIBS)

Move.o: core/Move.cpp
	$(CC) -c core/Move.cpp $(CFLAGS) $(LIBS)

Match.o: core/Match.cpp bots/Bot.h
	$(CC) -c core/Match.cpp $(CFLAGS) $(LIBS)

RandomBot.o: bots/RandomBot.cpp bots/Bot.h
	$(CC) -c bots/RandomBot.cpp $(CFLAGS) $(LIBS)

NegamaxV1.o: bots/NegamaxV1.cpp bots/Bot.h core/Pieces.h
	$(CC) -c bots/NegamaxV1.cpp $(CFLAGS) $(LIBS)

IterativeDeepeningV2.o: bots/IterativeDeepeningV2.cpp bots/Bot.h core/Pieces.h
	$(CC) -c bots/IterativeDeepeningV2.cpp $(CFLAGS) $(LIBS)

AlphaBetaPruningV3.o: bots/AlphaBetaPruningV3.cpp bots/Bot.h core/Pieces.h
	$(CC) -c bots/AlphaBetaPruningV3.cpp $(CFLAGS) $(LIBS)

clean:
	$(RM) main.o Board.o Pieces.o Move.o Match.o RandomBot.o NegamaxV1.o IterativeDeepeningV2.o AlphaBetaPruningV3.o