CXX = g++
CXXFLAGS = -std=c++17 -Wall -O2 $(shell pkg-config --cflags sdl2)
LDFLAGS = $(shell pkg-config --libs sdl2)

SOURCES = src/Main.cpp src/Chip8.cpp src/Platform.cpp
TARGET = chip8

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)

.PHONY: clean