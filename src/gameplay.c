#include "gameplay.h"

int moveTank(int *player, Textures *gameTextures, Tile **mapTiles)
{
	Tank tank = {0,0,0};
	int movement = 0;

	SDL_Event event;
	SDL_Rect rect = {0,0,TILE,TILE};

	while (1) {
		SDL_WaitEvent(&event);

		if (event.key.keysym.sym == SDLK_ESCAPE || event.type == SDL_QUIT) {
			return QUIT;
		}

		if (event.type == SDL_MOUSEBUTTONDOWN) {
			SDL_GetMouseState(&rect.x, &rect.y);

			if (!movement) {
				tank.x = rect.x/TILE;
				tank.y = rect.y/TILE;
				movement = chooseTank(player, mapTiles, &tank);
			}

			else if (mapTiles[rect.y/TILE][rect.x/TILE].passable != INVALID) {
				if (dropTank(player, mapTiles, gameTextures, &tank, rect) == QUIT)
				{
					return QUIT;
				}

				movement = 0;
			}
		}

		if (movement) {
			if (event.type == SDL_MOUSEMOTION) {
				rect.x = event.motion.x;
				rect.y = event.motion.y;
			}

			SDL_RenderClear(renderer);
			generateTextures(*player, gameTextures, mapTiles);
			SDL_RenderCopy(renderer, gameTextures->tex_tank[tank.type], NULL, &rect);
			SDL_RenderPresent(renderer);
		}
	}

	fprintf(stderr, "Something went terribly wrong");
	return QUIT;
}

int chooseTank(int *player, Tile **mapTiles, Tank *tank)
{
	tank->type = mapTiles[tank->y][tank->x].tank;

	if ((*player == 1 && (tank->type == TANK1 || tank->type == TANK1_CMD))
		|| (*player == 2 && (tank->type == TANK2 || tank->type == TANK2_CMD))) {
		calculateMoves(*player, mapTiles, *tank);
		return 1;
	}

	return 0;
}

int dropTank(int *player, Tile **mapTiles, Textures *gameTextures, Tank *tank, SDL_Rect rect)
{
	if (tank->type == *player) {
		mapTiles[rect.y][rect.x].tank = *player;
	} else {
		mapTiles[rect.y][rect.x].tank = *player + 2;
	}

	for (size_t i=0; i<HEIGHT; i++) {
		for (size_t j=0; j<WIDTH; j++) {
			mapTiles[j][i].passable = 0;
		}
	}

	if (*player == 1) {
		*player = 2;
	} else *player = 1;

	SDL_RenderClear(renderer);
	generateTextures(*player, gameTextures, mapTiles);
	SDL_RenderPresent(renderer);

	return checkEndGame(gameTextures, mapTiles);
}