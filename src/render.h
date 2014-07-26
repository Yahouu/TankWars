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

// End Include Guard
#endif
