/** 1st year C programming project
 *  TankWar
 *  Uses SDL2
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"



/************************************************************************************************
 *                                CONSTANTS AND DEFINES                                         *
 ************************************************************************************************/

// Defining our tabletop

#define TILE                65                  // Tile dimensions in pixels
#define DIMTANK             64                  // Tank sprite dimensions
#define WIDTH               11                  // Number of blocks horizontally
#define HEIGTH              11                  // Number of blocks vertically
#define DIMENSION           716                 // Map dimension in pixels

// Game elements

enum list_terrain {NORMAL, MINE, POLLUTED, BASE1, BASE2};
enum list_tank {EMPTY, TANK1, TANK2, TANK1_CMD, TANK2_CMD};
enum list_moves {INVALID, VALID, ENEMY};

// This structure contains all we need to know about each tile on the map

typedef struct struct_tile{
	int terrain;
	int tank;
	int allowed;	// tells us if the selected tank can move to this tile
}struct_tile;

// Our textures

typedef struct struct_textures{
    SDL_Texture *tex_terrain;
    SDL_Texture *tex_highlight;
    SDL_Texture *tex_tank[5];
    SDL_Texture *tex_player[2];
    SDL_Texture *tex_menu_bg;
}struct_textures;

// This structure contains informations on the tanks we're moving

typedef struct struct_tank{
	int x,y;
	int type;
}struct_tank;



/************************************************************************************************
 *                                          PROTOTYPES                                          *
 ************************************************************************************************/

int initialisation(struct_tile arr_S_tile[][WIDTH]);
int generateMap(struct_tile arr_S_tile[][WIDTH]);
void loadTextures( struct_textures *S_textures );
void launch( int *player, struct_textures *S_textures, struct_tile arr_S_tile[][WIDTH] );
int menu(struct_textures *S_textures);
void writeText(const char* text, SDL_Surface *surf_text, SDL_Texture *tex_text, TTF_Font *font, SDL_Rect *position, int x, int y);
void loadGame( int *player, struct_tile arr_S_tile[][WIDTH] );
void generateTextures( int player, struct_textures *S_textures, struct_tile arr_S_tile[][WIDTH] );
void moveTank(int *player, struct_textures *S_textures, struct_tile arr_S_tile[][WIDTH] );
int chooseTank(int* player, struct_tile arr_S_tile[][WIDTH], struct_tank* S_tank, SDL_Rect* rect_movement, int x_tmp, int y_tmp);
int dropTank(int* player, struct_tile arr_S_tile[][WIDTH], struct_textures* S_textures, struct_tank* S_tank, int x_tmp, int y_tmp);
void allowedMoves( int player, struct_tile arr_S_tile[][WIDTH], struct_tank S_tank );
int collision(int player, int i, int j, struct_tile arr_S_tile[][WIDTH], struct_tank S_tank);
int checkEndGame( struct_textures *S_textures, struct_tile arr_S_tile[][WIDTH] );
void endGame( struct_textures *S_textures, int score1, int score2);
void save( int player, struct_tile arr_S_tile[][WIDTH] );
void close( struct_textures *S_textures );

// Fenêtre de jeu et renderer

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
Mix_Music *music = NULL;

/************************************************************************************************
 *                                          FUNCTIONS                                           *
 ************************************************************************************************/

 int main(int argc, char *argv[])
{
	struct_tile arr_S_tile[HEIGTH][WIDTH];
	struct_textures S_textures;

	// Check everything initialised properly

    if (!initialisation(arr_S_tile))
    {
        fprintf(stdout,"Couldn't initialise SDL (%s)\n",SDL_GetError());
        return -1;
    }

    // Choose randomly which player starts

    srand(time(NULL));
    int player = ((rand()%2)+1);

    loadTextures( &S_textures );
    launch( &player, &S_textures, arr_S_tile );

    return 0;
}

