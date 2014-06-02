# Amazing Project Makefile
# Team Raboso

CC = gcc
SRC_DIR = src
CFLAGS = -Wall -pedantic -std=c11 -ggdb -lm

all: amazing_project

amazing_project: $(SRC_DIR)/AMStartup.o $(SRC_DIR)/graphics.o $(SRC_DIR)/avatar.o $(SRC_DIR)/maze.o
	$(CC) $(CFLAGS) $(SRC_DIR)/AMStartup.o $(SRC_DIR)/avatar.o $(SRC_DIR)/maze.o $(SRC_DIR)/graphics.o -o amazing_project -lpthread -lncurses

$(SRC_DIR)/AMStartup.o: $(SRC_DIR)/AMStartup.c $(SRC_DIR)/avatar.c $(SRC_DIR)/avatar.h $(SRC_DIR)/maze.c $(SRC_DIR)/maze.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/AMStartup.c -o $(SRC_DIR)/AMStartup.o -lpthread

$(SRC_DIR)/avatar.o: $(SRC_DIR)/avatar.c $(SRC_DIR)/avatar.h $(SRC_DIR)/maze.h $(SRC_DIR)/graphics.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/avatar.c -o $(SRC_DIR)/avatar.o

$(SRC_DIR)/maze.o: $(SRC_DIR)/maze.c $(SRC_DIR)/maze.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/maze.c -o $(SRC_DIR)/maze.o

$(SRC_DIR)/graphics.o: $(SRC_DIR)/graphics.c $(SRC_DIR)/graphics.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/graphics.c -o $(SRC_DIR)/graphics.o -lncurses


clean:
	rm -f amazing_project $(SRC_DIR)/*.o $(SRC_DIR)/*~ $(SRC_DIR)/*# $(SRC_DIR)/core*  *~
