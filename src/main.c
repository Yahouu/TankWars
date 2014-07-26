/* Authors: SanicEggnog, TalosThoren
 * Filename: main.c
 * Date: July 2014
 * Description: Control file for TankWar2D
 */

#include "include.h"

int main( int argc, char *argv[] )
{
    if (initialiseSDL() == -1) {
    	exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    int player = ((rand()%2) + 1);

    Tile ** mapTiles = generateMap();
    Textures * gameTextures = loadTextures();

    launcher(&player, gameTextures, mapTiles);

    return 0;
}

void launcher( int *player, Textures *gameTextures, Tile **mapTiles )
{
	static size_t launches = 0;
	static Menu *menu = NULL;
	int choice = displayMenu(menu);

	if (choice == QUIT) {
		saveGame(*player, mapTiles);
		close(gameTextures, mapTiles, menu);
	} else {
		if (choice == LOAD && launches == 0) {
			// The game is already loaded after launches > 0
			loadGame(player, mapTiles);
		}

		if (choice == NEW && launches > 0) {
			// Start a new game in the middle of another
			mapTiles = generateMap();
		}

		SDL_RenderClear(renderer);
		generateTextures(*player, gameTextures, mapTiles);
		SDL_RenderPresent(renderer);

		moveTank(player, gameTextures, mapTiles);

		++launches;
		launcher(player, gameTextures, mapTiles);
	}
}