int initialisation(struct_tile arr_S_tile[][WIDTH])
{
    /** This function initialises everything needed to launch the game.
     *  We initialise SDL, SDL_Image, SDL_TTF, SDL_Mixer, the renderer and we create the game window.
     *  Then we generate the tabletop from the txt file.
     *  If we have a problem, it's logged in the console.
     *
     *  Returns 1 if everything initialised properly, 0 otherwise.
     */

    int init=1;

    // Initialise SDL et check if the library launched properly.

    if (SDL_Init( SDL_INIT_EMPTYO | SDL_INIT_AUDIO) < 0 )
    {
        fprintf(stderr,"Couldn't initialise SDL (%s)\n",SDL_GetError());
        init=0;
    }

    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
        fprintf(stderr, "Couldn't initialise linear texture filtering" );
    }

    // Create our window

    window = SDL_CreateWindow("TankWar", SDL_WINDOWPOS_UNDEFINED,    // Window position on the desktop
                                         SDL_WINDOWPOS_UNDEFINED,    // Here we don't care
                                         DIMENSION,                  // Dimensions
                                         DIMENSION,
                                         SDL_WINDOW_SHOWN);          // Show the windows, hidden by default

    if( window == NULL )
    {
        fprintf( stderr, "Failed to create the window: %s\n", SDL_GetError() );
        init = 0;
    }

    // Create the renderer

    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );

    if( window == NULL )
    {
        fprintf( stderr, "Failed to create the renderer: SDL Error: %s\n", SDL_GetError() );
        init = 0;
    }

    // Default background colour is white

    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    // SDL_Image initialisation

    int flags=IMG_INIT_JPG|IMG_INIT_PNG;
    int initted=IMG_Init(flags);
    if((initted&flags) != flags) {
        fprintf(stderr,"Couldn't initialise png and jpg support using IMG_Init (%s)\n",IMG_GetError());
        init=0;
    }

    // SDL_mixer initialisation
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
    {
        fprintf(stderr, "Couldn't initialise SDL_mixer. Error: %s\n", Mix_GetError() );
        init = 0;
    }

    // SDL_TTF initialisation
    if(TTF_Init() == -1)
    {
        fprintf(stderr, "Couldn't initialise SDL_TTF : %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    // Generate the map
    init = generateMap(arr_S_tile);

    return init;

}

int generateMap(struct_tile arr_S_tile[][WIDTH])
{
    /** This function generates our map at the start of the game, then places the tanks.
     *  We also reset all the allowed moves.
     *
     *  Returns 1 if executed until the end.
     */

    FILE* map = NULL;
    char line[WIDTH * HEIGTH + 1] = {0};
    int i = 0, j = 0;

    map = fopen("map.txt", "r");
    if (map == NULL)
        return 0;

    fgets(line, WIDTH * HEIGTH + 1, map);

    for (i = 0 ; i < WIDTH ; i++)
    {
        for (j = 0 ; j < HEIGTH ; j++)
        {
            switch (line[(i * WIDTH) + j])
            {
                case '0':
                    arr_S_tile[j][i].terrain = NORMAL;
                    arr_S_tile[j][i].tank = EMPTY;
                    break;
                case '1':
                    arr_S_tile[j][i].terrain = MINE;
                    arr_S_tile[j][i].tank = EMPTY;
                    break;
                case '2':
                    arr_S_tile[j][i].terrain = POLLUTED;
                    arr_S_tile[j][i].tank = EMPTY;
                    break;
                case '3':
                    arr_S_tile[j][i].terrain = BASE1;
                    arr_S_tile[j][i].tank = TANK1;
                    break;
                case '4':
                    arr_S_tile[j][i].terrain = BASE2;
                    arr_S_tile[j][i].tank = TANK2;
                    break;
            }
        }
    }

    fclose(map);
    arr_S_tile[0][0].tank = TANK1_CMD;
    arr_S_tile[10][10].tank = TANK2_CMD;

    for (i = 0; i<HEIGTH ; i++)
    {
        for (j=0 ; j<WIDTH ; j++)
        {
            arr_S_tile[i][j].allowed = 0;
        }
    }

    return 1;
}

void loadTextures( struct_textures *S_textures )
{
    /**
     *  This function loads all our texture in memory.
     */

    TTF_Font *font = NULL;
    font = TTF_OpenFont("FORCED_SQUARE.ttf", 25);
    SDL_Color black = {0, 0, 0};

    S_textures->tex_terrain = IMG_LoadTexture( renderer, "./img/map.png");
    S_textures->tex_tank[TANK1] = IMG_LoadTexture( renderer, "./img/tanka.png");
    S_textures->tex_tank[TANK2] = IMG_LoadTexture( renderer, "./img/tankb.png");
    S_textures->tex_tank[TANK1_CMD] = IMG_LoadTexture( renderer, "./img/commanda.png");
    S_textures->tex_tank[TANK2_CMD] = IMG_LoadTexture( renderer, "./img/commandb.png");
    S_textures->tex_highlight = IMG_LoadTexture( renderer, "./img/highlight2.png");
    S_textures->tex_menu_bg = IMG_LoadTexture( renderer, "./img/menu_bg.png");
    S_textures->tex_player[0] = SDL_CreateTextureFromSurface( renderer, TTF_RenderText_Blended(font, "Player 1", black) );
    S_textures->tex_player[1] = SDL_CreateTextureFromSurface( renderer, TTF_RenderText_Blended(font, "Player 2", black) );

    TTF_CloseFont(font);
}

void launch( int *player, struct_textures *S_textures, struct_tile arr_S_tile[][WIDTH] )
{
    /** This function launches our game per se.
     *  Depending on the player's choice in the menu, we do different things :
     *
     *  If the player quits, we save the game and quit.
     *  If the player opens the game and continueds his game, we load it.
     *  If the player starts a new game in the middle of another, we regenerate the terrain.
     *
     *  When the player leaves the main game loop, we call this function recursively again.
     */

    static int i = 0;  // We use this counter to know if we just launched the game or not.
    int choice = menu(S_textures);

    if (choice == -1)
    {
        save( *player, arr_S_tile );
        close( S_textures );
    }

    else
    {
        if (choice == 1 && i == 0)
        {
            loadGame( player, arr_S_tile );
        }

        if (choice == 2 && i != 0)
        {
            /*  Lets us start a new game without reopening the game
                But also we don't have to reinitialise the entire game everytime we go back to the menu.
                We don't need to reload the game if the player continueds his game, it remains loaded. */
            generateMap( arr_S_tile );
        }

        SDL_RenderClear( renderer );
        generateTextures( *player, S_textures, arr_S_tile );
        SDL_RenderPresent( renderer );

        // Infinite loop
        moveTank( player, S_textures, arr_S_tile );

        ++i;
        launch(player, S_textures, arr_S_tile);
    }
}

int menu(struct_textures *S_textures)
{
    /** This function creates our menu.
     *
     *  The player has four options:
     *  - continued a game
     *  - Start a new game
     *  - Start or stop the music
     *  - Quit the game
     */

    SDL_Texture *tex_titre = NULL, *tex_option1 = NULL, *tex_option2 = NULL, *tex_option3 = NULL, *tex_option4 = NULL;
    SDL_Surface *surf_text = NULL;
    TTF_Font *font = NULL;
    SDL_Rect position = {0,0,DIMENSION,DIMENSION};
    SDL_Event event;

    music = Mix_LoadMUS( "cccp.mp3" );
    if( music == NULL )
	{
		fprintf( stderr, "Failed to load the music! Error SDL_mixer: %s\n", Mix_GetError() );
	}

    int continued = 0;

    SDL_RenderClear( renderer );
    SDL_RenderCopy( renderer, S_textures->tex_menu_bg, NULL, &position );

    font = TTF_OpenFont("FORCED_SQUARE.ttf", 65);
    writeText("TankWar", surf_text, tex_option1, font, &position, (DIMENSION/2 - 130), 100);

    font = TTF_OpenFont("FORCED_SQUARE.ttf", 35);
    writeText("Enter - continued a game", surf_text, tex_option1, font, &position, 50, 200);
    writeText("N        - New Game", surf_text, tex_option1, font, &position, 50, 250);
    writeText("M        - Play/Pause the music", surf_text, tex_option1, font, &position, 50, 300);
    writeText("Escape - Quit the game", surf_text, tex_option1, font, &position, 50, 350);

    SDL_FreeSurface( surf_text );
    SDL_RenderPresent( renderer );

    while (continued == 0)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continued = -1;
                break;
            case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: // Wants to stop the game
                        continued = -1;
                        break;
                    case SDLK_RETURN: // Load the game
                        continued = 1;
                        break;
                    case SDLK_n:    // Start a new game
                        continued = 2;
                        break;
                    case SDLK_m:
                        //If the music isn't playing...
                        if( Mix_PlayingMusic() == 0 )
                        {
                            //... start the music
                            Mix_PlayMusic( music, -1 );
                        }
                        //If it's already playing...
                        else
                        {
                            //... but paused ...
                            if( Mix_PausedMusic() == 1 )
                            {
                                //... resume it.
                                Mix_ResumeMusic();
                            }
                            //... and it's playing...
                            else
                            {
                                //... pause it.
                                Mix_PauseMusic();
                            }
                        }
                        break;
                }
                break;
        }
    }

    TTF_CloseFont(font);
    SDL_DestroyTexture(tex_titre);
    SDL_DestroyTexture(tex_option1);
    SDL_DestroyTexture(tex_option2);
    SDL_DestroyTexture(tex_option3);
    SDL_DestroyTexture(tex_option4);

    return continued;
}

