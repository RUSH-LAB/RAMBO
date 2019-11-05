# declare variables

CXX      := g++
CXXFLAGS := -Wall -fopenmp -O2
INCLUDE  := -Iinclude/
BUILD    := ./build
TARGET   := program

SRC      :=                      \
	 $(wildcard src/MurmurHash3.cpp)   \
	 $(wildcard src/Rambo_construction.cpp) \
	 $(wildcard src/intbitArray.cpp) \
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
