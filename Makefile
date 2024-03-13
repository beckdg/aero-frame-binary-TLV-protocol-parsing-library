CXX      := g++
CXXFLAGS := -Wall -Wextra -Werror -O2 -std=c++17 -Iinclude
TARGET   := aeroframe_parser
SOURCES  := $(filter-out src/main.cpp,$(wildcard src/*.cpp)) src/main.cpp
LIB_SOURCES := $(filter-out src/main.cpp,$(wildcard src/*.cpp))

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(LIB_SOURCES) src/main.cpp
	$(CXX) $(CXXFLAGS) $(LIB_SOURCES) src/main.cpp -o $@

test:
	cmake -B build && cmake --build build && ctest --test-dir build --output-on-failure

clean:
	rm -f $(TARGET)
	rm -rf build
