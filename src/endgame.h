// Include Guard
#ifndef ENDGAME_H__
#define ENDGAME_H__

// Include Files
#include "include.h"
#include "map.h"

typedef struct _score {
	size_t count;
	size_t stuck;
	size_t command;
	size_t total;
} Score;

typedef struct _endscreen {
	SDL_Texture *tex_game_over;
	SDL_Texture *tex_score1;
	SDL_Texture *tex_score2;
	SDL_Texture *tex_background;
} EndScreen;

int checkEndGame(Textures *gameTextures, Tile **mapTiles);
void countTanks(Score *score, Tile **mapTiles, int tank, int base, int origin);
void countPoints(Tile **mapTiles, Score *score, int tank, int origin);
int endGame(Textures *gameTextures, size_t score1, size_t score2);
void loadScreen(EndScreen *endscreen, int score1, int score2);

#endif
