#include "menu.h"
#include "render.h"

int displayMenu(Menu *menu) 
{
	SDL_Rect position = {0,0,DIMENSION,DIMENSION};

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, menu->tex_menu_bg, NULL, &position);

    writeText(menu->tex_title, &position, (DIMENSION/2 - 130), 100);
    writeText(menu->tex_load, &position, 50, 200);
    writeText(menu->tex_new, &position, 50, 250);
    writeText(menu->tex_music, &position, 50, 300);
    writeText(menu->tex_quit, &position, 50, 350);

    SDL_RenderPresent(renderer);

    return menuEvents(menu);
}

void loadMenu(Menu *menu)
{
	TTF_Font *font = TTF_OpenFont("../resources/FORCED_SQUARE.ttf", 65);
	SDL_Color black = {0,0,0};

	menu->music = Mix_LoadMUS("../resources/cccp.mp3");
	if (menu->music == NULL) {
		fprintf(stderr, "Failed to load the music (%s)\n", Mix_GetError());
	}

	fprintf(stdout, "Menu: Opened font and music file.\n");

	menu->tex_title = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, "TankWar", black));

	font = TTF_OpenFont("../resources/FORCED_SQUARE.ttf", 35);
	menu->tex_load = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, "Enter - continue a game", black));
	menu->tex_new = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, "N        - New Game", black));
	menu->tex_quit = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, "Escape - Quit the game", black));
	menu->tex_music = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, "M        - Play/Pause the music", black));

	TTF_CloseFont(font);

	menu->tex_menu_bg = IMG_LoadTexture( renderer, "../resources/img/menu_bg.png");
}

void writeText(SDL_Texture *text, SDL_Rect *position, int x, int y)
{
	SDL_QueryTexture(text, NULL, NULL, &position->w, &position->h);
	position->x = x;
	position->y = y;
	SDL_RenderCopy(renderer, text, NULL, position);
}

int menuEvents(Menu *menu)
{
	SDL_Event event;

	while (1) {
		SDL_WaitEvent(&event);
		switch(event.type) {
			case SDL_QUIT:
				return QUIT;
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym){
					case SDLK_ESCAPE:
						return QUIT;
					case SDLK_RETURN:
						return LOAD;
					case SDLK_n:
						return NEW;
					case SDLK_m:
						processMusic(menu);
						break;
				}
				break;
		}
	}

	fprintf(stderr, "Something went very wrong");
	return QUIT;
}

void processMusic(Menu *menu)
{
	if (!Mix_PlayingMusic()) {
		Mix_PlayMusic(menu->music, -1);
	} else {
		if (Mix_PausedMusic()) {
			Mix_ResumeMusic();
		} else {
			Mix_PauseMusic();
		}
	}
}
