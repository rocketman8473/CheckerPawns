CXX=g++
CXXFLAGS=-g -lSDL -I/usr/include/X11 -lSDL_image
PROJECT=CheckerPawns

all: main.o Settings.o Tile.o Board.o
	$(CXX) $(CXXFLAGS) main.o Settings.o Tile.o Board.o -o $(PROJECT)

main.o: main.cpp globals.h
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

Settings.o: Settings.cpp Settings.h
	$(CXX) $(CXXFLAGS) -c Settings.cpp -o Settings.o

Tile.o: Tile.cpp Tile.h globals.h
	$(CXX) $(CXXFLAGS) -c Tile.cpp -o Tile.o

Board.o: Board.cpp Board.h globals.h
	$(CXX) $(CXXFLAGS) -c Board.cpp -o Board.o

clean:
	rm *.o $(PROJECT)
