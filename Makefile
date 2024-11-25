# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17

# Program versions
SOURCES = bf-0.cpp bf-1.cpp bf-2.cpp bf-3.cpp bf-4.cpp
EXECUTABLES = $(SOURCES:.cpp=)

# Graph files
GRAPHS = ./dimacs/rmat15.dimacs ./dimacs/road-NY.dimacs

# Default target
all: $(EXECUTABLES)

# Build each executable with -lpthread after source file
bf-0: bf-0.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lpthread

bf-1: bf-1.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lpthread

bf-2: bf-2.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lpthread

bf-3: bf-3.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lpthread

bf-4: bf-4.cpp
	$(CXX) $(CXXFLAGS) -o $@ $< -lpthread

# Run targets
run: $(EXECUTABLES)
	@echo "Specify the VERSION (0-4) and GRAPH (rmat15 or road-NY) to run, e.g., 'make run VERSION=3 GRAPH=rmat15'"
	@echo "Valid graphs: rmat15, road-NY"

run-specific:
	@if [ -z "$(VERSION)" ] || [ -z "$(GRAPH)" ]; then \
	    echo "Usage: make run-specific VERSION=<0-4> GRAPH=<rmat15|road-NY>"; \
	else \
	    if [ "$(GRAPH)" = "rmat15" ]; then \
	        ./bf-$(VERSION) ./dimacs/rmat15.dimacs > temp.txt; \
	    elif [ "$(GRAPH)" = "road-NY" ]; then \
	        ./bf-$(VERSION) ./dimacs/road-NY.dimacs > temp.txt; \
	    else \
	        echo "Error: Invalid graph. Choose 'rmat15' or 'road-NY'."; \
	    fi; \
	fi

# Clean build files
clean:
	rm -f $(EXECUTABLES) temp.txt
