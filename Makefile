# declare variables

CXX      := g++-9
CXXFLAGS := -Wall -fopenmp -std=c++17 -O1 -pedantic -Wall -Wextra -Wimplicit-fallthrough=0 -mavx -march=native -lstdc++fs
INCLUDE  := -Iinclude/ -Iext/CLI11/include -Iext/spdlog/include -Iext/bit-algorithms/include -Iext/bit-algorithms/ext/bit/
BUILD    := ./build
TARGET   := program

SRC      :=                      \
	 $(wildcard src/MurmurHash3.cpp)   \
	 $(wildcard src/Rambo_construction.cpp) \
	 $(wildcard src/bitArray.cpp) \
     $(wildcard src/MyBloom.cpp)   \
	 $(wildcard src/utils.cpp)   \
	 $(wildcard src/main.cpp)   \


OBJECTS := $(SRC:%.cpp=$(BUILD)/%.o)

all: clean build $(BUILD)/$(TARGET)

$(BUILD)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(OBJECTS) -o $(BUILD)/$(TARGET)

$(BUILD)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -o $@ -c $<

.PHONY: all build clean debug release

build:
	@mkdir -p $(BUILD)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

clean:
	-@rm -rvf $(BUILD)/*