void writeText(const char* text, SDL_Surface* surf_text, SDL_Texture* tex_text, TTF_Font* font, SDL_Rect* position, int x, int y)
{
    /**
     *  This function lets us write text in a set position.
     */

    SDL_Color black = {0, 0, 0};
    surf_text = TTF_RenderText_Blended(font, text, black);
    tex_text = SDL_CreateTextureFromSurface( renderer, surf_text );

    SDL_QueryTexture(tex_text, NULL, NULL, &position->w, &position->h);
    position->x = x;
    position->y = y;
    SDL_RenderCopy( renderer, tex_text, NULL, position );
}

void loadGame( int *player, struct_tile arr_S_tile[][WIDTH])
{
    /**
     *  This function loads the saved game if the player wants to continued playing it after launching the game.
     */

    FILE* save = NULL;
    char line[WIDTH * HEIGTH + 2] = {0};    // +2 to read the saved player and the EOF character
    int i = 0, j = 0;

    save = fopen("save.txt", "r");
    if (save == NULL)
        fprintf(stderr, "Failed to load the game.");

    fgets(line, WIDTH * HEIGTH + 2, save);

    for (i = 0 ; i < WIDTH ; i++)
    {
        for (j = 0 ; j < HEIGTH ; j++)
        {
            switch (line[(i * WIDTH) + j])
            {
                case '0':
                    arr_S_tile[j][i].tank = EMPTY;
                    break;
                case '1':
                    arr_S_tile[j][i].tank = TANK1;
                    break;
                case '2':
                    arr_S_tile[j][i].tank = TANK2;
                    break;
                case '3':
                    arr_S_tile[j][i].tank = TANK1_CMD;
                    break;
                case '4':
                    arr_S_tile[j][i].tank = TANK2_CMD;
                    break;
            }
        }
    }

    *player = line[WIDTH*HEIGTH] - '0';    // Converts our tank char to int

    fclose(save);
}

