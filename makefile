# 
#use the makefile to build the project,
#Third library is libsys.a to print the log while the project runing
#
SRC = src/readbmp.cpp
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


${TARGET}:$(SRC)
	${CXX} $< $(CC_FLAG) -I $(INCLUDE) -I $(SDK_INC) -L $(LIB) ${LLDFLAGS} -o ${TARGET}

test:

initdir:
	mkdir -p bin
	mkdir -p $(LIB)
	mkdir -p log
	
clean:
	rm -rf ${TARGET} bin log/*

.PHONY:test debug
