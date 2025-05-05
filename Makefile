# Makefile for Electrical Management System

CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -O3

SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin
OBJ_DIR = obj

# Main application sources
MAIN_SRCS = $(wildcard $(SRC_DIR)/*.cpp)
MAIN_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(MAIN_SRCS))

MAIN_TARGET = $(BIN_DIR)/electrical_management_system

INCLUDES = -I$(INCLUDE_DIR) -I/usr/include -I/usr/include/rapidjson -I$(INCLUDE_DIR)/RestApi -I$(INCLUDE_DIR)/Monitoring

LIBS = -lboost_system -lboost_program_options -lmysqlcppconn -lssl -lcrypto -lpthread -lprometheus-cpp-core -lprometheus-cpp-pull

all: directories $(MAIN_TARGET)

directories:
	@if [ ! -d "$(BIN_DIR)" ]; then mkdir -p $(BIN_DIR); fi
	@if [ ! -d "$(OBJ_DIR)" ]; then mkdir -p $(OBJ_DIR); fi

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(MAIN_TARGET): $(MAIN_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(OBJ_DIR)/*.o $(MAIN_TARGET)
	
run-main: $(MAIN_TARGET)
	./$(MAIN_TARGET)

.PHONY: all clean run-main directories
