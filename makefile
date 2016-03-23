# 
#use the makefile to build the project,
#Third library is libsys.a to print the log while the project runing
#
SRC = src/bmp.cpp  
ISRC = include/*.cpp
LIB = ./sdk/lib/
OBJ =  
HEADERS = 
SDK_INC = ./sdk/inc
INCLUDE = ./include 
CC = gcc
CXX = g++
TARGET = bin/Imager
#BASEFLAGS = -s -O3
LLDFLAGS=-lsys -lpthread
CC_FLAG = -g -Wall 

all:initdir ${TARGET}


${TARGET}:$(SRC) $(ISRC)
	${CXX} $^ $(CC_FLAG) -I $(INCLUDE) -I $(SDK_INC) -L $(LIB) ${LLDFLAGS} -o ${TARGET}

test:
	./$(TARGET) source/test24.bmp 0 0 test.bmp
initdir:
	mkdir -p bin
	mkdir -p $(LIB)
	mkdir -p log
	
clean:
	rm -rf ${TARGET} bin log/* include/*~ src/*~

.PHONY:test debug
