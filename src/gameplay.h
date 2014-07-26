// Include Guard
#ifndef GAMEPLAY_H__
#define GAMEPLAY_H__

// Include Files
#include "include.h"
#include "render.h"
#include "moves.h"
#include "endgame.h"

/* might not be needed
typedef struct _coordinates {
	int x;
	int y;
	SDL_Rect rect;
} Coordinates;*/

int moveTank(int *player, Textures *gameTextures, Tile **mapTiles);
int chooseTank(int *player, Tile **mapTiles, Tank *tank);
int dropTank(int *player, Tile **mapTiles, Textures *gameTextures, Tank *tank, SDL_Rect rect);

#endif