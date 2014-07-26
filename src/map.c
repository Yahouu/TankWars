/* Authors: SanicEggnog, TalosThoren
 * Filename: map.c
 * Date: July 2014
 * Description: Functions for initializing and interacting with the map.
 * Implementation File.
 */

// Include Files
#include "map.h"

Tile ** generateMap()
{
	Tile **mapTile = (Tile**)malloc(sizeof(Tile*) * HEIGHT);
	for (size_t i = 0 ; i < HEIGHT ; i++) {
		mapTile[i] = (Tile*)malloc(sizeof(Tile) * WIDTH);
	}

	char *line = getMap();

    for (size_t i = 0 ; i < WIDTH ; i++)
    {
        for (size_t j = 0 ; j < HEIGHT ; j++)
        {
            switch (line[(i * WIDTH) + j])
            {
                case '0':
                    mapTile[j][i].terrain = NORMAL;
                    mapTile[j][i].tank = EMPTY;
                    break;
                case '1':
                    mapTile[j][i].terrain = MINE;
                    mapTile[j][i].tank = EMPTY;
                    break;
                case '2':
                    mapTile[j][i].terrain = POLLUTED;
                    mapTile[j][i].tank = EMPTY;
                    break;
                case '3':
                    mapTile[j][i].terrain = BASE1;
                    mapTile[j][i].tank = TANK1;
                    break;
                case '4':
                    mapTile[j][i].terrain = BASE2;
                    mapTile[j][i].tank = TANK2;
                    break;
            }

            mapTile[i][j].passable = 0;
        }
    }

    mapTile[0][0].tank = TANK1_CMD;
    mapTile[10][10].tank = TANK2_CMD;

    free(line);
    return mapTile;
}

char * getMap()
{
	FILE *map = NULL;
    char *line = (char*)malloc(sizeof(char) * WIDTH * HEIGHT + 1);

	map = fopen("../resources/map.txt", "r");

	if (map == NULL) {
		fprintf(stderr, "map.txt is empty, cannot generate the map");
		exit(EXIT_FAILURE);
	}

	fgets(line, WIDTH * HEIGHT + 1, map);
	fclose(map);

	return line;
}
