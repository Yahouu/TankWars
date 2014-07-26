/* Authors: SanicEggnog, TalosThoren
 * Filename: sdl.h
 * Date: July 2014
 * Description: Functions for initializing and interacting with SDL. Declaration
 * file.
 */

// Include Guard
#ifndef SDL_H__
#define SDL_H__

// Include Files
#include "include.h"
#include "menu.h"

// Function Prototypes
int initialiseSDL();
void close(Textures *gameTextures, Tile **mapTiles, Menu *menu);
void freeTextures(Textures *gameTextures);
void freeMap(Tile **mapTiles);
void freeMenu(Menu *menu);

// End Include Guard
#endif
