# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall

# Target name
TARGET = fp_simulator

# Source file
SRCS = fp_simulator.cpp

# Default target
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# Clean build
clean:
	rm -f $(TARGET)
