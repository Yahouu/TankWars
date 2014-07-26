#include "saves.h"

void loadGame(int *player, Tile **mapTiles)
{
	FILE *save = NULL;
	char line[WIDTH * HEIGHT + 2] = {0};

	save = fopen("../resources/save.txt", "r");
	if (save == NULL) {
		fprintf(stderr, "Failed to load the game\n");
	}

	fgets(line, WIDTH * HEIGHT + 2, save);
	fclose(save);

	for (size_t i=0; i<WIDTH; i++) {
		for (size_t j=0; j<HEIGHT; j++) {
			switch (line[(i * WIDTH) + j]) {
				case '0':
					mapTiles[j][i].tank = EMPTY;
					break;
				case '1':
					mapTiles[j][i].tank = TANK1;
					break;
				case '2':
					mapTiles[j][i].tank = TANK2;
					break;
				case '3':
					mapTiles[j][i].tank = TANK1_CMD;
					break;
				case '4':
					mapTiles[j][i].tank = TANK2_CMD;
					break;
			}
		}
	}

	*player = line[WIDTH * HEIGHT] - '0';
}

void saveGame(int player, Tile **mapTiles)
{
	FILE *save = fopen("../resources/save.txt", "w");
	if (save == NULL) {
		fprintf(stderr, "Failed to save the game\n");
	}

	for (size_t i=0; i<WIDTH; i++) {
		for (size_t j=0; j<HEIGHT; j++) {
			fprintf(save, "%d", mapTiles[j][i].tank);
		}
	}

	fprintf(save, "%d", player);
	fclose(save);
}