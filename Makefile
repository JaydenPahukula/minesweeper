TARGET = minesweeper
SRC_FILES = src/App.cpp src/main.cpp src/Game.cpp src/Tile.cpp src/Menu.cpp src/Timer.cpp src/Confetti.cpp

OBJECTS = $(SRC_FILES:.cpp=.o)

FLAGS = -Wall -g -std=c++11 -static-libgcc -static-libstdc++ -static -DSFML_STATIC

INC_PATH = ./lib/SFML/include
LIB_PATH = ./lib/SFML/lib

LIBS = sfml-graphics-s sfml-window-s sfml-system-s

ifeq ($(shell echo "Windows"), "Windows")
    TARGET := $(TARGET).exe
    SRC_FILES := $(subst /,\,$(SRC_FILES)) # replace slashes with backslashes
	DEL = del
	FLAGS += -mwindows
	LIBS += opengl32 freetype winmm gdi32
else
	DEL = rm -f
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	g++ $^ -o $@ $(FLAGS) -I$(INC_PATH) -L$(LIB_PATH) $(addprefix -l,$(LIBS))

.cpp.o:
	g++ -o $@ -c $< $(FLAGS) -I$(INC_PATH) -L$(LIB_PATH) $(addprefix -l,$(LIBS))

clean:
	$(DEL) $(TARGET) $(OBJECTS)

# DEPENDENCIES
src/main.o: src/main.cpp src/definitions.h src/App.h
src/App.o: src/App.cpp src/App.h src/definitions.h src/Game.h src/Menu.h src/Timer.h src/Confetti.h
src/Menu.o: src/Menu.cpp src/definitions.h
src/Game.o: src/Game.cpp src/Game.h src/definitions.h src/Tile.h
src/Tile.o: src/Tile.cpp src/Tile.h src/definitions.h
src/Timer.o: src/Timer.cpp src/Timer.h
src/Confetti.o: src/Confetti.cpp src/Confetti.h src/definitions.h

.PHONY: all clean