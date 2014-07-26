/* Authors: SanicEggnog, TalosThoren
 * Filename: sdl.c
 * Date: July 2014
 * Description: Functions for initializing and interacting with SDL.
 * Implementation file. 
 */

// Include Files
#include "sdl.h"
#include "render.h"

// Initialize the SDL systems we'll be using
int initialiseSDL()
{
	if (SDL_Init(SDL_INIT_AUDIO) != 0) {
		fprintf(stderr, "Failed to initialise SDL (%s)\n", SDL_GetError());
		return -1;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
		fprintf(stderr, "Failed to initialise linear texture rendering");
	}

	window = SDL_CreateWindow("TankWar",SDL_WINDOWPOS_UNDEFINED,
										SDL_WINDOWPOS_UNDEFINED,
										DIMENSION,
										DIMENSION,
										SDL_WINDOW_SHOWN);

	if (window == NULL) {
		fprintf(stderr, "Failed to create the window: (%s)\n", SDL_GetError());
		return -1;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	if (renderer == NULL) {
		fprintf(stderr, "Failed to initialise renderer: (%s)\n", SDL_GetError());
		return -1;
	}

	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);

	int initted = IMG_Init(IMG_INIT_PNG);

	if ((initted&IMG_INIT_PNG) != IMG_INIT_PNG) {
		fprintf(stderr, "Failed to initialise PNG support (%s)\n", IMG_GetError());
		return -1;
	}

	initted = Mix_Init(MIX_INIT_MP3);
	if ((initted&MIX_INIT_MP3) != MIX_INIT_MP3) {
		fprintf(stdout, "Mix_Init: Failed to init MP3 support! (%s)\n", Mix_GetError());
    	return -1;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
		fprintf(stderr, "Failed to initialise SDL_mixer (%s)\n", Mix_GetError());
		return -1;
	}

	if (TTF_Init() == -1) {
		fprintf(stderr, "Failed to initialise SDL_TTF (%s)\n", TTF_GetError());
		return -1;
	}

	fprintf(stdout, "Finished initialisation.\n");
	
    return 0;
} 

void close(Textures *gameTextures, Tile **mapTiles, Menu *menu)
{
	freeTextures(gameTextures);
	freeMenu(menu);
	freeMap(mapTiles);

	SDL_DestroyRenderer(renderer);
	fprintf(stdout, "Freed renderer.\n");
	SDL_DestroyWindow(window);
	fprintf(stdout, "Freed window.\n");
	IMG_Quit();
	fprintf(stdout, "Freed IMG.\n");
	while(Mix_Init(0))
    	Mix_Quit();
    Mix_CloseAudio();
	fprintf(stdout, "Freed MIXER.\n");
	TTF_Quit();
	fprintf(stdout, "Freed TTF.\n");
	SDL_Quit();
}

void freeTextures(Textures *gameTextures)
{
    SDL_DestroyTexture(gameTextures->tex_terrain);
    SDL_DestroyTexture(gameTextures->tex_highlight);

    for (size_t i=0; i<2; i++) {
    	SDL_DestroyTexture(gameTextures->tex_player[i]);
    }

    for (size_t i=1; i<5; i++) {
    	SDL_DestroyTexture(gameTextures->tex_tank[i]);
    }

    free(gameTextures);
    fprintf(stdout, "Freed textures.\n");
}

void freeMap(Tile **mapTiles)
{
	for (size_t i = 0 ; i < HEIGHT ; i++) {
		free(mapTiles[i]);
	}

	free(mapTiles);
	fprintf(stdout, "Freed map array.\n");
}

void freeMenu(Menu *menu)
{
	SDL_DestroyTexture(menu->tex_title);
	SDL_DestroyTexture(menu->tex_menu_bg);
	SDL_DestroyTexture(menu->tex_load);
	SDL_DestroyTexture(menu->tex_new);
	SDL_DestroyTexture(menu->tex_quit);
	SDL_DestroyTexture(menu->tex_music);
	Mix_FreeMusic(menu->music);

	free(menu);
	fprintf(stdout, "Freed menu.\n");
}
