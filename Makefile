# Makefile for Electrical Management System

CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -O3

SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))

TARGET = $(BIN_DIR)/electrical_management_system

INCLUDES = -I$(INCLUDE_DIR) -I/usr/include -I/usr/include/rapidjson

LIBS = -lboost_system -lboost_program_options -lmysqlcppconn

all: directories $(TARGET)

directories:
	@if [ ! -d "$(BIN_DIR)" ]; then mkdir -p $(BIN_DIR); fi
	@if [ ! -d "$(OBJ_DIR)" ]; then mkdir -p $(OBJ_DIR); fi
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)
	
run: all
	./$(TARGET)

.PHONY: all clean run directories
