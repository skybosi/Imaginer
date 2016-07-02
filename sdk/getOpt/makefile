# 
#use the makefile to build the project,
#Third library is libsys.a to print the log while the project runing
#
SRC = *.cpp  
ISRC = 
LIB =
OBJ =  
HEADERS = 
SDK_INC =
INCLUDE = ./include 
CC = gcc
CXX = g++
TARGET = bin/testopt
#BASEFLAGS = -s -O3
LLDFLAGS=
CC_FLAG = -g -Wall 

all:initdir ${TARGET}


${TARGET}:$(SRC) $(ISRC)
	${CXX} $^ $(CC_FLAG) -I $(INCLUDE) -I $(SDK_INC) -L $(LIB) ${LLDFLAGS} -o ${TARGET}

test:
	./$(TARGET) -T true -R 1 2 33 -c -H Blue -h -asdf
initdir:
	mkdir -p bin
clean:
	rm -rf ${TARGET} *~

.PHONY:test debug
