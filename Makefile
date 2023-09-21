OUT	= ./bin/ghub-gui
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lusb-1.0 -lX11
INCLUDE = ./include

all: obj/main.o obj/mouselist.o
	$(CC) -g $^ -o $(OUT) $(LFLAGS)

obj/main.o: source/main.c
	$(CC) -I$(INCLUDE) $(FLAGS) $^ -o $@

obj/mouselist.o: source/mouselist.c
	$(CC) -I$(INCLUDE) $(FLAGS) $^ -o $@


clean:
	rm -f obj/*
	rm -f bin/*

