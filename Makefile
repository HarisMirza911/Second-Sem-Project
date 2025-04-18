# Makefile for Electrical Management System

CXX = g++
CXXFLAGS = -std=c++17 -Wall -g -O3

SRC_DIR = src
INCLUDE_DIR = include
BIN_DIR = bin

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:.cpp=.o)

TARGET = $(BIN_DIR)/electrical_management_system

INCLUDES = -I$(INCLUDE_DIR) -I/usr/include -I/usr/include/rapidjson

LIBS = -lboost_system -lboost_program_options

all: directories $(TARGET)

directories:
	@mkdir -p $(BIN_DIR)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)
	
run: all
	./$(TARGET)

.PHONY: all clean run directories
