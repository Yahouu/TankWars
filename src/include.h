/* Authors: SanicEggnog, TalosThoren
 * Filename: include.h
 * Date: July 2014
 * Description: Consolodates common include files and global settings.
 */

// Inclusion Guard
#ifndef INCLUDE_H_
#define INCLUDE_H_

// Standard Library Includes
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

// SDL2 Includes
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

// Global Settings
//const int MAP_WIDTH = 11;
//const int MAP_HEIGHT = 11;

#define TILE            65                  // Tile dimension
#define DIMTANK         64                  // Tank dimension
#define WIDTH           11                  // Number of blocks horizontally
#define HEIGHT          11                  // Number of blocks vertically
#define DIMENSION       716                 // Window dimension

// Game elements
enum list_terrain {NORMAL, MINE, POLLUTED, BASE1, BASE2};
enum list_tank {EMPTY, TANK1, TANK2, TANK1_CMD, TANK2_CMD};
enum list_moves {INVALID, VALID, ENEMY};
enum menu_choice {QUIT = -1, LOAD = 1, NEW = 2};

// This structure contains all we need to know about each tile on the map
typedef struct _tile {
	int terrain;
	int tank;
    int passable;
    int destructable;
} Tile;

// Our textures
typedef struct _textures {
    SDL_Texture *tex_terrain;
    SDL_Texture *tex_highlight;
    SDL_Texture *tex_tank[5];
    SDL_Texture *tex_player[2];
} Textures;

// This structure contains informations on the tanks we're moving
typedef struct _tank {
	int x,y;
	int type;
} Tank;

extern SDL_Window *window;
extern SDL_Renderer *renderer;

/* Local Includes
#include "menu.h"
#include "sdl.h"
#include "render.h"
#include "map.h"
#include "endgame.h"
#include "gameplay.h"
#include "moves.h"
#include "saves.h"*/

void launcher( int *player, Textures *gameTextures, Tile **mapTiles );

// END of Inclusion Guard
#endif
