# Author: TalosThoren
# Filename: Makefile
# Date: July 2014
# Description: Makefile for TankWar2D.

ODIR=../obj
BDIR=../bin

HEADERS := map.h render.h sdl.h endgame.h gameplay.h menu.h moves.h saves.h
_OBJECTS := $(HEADERS:.h=.o)
OBJECTS = $(patsubst %,$(ODIR)/%,$(_OBJECTS))
#LIBS := -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -luser32 -lgdi32 -lwinmm -ldxguid -mwindows
LIBS := -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf
INCLUDES := -I. -I/usr/includ/SDL2 -I$(ODIR)
#CFLAGS = -Wall -O2 -std=c11
CFLAGS = -O2 -std=c11
CC = gcc

default: TankWar2D

build:
	@test -d $(ODIR) || mkdir $(ODIR)
	@test -d $(BDIR) || mkdir $(BDIR)

check: TankWar2D
	./TankWar2D

clean:
	rm -rf ../obj/
	rm -rf ../bin/
	rm -f ./*~
	rm -f ./*.swp

rebuild: clean default

TankWar2D: ${OBJECTS} include.h
	${CC} main.c $(INCLUDES) $(LIBS) $(CFLAGS) $^ -o $(ODIR)/$@

$(ODIR)/%.o: %.c %.h include.h build
	${CC} -c $(CFLAGS) $< -o $@

.PHONY: default clean check dist distcheck install rebuild uninstall
