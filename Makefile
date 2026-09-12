CXX := g++
CXXSTD := -std=c++17
CXXFLAGS := $(CXXSTD) -O2 -Wall -Wextra -Iinclude
SRC := $(wildcard src/*.cpp)
OBJ := $(SRC:src/%.cpp=build/%.o)
BIN := build/market_maker

.PHONY: all clean run

all: $(BIN)

$(BIN): $(OBJ) | build
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

build/%.o: src/%.cpp include/*.hpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build

clean:
	rm -rf build
