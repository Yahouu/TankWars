/* Authors: SanicEggnog, TalosThoren
 * Filename: render.h
 * Date: July 2014
 * Description: Functions for rendering to the screen. Declaration file.
 */

// Include Guard
#ifndef RENDER_H__
#define RENDER_H__

// Include Files
#include "include.h"

// Function Prototypes

Textures * loadTextures();

void generateTextures( int player, Textures *gameTextures, Tile **mapTiles );

/* TODO: Refactor
void writeText( const char*, SDL_Surface*, SDL_Texture*, TTF_Font*, SDL_Rect*, int, int );
*/

// End Include Guard
#endif
