TARGET = minesweeper
SRC_FILES = src/App.cpp src/main.cpp src/Game.cpp src/Tile.cpp

CXX = g++
CFLAGS = -Wall -g -std=c++11

OBJECTS = $(SRC_FILES:.cpp=.o)

ifeq ($(OS),Windows_NT)
	TARGET := $(TARGET).exe
	DEL = del

	INC_PATH = Z:/CSCI200/include/
	LIB_PATH = Z:/CSCI200/lib/

	ARCH = 
else
	DEL = rm

	INC_PATH = /usr/local/include/
	LIB_PATH = /usr/local/lib/

	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		CCFLAGS += -D LINUX
	endif
	ifeq ($(UNAME_S),Darwin)
		TARGET_MAKEFILE = Makefile.osx
		CCFLAGS += -D OSX
	endif

	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		ARCH = 
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		ARCH = 
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		ARCH = -arch x86_64
	endif
endif

LIBS = -lsfml-graphics -lsfml-window -lsfml-system

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(ARCH) -o $@ $^ -L$(LIB_PATH) $(LIBS)

.cpp.o:
	$(CXX) $(CFLAGS) $(ARCH) -o $@ -c $< -I$(INC_PATH)

clean:
	$(DEL) $(TARGET) $(OBJECTS)

# DEPENDENCIES
src/main.o: src/main.cpp src/definitions.h src/App.h
src/App.o: src/App.cpp src/App.h src/definitions.h src/Game.h
src/Game.o: src/Game.cpp src/Game.h src/definitions.h src/Tile.h
src/Tile.o: src/Tile.cpp src/Tile.h src/definitions.h

.PHONY: all clean