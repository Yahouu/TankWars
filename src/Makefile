# Author: TalosThoren
# Filename: Makefile
# Date: July 2014
# Description: Makefile for TankWar2D.

ODIR=../obj
BDIR=../bin

HEADERS := map.h render.h sdl.h endgame.h gameplay.h menu.h moves.h saves.h
_OBJECTS := $(HEADERS:.h=.o)
OBJECTS = $(patsubst %,$(ODIR)/%,$(_OBJECTS))
LIBS := -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_mixer -lSDL2_ttf -luser32 -lgdi32 -lwinmm -ldxguid
INCLUDES := -I. -I$(ODIR)
CFLAGS = -Wall -g -std=c11
#NODEBUG = -O2 -std=c11 -mwindows
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
	${CC} $^ main.c $(INCLUDES) $(LIBS) $(CFLAGS) -o $(BDIR)/$@

$(ODIR)/%.o: %.c %.h include.h build
	${CC} $< -c $(CFLAGS) -o $@

.PHONY: default clean check dist distcheck install rebuild uninstall