void generateTextures( int player, struct_textures *S_textures, struct_tile arr_S_tile[][WIDTH] )
{
    /**
     *  This function renders our textures into the game window.
     */

    SDL_Rect rect_tank = {0,0,TILE,TILE};
    SDL_Rect rect_map = {0,0,DIMENSION,DIMENSION};
    SDL_Rect rect_text = {0,0,0,0};

    int i,j;

    SDL_RenderCopy( renderer, S_textures->tex_terrain, NULL, &rect_map );

    for (i = 0 ; i < WIDTH ; i++)
    {
        for (j = 0 ; j < HEIGTH ; j++)
        {
            rect_tank.y = i * TILE;
            rect_tank.x = j * TILE;
            switch(arr_S_tile[i][j].tank)
            {
                case 1:
                    SDL_RenderCopy( renderer, S_textures->tex_tank[TANK1], NULL, &rect_tank );
                    break;

                case 2:
                    SDL_RenderCopy( renderer, S_textures->tex_tank[TANK2], NULL, &rect_tank );
                    break;

                case 3:
                    SDL_RenderCopy( renderer, S_textures->tex_tank[TANK1_CMD], NULL, &rect_tank );
                    break;

                case 4:
                    SDL_RenderCopy( renderer, S_textures->tex_tank[TANK2_CMD], NULL, &rect_tank );
                    break;
            }

            if(arr_S_tile[i][j].allowed == 1)
            {
                SDL_RenderCopy( renderer, S_textures->tex_highlight, NULL, &rect_tank);
            }
        }
    }

    SDL_QueryTexture(S_textures->tex_player[1], NULL, NULL, &rect_text.w, &rect_text.h);
    rect_text.x = DIMENSION - rect_text.w*1.2;
    rect_text.y = 0.2*rect_text.h;
    if(player == 1)
    {
        SDL_RenderCopy( renderer, S_textures->tex_player[0], NULL, &rect_text );
    }

    else SDL_RenderCopy( renderer, S_textures->tex_player[1], NULL, &rect_text );
}

