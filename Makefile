OUT	= ./bin/ghub-gui
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lusb-1.0 -lX11
INCLUDE = ./include

all: obj/main.o obj/device.o
	$(CC) -g $^ -o $(OUT) $(LFLAGS)

obj/main.o: source/main.c
	$(CC) -I$(INCLUDE) $(FLAGS) $^ -o $@

obj/device.o: source/device.c
	$(CC) -I$(INCLUDE) $(FLAGS) $^ -o $@


clean:
	rm -f obj/*
	rm -f bin/*

