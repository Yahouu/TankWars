#TankWar

##Objectives

TankWar is a modern Polish game that is played on a square board of 11x11 squares. Each player manages his army of 14 tanks - 13 normal and the tank commander, who is strong enough to move through the center field contaminated by chemical weapons. Initially, tank commanders are located in the lower right corner (from the point of view of each player) and normal tanks occupy the remaining area of ​​the base (blue squares for the white player and green squares for the black player).

The goal is to occupy the enemy base with as much tanks as possible. The player with the most points is the winner. The number of points corresponds to the events of the game and will be described in the final section. 

![alt text](http://sanic.violates.me/i/nvSlaf.jpg "Tabletop")

##Goals

####Tank Movement

Each player can move a tank per turn. Tanks can move several squares vertically, horizontally or diagonally with these conditions:
  - A tank cannot move on the black squares (mines).
  - A tank can not jump over an ally or enemy tank.
  - Only the tank commander can go on the purple squares (or cross them) in the center of the map (polluted area).
  - Tank commanders cannot capture each other in the polluted purple zone at the center.
  - Tanks that reach the enemy base (green or blue squares) cannot leave the base until the end of the game.

####How to capture enemy tanks

The game ends when a player moves all his remaining tanks to the enemy base . The final score is calculated and the player with the most points wins. The game is drawn if each player has the same points.

Points are counted as follows :

 - 3 points if the tank commander is on the starting tile of the opponent tank commander.
 - 2 points for a tank commander who is in any tile of the enemy base.
 - 2 points for a regular tank that is in any tile of the enemy base .
 - 1 point for any tank (ordinary or commander) that remains on the battlefield and isn't destroyed.

![alt text](http://i.imgur.com/WvEJlta.jpg?1 "Counting Points")

The white player has 2 points for the commander tank in the enemy base and 2 x 1 point for the two tanks on the battlegrounds, the result is 4 points total. The black player obtains 3 points for the tank commander on the starting tile of the white commmander tank and 2x2 points for his two tanks in the enemy base, the result is 7 points. Black wins.


##Version

1.2

##Compilation

 - Install SDL2, SDL_Image, SDL_TTF and SDL_mixer
 - Use the proper compilation flags. For example, on Windows:
 ```
 gcc /path/to/main.c -lSDL2_image -lSDL2_mixer -lSDL2_ttf -lSDL2main -lSDL2 -luser32 -lgdi32 -lwinmm -ldxguid -mwindows -o /path/to/tankwar.exe
```
   Under Linux (compiling with gcc works, but the game segfaults):
 ```
 g++ -I/usr/include -lSDL2 -lSDL2main -lSDL2_image -lSDL2_mixer -lSDL2_ttf main.c
 ```
 - On Windows, copy the proper dll for SDL2 et the libraries in the project directory.


##Technologies 

* [C]
* [SDL2] - 2D and 3D video game development library

[C]:https://en.wikipedia.org/wiki/C_%28programming_language%29
[SDL2]:http://www.libsdl.org/
