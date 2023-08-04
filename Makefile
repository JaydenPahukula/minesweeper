TARGET = minesweeper.exe
SRC_FILES = src\App.cpp src\main.cpp src\Game.cpp src\Tile.cpp

OBJECTS = $(SRC_FILES:.cpp=.o)

CFLAGS = -Wall -g -std=c++11 -static-libgcc -static-libstdc++ -static
SFML_FLAGS = -DSFML_STATIC

INC_PATH = .\lib\SFML\include
LIB_PATH = .\lib\SFML\lib

LIBS = sfml-graphics-s sfml-window-s sfml-system-s opengl32 freetype winmm gdi32


all: $(TARGET)

$(TARGET): $(OBJECTS)
	g++ $^ -o $@ $(CFLAGS) $(SFML_FLAGS) -I$(INC_PATH) -L$(LIB_PATH) $(addprefix -l,$(LIBS))

.cpp.o:
	g++ -o $@ -c $< $(CFLAGS) $(SFML_FLAGS) -I$(INC_PATH) -L$(LIB_PATH) $(addprefix -l,$(LIBS))

clean:
	del $(TARGET) $(OBJECTS)

# DEPENDENCIES
src/main.o: src/main.cpp src/definitions.h src/App.h
src/App.o: src/App.cpp src/App.h src/definitions.h src/Game.h
src/Game.o: src/Game.cpp src/Game.h src/definitions.h src/Tile.h
src/Tile.o: src/Tile.cpp src/Tile.h src/definitions.h

.PHONY: all clean