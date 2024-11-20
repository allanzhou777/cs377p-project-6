# Makefile for PageRank application

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -Wall -Wextra -std=c++11

# Output executable
TARGET = main

# Source file
SRC = main.cpp

# DIMACS files
DIMACS_FILES = wiki.dimacs rmat15.dimacs road-NY.dimacs

# Default rule
all: $(TARGET)

# Rule to compile the main program
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Rules to run the program with different DIMACS files
run: $(TARGET)
	@echo "Running with wiki.dimacs"
	./$(TARGET) ./dimacs/wiki.dimacs
	@echo "Running with rmat15.dimacs"
	./$(TARGET) ./dimacs/rmat15.dimacs
	@echo "Running with road-NY.dimacs"
	./$(TARGET) ./dimacs/road-NY.dimacs

# Shortcuts to run with specific DIMACS files
wiki: $(TARGET)
	./$(TARGET) ./dimacs/wiki.dimacs

rmat15: $(TARGET)
	./$(TARGET) ./dimacs/rmat15.dimacs

road-NY: $(TARGET)
	./$(TARGET) ./dimacs/road-NY.dimacs

# Clean up generated files
clean:
	rm -f $(TARGET)
