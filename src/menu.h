// Include Guard
#ifndef MENU_H__
#define MENU_H__

// Include Files
#include "include.h"

typedef struct _menu {
	SDL_Texture *tex_title;
	SDL_Texture *tex_menu_bg;
	SDL_Texture *tex_load;
	SDL_Texture *tex_new;
	SDL_Texture *tex_quit;
	SDL_Texture *tex_music;
	Mix_Music *music;
} Menu;

int displayMenu(Menu *menu);
void loadMenu(Menu *menu);
int menuEvents(Menu *menu);
void writeText( const char*, SDL_Surface*, SDL_Texture*, TTF_Font*, SDL_Rect*, int, int );
void processMusic(Menu *menu);

#endif
