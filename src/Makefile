# Author: TalosThoren
# Filename: Makefile
# Date: July 2014
# Description: Makefile for TankWar2D.

ODIR=../obj
BDIR=../bin

HEADERS := map.h render.h sdl.h endgame.h gameplay.h menu.h moves.h saves.h
_OBJECTS := $(HEADERS:.h=.o)
OBJECTS = $(patsubst %,$(ODIR)/%,$(_OBJECTS))
LIBS := -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -luser32 -lgdi32 -lwinmm -ldxguid -mwindows
CFLAGS = -Wall -O2 -std=c11
CC = gcc

default: build $(BDIR)/TankWar2D

build:
	@test -d $(ODIR) || mkdir $(ODIR)
	@test -d $(BDIR) || mkdir $(BDIR)

check: TankWar2D
	./TankWar2D

clean:
	rm -f ./obj/*
	rm -f ./bin/*TankWar2D
	rm -f ./*~
	rm -f ./*.swp

rebuild: clean default

$(BDIR)/TankWar2D: ${OBJECTS}
	${CC} -I. $(CFLAGS) ${LIBS} $^ -o $@

$(ODIR)/main.o: include.h main.c
	${CC} -I. -c $(CFLAGS) ${LIBS} $< -o $@

$(ODIR)/%.o: %.c %.h include.h
	${CC} -I. -c $(CFLAGS) ${LIBS} $< -o $@

.PHONY: default clean check dist distcheck install uninstall