void moveTank( int *player, struct_textures *S_textures, struct_tile arr_S_tile[][WIDTH] )
{
    /** This function lets us move our tanks turn by turn,
     *  As long as we don't leave the main loop voluntarily or until the game ends.
     */

    int continued = 1, mouvement = 0;
    int x_tmp=0, y_tmp=0;
    struct_tank S_tank;
    S_tank.x=0;
    S_tank.y=0;
    S_tank.type=0;

    SDL_Event event;    // Used for event management
    SDL_Rect rect_movement = {0,0,TILE,TILE};  // Used when moving the tank so that the sprite follows the mouse

    while (continued)
    {
        SDL_WaitEvent(&event);

        // If we close the window or press Esc, we leave the loop

        if (event.key.keysym.sym == SDLK_ESCAPE)
        {
            continued = 0;
        }

        // If we click in the window

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            SDL_GetMouseState( &x_tmp, &y_tmp );    // Get the click coordinates
            if (!mouvement)
            {
                mouvement = chooseTank(player, arr_S_tile, &S_tank, &rect_movement, x_tmp, y_tmp);
            }

            else if (arr_S_tile[y_tmp/TILE][x_tmp/TILE].allowed != INVALID)
            {
                continued = dropTank(player, arr_S_tile, S_textures, &S_tank, x_tmp, y_tmp);
                mouvement = 0;
            }
        }

        if (mouvement == 1)
        {
            // When we've selected a tank and we move the mouse, the tank follows our moves.
            if (event.type == SDL_MOUSEMOTION)
            {
                rect_movement.x = event.motion.x;
                rect_movement.y = event.motion.y;
            }

            SDL_RenderClear( renderer );
            generateTextures( *player, S_textures, arr_S_tile );
            SDL_RenderCopy( renderer, S_textures->tex_tank[S_tank.type], NULL, &rect_movement );
            SDL_RenderPresent( renderer );
        }
    }
}

int chooseTank(int* player, struct_tile arr_S_tile[][WIDTH], struct_tank* S_tank, SDL_Rect* rect_movement, int x_tmp, int y_tmp)
{
    /**
     *  This function lets us move a tank if the player clicks on one of his tanks.
     *  If the rights conditions are met, we generate the allowed moves for this tank.
     */

    S_tank->x = x_tmp/TILE;
    S_tank->y = y_tmp/TILE;
    rect_movement->x = x_tmp;
    rect_movement->y = y_tmp;
    S_tank->type = arr_S_tile[S_tank->y][S_tank->x].tank;

    if ((*player == 1 && (S_tank->type == TANK1 || S_tank->type == TANK1_CMD))
        || (*player == 2 && (S_tank->type == TANK2 || S_tank->type == TANK2_CMD)))
    {
        allowedMoves( *player, arr_S_tile, *S_tank);
        return 1;
    }

    return 0;
}

int dropTank(int* player, struct_tile arr_S_tile[][WIDTH], struct_textures* S_textures, struct_tank* S_tank, int x_tmp, int y_tmp)
{
    /**
     *  This function lets us drop the tanks the player is moving when he clicks on a tile.
     *  We check which tank we need to place and if the tile is allowed.
     *  Then we check if the game needs to end.
     */

    int i, j;

    /* We place either a normal tank or a commander tank */
    if(S_tank->type == *player) // We use the fact that if player == 1, then we have TANK1 (==1)
    {
        arr_S_tile[y_tmp / TILE][x_tmp / TILE].tank = *player;
    }

    else arr_S_tile[y_tmp / TILE][x_tmp / TILE].tank = *player+2;   // TANKx_CMD = TANKx + 2

    /* Reinitialise the allowed moves array */
    for (i = 0; i<HEIGTH ; i++)
    {
        for (j=0 ; j<WIDTH ; j++)
        {
            arr_S_tile[i][j].allowed = 0;
        }
    }

    if (*player == 1) *player = 2;
    else *player = 1;

    /* Regenerate the textures */
    SDL_RenderClear( renderer );
    generateTextures( *player, S_textures, arr_S_tile );
    SDL_RenderPresent( renderer );

    /* Check if the game is ending */
    return checkEndGame(S_textures, arr_S_tile);
}

