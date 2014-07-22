# Author: TalosThoren
# Filename: Makefile
# Date: July 2014
# Description: Makefile for TankWar2D.

HEADERS := map.h render.h sdl.h
OBJECTS := $(HEADERS:.h=.o)
LIBS := -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
LIB_DIR := /usr/include/SDL2/
CC := /usr/bin/gcc

default: TankWar2D

check: TankWar2D
	./TankWar2D

clean:
	rm -f ./*.o
	rm -f ./*TankWar2D
	rm -f ./*~
	rm -f ./*.swp

TankWar2D: ${OBJECTS} include.h main.c
	${CC} -I.  -I${LIB_DIR} ${LIBS} -o TankWar2D main.c ${OBJECTS}

map.o: include.h map.h map.c
	${CC} -c -I. -I${LIB_DIR} ${LIBS} map.c

render.o: include.h render.h render.c
	${CC} -c -I. -I${LIB_DIR} ${LIBS} render.c

sdl.o: include.h sdl.h sdl.c
	${CC} -c -I. -I${LIB_DIR} ${LIBS} sdl.c

.PHONY: default clean check dist distcheck install uninstall
