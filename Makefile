# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -O3 -Wall -std=c++11

# Source directory
SRC_DIR = src

# Targets
all: server client

server: $(SRC_DIR)/server.cpp
	$(CXX) $(CXXFLAGS) $^ -o server -pthread

client: $(SRC_DIR)/client.cpp
	$(CXX) $(CXXFLAGS) $^ -o client -pthread

clean:
	rm -f server client