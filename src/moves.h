// Include Guard
#ifndef MOVES_H__
#define MOVES_H__

// Include Files
#include "include.h"

void calculateMoves(int player, Tile **mapTiles, Tank tank);
void down(int player, Tile **mapTiles, Tank tank);
void up(int player, Tile **mapTiles, Tank tank);
void left(int player, Tile **mapTiles, Tank tank);
void right(int player, Tile **mapTiles, Tank tank);
void rightdown(int player, Tile **mapTiles, Tank tank);
void rightup(int player, Tile **mapTiles, Tank tank);
void leftdown(int player, Tile **mapTiles, Tank tank);
void leftup(int player, Tile **mapTiles, Tank tank);
int collision(int player, int i, int j, Tile **mapTiles, Tank tank);

#endif