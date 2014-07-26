/* Authors: SanicEggnog, TalosThoren
 * Filename: render.c
 * Date: July 2014
 * Description: Functions for rendering to the screen. Implementation file.
 */

// Include Files
#include "render.h"


Textures * loadTextures()
{
	Textures *gameTextures = (Textures*)malloc(sizeof(Textures));
	TTF_Font *font = TTF_OpenFont("../resources/FORCED_SQUARE.ttf", 25);
	SDL_Color black = {0, 0, 0};

	gameTextures->tex_terrain = IMG_LoadTexture( renderer, "../resources/img/map.png");
	gameTextures->tex_tank[EMPTY] = NULL;
	gameTextures->tex_tank[TANK1] = IMG_LoadTexture( renderer, "../resources/img/tanka.png");
	gameTextures->tex_tank[TANK2] = IMG_LoadTexture( renderer, "../resources/img/tankb.png");
	gameTextures->tex_tank[TANK1_CMD] = IMG_LoadTexture( renderer, "../resources/img/commanda.png");
	gameTextures->tex_tank[TANK2_CMD] = IMG_LoadTexture( renderer, "../resources/img/commandb.png");
	gameTextures->tex_highlight = IMG_LoadTexture( renderer, "../resources/img/highlight2.png");
	gameTextures->tex_player[0] = SDL_CreateTextureFromSurface( renderer, TTF_RenderText_Blended(font, "Player 1", black) );
	gameTextures->tex_player[1] = SDL_CreateTextureFromSurface( renderer, TTF_RenderText_Blended(font, "Player 2", black) );

	TTF_CloseFont(font);

	return gameTextures;
}

// Generate textures for a tile
void generateTextures( int player, Textures *gameTextures, Tile **mapTiles )
{
	SDL_Rect rect_tank = {0,0,TILE,TILE};
	SDL_Rect rect_map = {0,0,DIMENSION,DIMENSION};
	SDL_Rect rect_text = {0,0,0,0};

	SDL_RenderCopy(renderer, gameTextures->tex_terrain, NULL, &rect_map);

	for (size_t i=0; i < WIDTH; i++) {
		for (size_t j=0; j < HEIGHT; j++) {
			rect_tank.y = i * TILE;
			rect_tank.x = j * TILE;

			switch(mapTiles[i][j].tank) {
				case 1:
					SDL_RenderCopy(renderer, gameTextures->tex_tank[TANK1], NULL, &rect_tank);
					break;

				case 2:
					SDL_RenderCopy(renderer, gameTextures->tex_tank[TANK2], NULL, &rect_tank);
					break;

				case 3:
					SDL_RenderCopy(renderer, gameTextures->tex_tank[TANK1_CMD], NULL, &rect_tank);
					break;

				case 4:
					SDL_RenderCopy(renderer, gameTextures->tex_tank[TANK2_CMD], NULL, &rect_tank);
					break;
			}

			if (mapTiles[i][j].passable == 1) {
				SDL_RenderCopy(renderer, gameTextures->tex_highlight, NULL, &rect_tank);
			}
		}
	}

	SDL_QueryTexture(gameTextures->tex_player[1], NULL, NULL, &rect_text.w, &rect_text.h);
	rect_text.x = DIMENSION - rect_text.w * 1.2;
	rect_text.y = 0.2 * rect_text.h;

	if (player == 1) {
		SDL_RenderCopy(renderer, gameTextures->tex_player[0], NULL, &rect_text);
	} else {
		SDL_RenderCopy(renderer, gameTextures->tex_player[1], NULL, &rect_text);
	}
}
