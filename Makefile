CXX = g++-14 -std=c++20
CXXFLAGS = -Wall -g  
SRC = $(wildcard src/*.cc)   # list of all .cc files in the current directory
INCLUDE = -Iinclude/
LIBRARY = -L$(wildcard lib/*)
LINKERFLAGS = -lSDL2
BUILD_DIR = build
EXEC = chip8

all:
	${CXX} ${CXXFLAGS} ${INCLUDE} ${LIBRARY} ${SRC} -o ${BUILD_DIR}/${EXEC} ${LINKERFLAGS}