void allowedMoves( int player, struct_tile arr_S_tile[][WIDTH], struct_tank S_tank )
{
    /** This function studies case by case the possible moves in each of the 8 possible directions.
     *  Collision detection inside each loop happens in collision().
     *
     *  Each tile in the tabletop has a binary value if we can move there or not. 
     */

    int i,j;
    arr_S_tile[S_tank.y][S_tank.x].allowed = 1;    // We can place the tank back down on the starting position

    /* DOWNWARDS */

    for (i=S_tank.y+1; i<HEIGTH; i++)
    {
        if (collision( player, i, S_tank.x, arr_S_tile, S_tank) == 0)
        {
            break;
        }
    }

    /* UPWARDS*/

    for (i=S_tank.y-1; i>=0; i--)
    {
        if (collision( player, i, S_tank.x, arr_S_tile, S_tank) == 0)
        {
            break;
        }
    }

    /* LEFT */

    for (i=S_tank.x-1; i>=0; i--)
    {
        if (collision( player, S_tank.y, i, arr_S_tile, S_tank) == 0)
        {
            break;
        }
    }

    /* RIGHT */

    for (i=S_tank.x+1; i<WIDTH; i++)
    {
        if (collision( player, S_tank.y, i, arr_S_tile, S_tank) == 0)
        {
            break;
        }
    }

    /* RIGHT + DOWN */

        for (i=S_tank.y+1, j=S_tank.x+1; i<HEIGTH && j<WIDTH; i++,j++)
        {
            if (collision( player, i, j, arr_S_tile, S_tank) == 0)
            {
                break;
            }
        }


    /* RIGHT + UP */

        for (i=S_tank.y-1, j=S_tank.x+1; i>=0 && j<WIDTH; i--,j++)
        {
            if (collision( player,i, j, arr_S_tile, S_tank) == 0)
            {
                break;
            }
        }

    /* LEFT + DOWN */

        for (i=S_tank.y+1, j=S_tank.x-1; i<HEIGTH && j>=0; i++,j--)
        {
            if (collision( player, i, j, arr_S_tile, S_tank) == 0)
            {
                break;
            }
        }

    /* LEFT + UP */

        for (i=S_tank.y-1, j=S_tank.x-1; i>=0 && j>=0; i--,j--)
        {
            if (collision( player, i, j, arr_S_tile, S_tank) == 0)
            {
                break;
            }
        }

        arr_S_tile[S_tank.y][S_tank.x].tank = EMPTY;

}

int collision(int player, int i, int j, struct_tile arr_S_tile[][WIDTH], struct_tank S_tank)
{
    /** This function checks the collisions in one direction, given by allowedMoves().
     *
     *  We check the player, the starting position of the tank (y,x) and the tank type.
     *  i and j are the counters in each loop of allowedMoves().
     *  One of the counters can be replaced by x or y depending on the direction.
     *
     *  Returns 0 if we are stopped by a forbidden tile so we can exit the loop.
     */

    /* If we meet an allied tank or a mine, we stop */
    if ((player == 1 && (arr_S_tile[i][j].tank == TANK1 || arr_S_tile[i][j].tank == TANK1_CMD))
        || (player == 2 && (arr_S_tile[i][j].tank == TANK2 || arr_S_tile[i][j].tank == TANK2_CMD))
        || ( arr_S_tile[i][j].terrain == MINE ))
    {
        return 0;
    }

    /* If we meet a polluted tile with a normal tank, we stop */
    else if (arr_S_tile[i][j].terrain == POLLUTED && (S_tank.type == TANK1 || S_tank.type == TANK2))
    {
        return 0;
    }

    /* If we are in the enemy base, we can't leave it */
    else if ((player == 1 && arr_S_tile[S_tank.y][S_tank.x].terrain == BASE2 && arr_S_tile[i][j].terrain == NORMAL)
             || (player == 2 && arr_S_tile[S_tank.y][S_tank.x].terrain == BASE1 && arr_S_tile[i][j].terrain == NORMAL))
    {
        return 0;
    }

    /* If we meet an enemy tank, we can capture it then stop, except if it's in a polluted tile */
    else if ((player == 1 && (arr_S_tile[i][j].tank == TANK2 || arr_S_tile[i][j].tank == TANK2_CMD))
             || (player == 2 && (arr_S_tile[i][j].tank == TANK1 || arr_S_tile[i][j].tank == TANK1_CMD)))
    {
        if (arr_S_tile[i][j].terrain == POLLUTED)
        {
            return 0;
        }

        else
        {
           arr_S_tile[i][j].allowed = 1;
            return 0;
        }
    }

    /* Otherwise, we can move */
    else
    {
        arr_S_tile[i][j].allowed = 1;
    }

    return 1;
}

