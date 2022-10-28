#CC specifies which compiler we're using
CC = g++ 

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -g -Wall -ansi -std=c++11 

#This is the target that compiles our executable

SRC_DIR := src
OBJ_NAME := TP1_G14

all :
	$(CC) $(wildcard $(SRC_DIR)/*.cc) $(COMPILER_FLAGS) -o $(OBJ_NAME)