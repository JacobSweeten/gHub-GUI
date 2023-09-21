OUT	= ./bin/ghub-gui
CC	 = gcc
FLAGS	 = -g -c -Wall
LFLAGS	 = -lusb-1.0 -lGLX `pkg-config --cflags --libs gtk+-3.0`
INCLUDE = -I./include `pkg-config --cflags --libs gtk+-3.0`

all: obj/main.o obj/device.o obj/window.o
	$(CC) -g $^ -o $(OUT) $(LFLAGS)

obj/main.o: source/main.c
	$(CC) $(FLAGS) $(INCLUDE) $^ -o $@

obj/device.o: source/device.c
	$(CC) $(FLAGS) $(INCLUDE) $^ -o $@

obj/window.o: source/window.c
	$(CC) $(FLAGS) $(INCLUDE) $^ -o $@


clean:
	rm -f obj/*
	rm -f bin/*