int checkEndGame( struct_textures* S_textures, struct_tile arr_S_tile[][WIDTH] )
{
    /** This function checks if the game has ended or not, then counts points if necessary.
     *  The game ends if :
     *
     *  - One player has lost all his tanks.
     *  - One player's tanks are all in the enemy base and the commander tank on the enemy commander's starting position
     */

    int i, j, count1 = 0, count2 = 0, frozen1 = 0, frozen2 = 0, cmd1 = 0, cmd2 = 0, fin = 0, score1 = 0, score2 = 0;

    /* We count the number of tanks left and those which can't leave the base anymore */

    for ( i=0 ; i < WIDTH ; i++)
    {
        for ( j=0 ; j < HEIGTH ; j++)
        {
            if (arr_S_tile[j][i].tank == TANK1 || arr_S_tile[j][i].tank == TANK1_CMD)
            {
                ++count1;
                if (arr_S_tile[j][i].tank == TANK1 && arr_S_tile[j][i].terrain == BASE2)
                {
                    ++frozen1;
                }

                /* We want to know if the commander tank is in the enemy base but not on the enemy commander's starting position */
                if ((i!=0 || j!=0) && arr_S_tile[j][i].tank == TANK1_CMD && arr_S_tile[j][i].terrain == BASE2)
                {
                    ++cmd1;
                }
            }

            else if (arr_S_tile[j][i].tank == TANK2 || arr_S_tile[j][i].tank == TANK2_CMD)
            {
                ++count2;
                if (arr_S_tile[j][i].tank == TANK2 && arr_S_tile[j][i].terrain == BASE1)
                {
                    ++frozen2;
                }

                if ((i!=10 || j!=10) && arr_S_tile[j][i].tank == TANK2_CMD && arr_S_tile[j][i].terrain == BASE1)
                {
                    ++cmd2;
                }
            }
        }
    }

    if (arr_S_tile[0][0].tank == TANK2_CMD)
    {
        ++frozen2;
    }

    if (arr_S_tile[10][10].tank == TANK1_CMD)
    {
        ++frozen1;
    }

    /* Check if the game has to end */

    if (count1 == 0 || count2 == 0)
    {
        fin = 1;
    }

    else if (frozen1 == count1 || frozen2 == count2)
    {
        fin = 1;
    }

    /* Count points if the game has ended */

    if (fin == 1)
    {
        /*  Start by verifying if the commander tank is on the enemy commander's starting position.
            If he's in the base but not on the starting position, he earns two points like a normal tank. */

        if (arr_S_tile[0][0].tank == TANK2_CMD)
        {
            score2 += 3;
            --frozen2;
            --count2;
        }

        else if (cmd2 == 1)
        {
            score2 += 2;
            --count2;
        }

        if (arr_S_tile[10][10].tank == TANK1_CMD)
        {
            score1 += 3;
            --frozen1;
            --count1;
        }

        else if (cmd1 == 1)
        {
            score1 += 2;
            --count1;
        }

        /* Continue with tanks in the enemy base */

        for (  ; frozen2 > 0 ; frozen2--, count2--)
        {
            score2 += 2;
        }

        for (  ; frozen1 > 0 ; frozen1--, count1--)
        {
            score1 += 2;
        }

        /* End with the remaining tanks on the battleground */

        for (  ; count2 > 0 ; count2--)
        {
            score2 += 1;
        }

        for (  ; count1 > 0 ; count1--)
        {
            score1 += 1;
        }

        /* End the game */

        endGame( S_textures, score1, score2);
        generateMap( arr_S_tile ); //  Reinitialise the game
        return 0;
    }

    return 1;
}

