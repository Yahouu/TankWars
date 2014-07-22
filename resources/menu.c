#include "menu.h"

int displayMenu() 
{
	static Menu *menu = NULL;

	if (menu == NULL) {
		loadMenu(menu);
	}

	SDL_Rect position = {0,0,DIMENSION,DIMENSION};

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, menu->tex_menu_bg, NULL, &position);

    writeText(menu->text_title, &position, (DIMENSION/2 - 130), 100);
    writeText(menu->text_load, &position, 50, 200);
    writeText(menu->text_new, &position, 50, 250);
    writeText(menu->text_music, &position, 50, 300);
    writeText(menu->text_quit, &position, 50, 350);

    SDL_RenderPresent(renderer);

    return menuEvents(menu);
}

void loadMenu(Menu *menu)
{
	TTF_Font *font = TTF_OpenFont("FORCED_SQUARE.ttf", 65);
	SDL_Color black = {0,0,0,0};

	menu->music = Mix_LoadMUS("cccp.mp3");
	if (music == NULL) {
		fprintf(stderr, "Failed to load the music (%s)\n", Mix_GetError());
	}

	menu->text_title = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, "TankWar", black));

	font = TTF_OpenFont("FORCED_SQUARE.ttf", 35);
	menu->text_load = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, "Enter - continue a game", black));
	menu->text_new = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, "N        - New Game", black));
	menu->text_quit = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, "Escape - Quit the game", black));
	menu->text_music = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, "M        - Play/Pause the music", black));

	TTF_CloseFont(font);

	menu->tex_menu_bg = IMG_LoadTexture( renderer, "./img/menu_bg.png");
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
	int choice = 0;

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