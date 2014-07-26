#include "moves.h"

void calculateMoves(int player, Tile **mapTiles, Tank tank)
{
	mapTiles[tank.x][tank.y].passable = 1;

	down(player, mapTiles, tank);
	up(player, mapTiles, tank);
	left(player, mapTiles, tank);
	right(player, mapTiles, tank);
	rightdown(player, mapTiles, tank);
	rightup(player, mapTiles, tank);
	leftdown(player, mapTiles, tank);
	leftup(player, mapTiles, tank);
}

void down(int player, Tile **mapTiles, Tank tank)
{
	for (int i=tank.y+1; i<HEIGHT; i++) {
		if (collision(player, i, tank.x, mapTiles, tank)) {
			break;
		}
	}
}
void up(int player, Tile **mapTiles, Tank tank)
{
	for (int i=tank.y-1; i>=0; i--) {
		if (collision(player, i, tank.x, mapTiles, tank)) {
			break;
		}
	}
}
void left(int player, Tile **mapTiles, Tank tank)
{
	for (int i=tank.x-1; i>=0; i--) {
		if (collision(player, tank.y, i, mapTiles, tank)) {
			break;
		}
	}
}
void right(int player, Tile **mapTiles, Tank tank)
{
	for (int i=tank.x+1; i<WIDTH; i++) {
		if (collision(player, tank.y, i, mapTiles, tank)) {
			break;
		}
	}
}
void rightdown(int player, Tile **mapTiles, Tank tank)
{
	for (int i=tank.y+1, j=tank.x+1; i<HEIGHT && j<WIDTH; i++, j++) {
		if (collision(player, i, j, mapTiles, tank)) {
			break;
		}
	}
}
void rightup(int player, Tile **mapTiles, Tank tank)
{
	for (int i=tank.y-1, j=tank.x+1; i>=0 && j<WIDTH; i--, j++) {
		if (collision(player, i, j, mapTiles, tank)) {
			break;
		}
	}
}
void leftdown(int player, Tile **mapTiles, Tank tank)
{
	for (int i=tank.y+1, j=tank.x-1; i<HEIGHT && j>=0; i++, j--) {
		if (collision(player, i, j, mapTiles, tank)) {
			break;
		}
	}
}
void leftup(int player, Tile **mapTiles, Tank tank)
{
	for (int i=tank.y-1, j=tank.x-1; i>=0 && j>=0; i--, j--) {
		if (collision(player, i, j, mapTiles, tank)) {
			break;
		}
	}
}

int collision(int player, int i, int j, Tile **mapTiles, Tank tank)
{
	if ((player == 1 && (mapTiles[i][j].tank == TANK1 || mapTiles[i][j].tank == TANK1_CMD))
		|| (player == 2 && (mapTiles[i][j].tank == TANK2 || mapTiles[i][j].tank == TANK2_CMD))
		|| (mapTiles[i][j].terrain == MINE)) {
		return 1;
	}

	else if (mapTiles[i][j].terrain == POLLUTED && (tank.type == TANK1 || tank.type == TANK2)) {
		return 1;
	}

	else if ((player == 1 && mapTiles[tank.y][tank.x].terrain == BASE2 && mapTiles[i][j].terrain == NORMAL)
		|| (player == 2 && mapTiles[tank.y][tank.x].terrain == BASE1 && mapTiles[i][j].terrain ==NORMAL)) {
		return 1;
	}

	else if ((player == 1 && (mapTiles[i][j].tank == TANK2 || mapTiles[i][j].tank == TANK2_CMD))
			|| (player == 2 && (mapTiles[i][j].tank == TANK1 || mapTiles[i][j].tank == TANK1_CMD))) {
		if (mapTiles[i][j].terrain == POLLUTED) {
			return 1;
		} else {
			mapTiles[i][j].passable = 1;
			return 1;
		}
	}

	else {
		mapTiles[i][j].passable = 1;
	}

	return 0;
}