void endGame( struct_textures* S_textures, int score1, int score2)
{
    /** This function manages the end game.
     *  Display the scores and the winner.
     */

    int continued = 1;
    SDL_Event event;
    SDL_Texture *tex_game_over = NULL, *tex_score1 = NULL, *tex_score2 = NULL;
    SDL_Surface *surf_text = NULL;
    TTF_Font *font = NULL;
    SDL_Rect position = {0,0,DIMENSION,DIMENSION};
    SDL_Color black = {0, 0, 0};

    font = TTF_OpenFont("FORCED_SQUARE.ttf", 55);
    char s_score1[20], s_score2[20];
    sprintf(s_score1, "Player 1: %d points", score1);   // Convert the int score in a string
    sprintf(s_score2, "Player 2: %d points", score2);

    SDL_RenderClear( renderer );
    SDL_RenderCopy( renderer, S_textures->tex_menu_bg, NULL, &position );

    if (score2 > score1)
    {
        surf_text = TTF_RenderText_Blended(font, "PLayer 2 won the game !", black);
    }

    else if (score1 > score2)
    {
        surf_text = TTF_RenderText_Blended(font, "Player 1 won the game !", black);
    }

    else
    {
        surf_text = TTF_RenderText_Blended(font, "It's a tie !", black);
    }

    // We don't use writeText() because we need to adapt the position according to its width to center the text
    tex_game_over = SDL_CreateTextureFromSurface( renderer, surf_text );
    SDL_QueryTexture(tex_game_over, NULL, NULL, &position.w, &position.h);
    position.x = DIMENSION/2 - position.w/2;
    position.y = 100;
    SDL_RenderCopy( renderer, tex_game_over, NULL, &position );

    font = TTF_OpenFont("FORCED_SQUARE.ttf", 35);
    writeText(s_score1, surf_text, tex_score1, font, &position, 50, 200);
    writeText(s_score2, surf_text, tex_score2, font, &position, 50, 250);

    SDL_FreeSurface( surf_text );
    SDL_RenderPresent( renderer );

    while (continued)
    {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_ESCAPE)
        {
            continued = 0;
        }
    }

    TTF_CloseFont(font);
    SDL_DestroyTexture(tex_game_over);
    SDL_DestroyTexture(tex_score1);
    SDL_DestroyTexture(tex_score2);
}

void save( int player, struct_tile arr_S_tile[][WIDTH] )
{
    /**
     *  This function saves the game in save.txt.
     */

    FILE* save = NULL;
    int i = 0, j = 0;

    save = fopen("save.txt", "w");
    if (save == NULL)
        fprintf(stderr, "Failed to save the game\n");

    for (i = 0 ; i < WIDTH ; i++)
    {
        for (j = 0 ; j < HEIGTH ; j++)
        {
            fprintf(save, "%d", arr_S_tile[j][i].tank);
        }
    }

    fprintf(save, "%d", player);

    fclose(save);
}

void close( struct_textures *S_textures )
{
    /**
     *  This functions empties the memory, nullifies our pointers and quits SDL.
     */

    int i;

    for (i = 1 ; i < 5 ; i++)
    {
        SDL_DestroyTexture( S_textures->tex_tank[i] );
        S_textures->tex_tank[i] = NULL;
    }

    for (i = 0 ; i < 2 ; i++)
    {
        SDL_DestroyTexture( S_textures->tex_player[i] );
        S_textures->tex_player[i] = NULL;
    }

    SDL_DestroyTexture( S_textures->tex_highlight );
    S_textures->tex_highlight = NULL;
    SDL_DestroyTexture( S_textures->tex_terrain );
    S_textures->tex_terrain = NULL;
    SDL_DestroyTexture( S_textures->tex_menu_bg );
    S_textures->tex_menu_bg = NULL;
    Mix_FreeMusic( music );
    music = NULL;
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    renderer= NULL;
    window = NULL;
    IMG_Quit();
    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
}
