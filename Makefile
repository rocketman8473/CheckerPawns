CXX=g++
CXXFLAGS=-g -lSDL -lSDL_image
PROJECT=CheckerPawns

all: main.o Window.o Settings.o Tile.o Row.o Board.o
	$(CXX) $(CXXFLAGS) main.o Window.o Settings.o Tile.o Row.o Board.o -o $(PROJECT)

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o main.o

Window.o: Window.cpp Window.h
	$(CXX) $(CXXFLAGS) -c Window.cpp -o Window.o

Settings.o: Settings.cpp Settings.h
	$(CXX) $(CXXFLAGS) -c Settings.cpp -o Settings.o

Tile.o: Tile.cpp Tile.h
	$(CXX) $(CXXFLAGS) -c Tile.cpp -o Tile.o

Row.o: Row.cpp Row.h
	$(CXX) $(CXXFLAGS) -c Row.cpp -o Row.o

Board.o: Board.cpp Board.h
	$(CXX) $(CXXFLAGS) -c Board.cpp -o Board.o

clean:
	rm *.o $(PROJECT)
