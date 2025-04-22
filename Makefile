# Makefile for Electrical Management System

CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -O3

SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin
OBJ_DIR = obj

# Main application sources (excluding server.cpp)
MAIN_SRCS = $(filter-out $(SRC_DIR)/server.cpp, $(wildcard $(SRC_DIR)/*.cpp))
MAIN_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(MAIN_SRCS))

# Server sources
SERVER_SRCS = $(SRC_DIR)/server.cpp
SERVER_OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SERVER_SRCS))

MAIN_TARGET = $(BIN_DIR)/electrical_management_system
SERVER_TARGET = $(BIN_DIR)/server

INCLUDES = -I$(INCLUDE_DIR) -I/usr/include -I/usr/include/rapidjson -I$(INCLUDE_DIR)/RestApi

LIBS = -lboost_system -lboost_program_options -lmysqlcppconn -lssl -lcrypto -lpthread

all: directories $(MAIN_TARGET) $(SERVER_TARGET)

directories:
	@if [ ! -d "$(BIN_DIR)" ]; then mkdir -p $(BIN_DIR); fi
	@if [ ! -d "$(OBJ_DIR)" ]; then mkdir -p $(OBJ_DIR); fi

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(MAIN_TARGET): $(MAIN_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

$(SERVER_TARGET): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(OBJ_DIR)/*.o $(MAIN_TARGET) $(SERVER_TARGET)
	
run-main: $(MAIN_TARGET)
	./$(MAIN_TARGET)

run-server: $(SERVER_TARGET)
	./$(SERVER_TARGET)

.PHONY: all clean run-main run-server directories
