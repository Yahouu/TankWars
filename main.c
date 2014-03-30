/*  Projet de programmation en C - 1ère année
    Tank Wars
    Construit avec SDL 2.0
    */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL.h"
#include "SDL_image.h"

// On définit notre tableau de jeu

#define BLOC                46                  // Dimensions en pixels d'un bloc
#define DIMTANK             32                  // Dimensions d'un tank
#define LARGEUR             11                  // Nombre de blocs horizontalement
#define HAUTEUR             11                  // Nombre de blocs verticalement
#define DIMENSION           506                 // Dimensions de notre fenêtre

// On définit nos éléments de jeu. Leur valeur ne nous importe pas.

enum liste_terrain {NORMAL, MINE, POLLUE, BASE1, BASE2};
enum liste_tank {VIDE, TANK1, TANK2, TANK1_CMD, TANK2_CMD};

// Prototypes

int initialisation();
void loadTextures( SDL_Texture *sp_terrain[],SDL_Texture *sp_tank[],SDL_Texture *sp_tank_mvt[]);
void generateTextures(SDL_Window *ecran, SDL_Texture *sp_terrain[], SDL_Texture *sp_tank[], int tab_tank[][HAUTEUR], int terrain[][HAUTEUR]);
void deplacerTank(int *joueur, SDL_Texture *sp_terrain[], SDL_Texture *sp_tank[], SDL_Texture *sp_tank_mvt[], int tab_tank[][HAUTEUR], int terrain[][HAUTEUR]);
void pause();
void close(SDL_Window *ecran, SDL_Texture *sp_terrain[], SDL_Texture *sp_tank[],SDL_Texture *sp_tank_mvt[]);

// Fenêtre de jeu et renderer

SDL_Window *ecran = NULL;
SDL_Renderer *renderer = NULL;

// Fonction principale

int main(int argc, char *argv[])
{
    if (!initialisation())
    {
        fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        return -1;
    }

    srand(time(NULL));
    int joueur = ((rand()%2)+1);   // Joueur 1 ou 2 commence au hasard

    // Notre terrain de jeu

    int terrain[LARGEUR][HAUTEUR] = {
        { 3,3,3,3,3,1,0,0,0,0,0 },
        { 3,1,3,3,0,0,0,0,0,1,0 },
        { 3,3,3,1,0,1,0,1,0,0,0 },
        { 3,3,1,0,0,0,0,0,1,0,0 },
        { 3,0,0,0,2,2,2,0,0,0,0 },
        { 1,0,1,0,2,2,2,0,1,0,1 },
        { 0,0,0,0,2,2,2,0,0,0,4 },
        { 0,0,1,0,0,0,0,0,1,4,4 },
        { 0,0,0,1,0,1,0,1,4,4,4 },
        { 0,1,0,0,0,0,0,4,4,1,4 },
        { 0,0,0,0,0,1,4,4,4,4,4 },
    };

    // Nos tanks

    int tab_tank[LARGEUR][HAUTEUR] = {
        { 1,1,1,1,1,0,0,0,0,0,0 },
        { 1,0,1,1,0,0,0,0,0,0,0 },
        { 1,1,1,0,0,0,0,0,0,0,0 },
        { 1,1,0,0,0,0,0,0,0,0,0 },
        { 1,0,0,0,0,0,0,0,0,0,0 },
        { 0,0,0,0,0,0,0,0,0,0,0 },
        { 0,0,0,0,0,0,0,0,0,0,2 },
        { 0,0,0,0,0,0,0,0,0,2,2 },
        { 0,0,0,0,0,0,0,0,2,2,2 },
        { 0,0,0,0,0,0,0,2,2,0,2 },
        { 0,0,0,0,0,0,2,2,2,2,2 },
    };

    // On charge les textures

    SDL_Texture *sp_terrain[5] = {NULL}, *sp_tank[5] = {NULL}, *sp_tank_mvt[5] = {NULL};
    loadTextures(sp_terrain, sp_tank, sp_tank_mvt);

    // On génère notre terrain pour la première fois

    SDL_RenderClear( renderer );
    generateTextures(ecran, sp_terrain, sp_tank, tab_tank, terrain);
    SDL_RenderPresent( renderer );

    // Boucle infinie pour bouger un tank
    deplacerTank(&joueur, sp_terrain, sp_tank, sp_tank_mvt, tab_tank, terrain);

    pause();
    close(ecran, sp_terrain, sp_tank, sp_tank_mvt);

    return 0;
}

int initialisation()
{
    /*  Cette fonction permet d'initialiser toutes les autres fonctions nécessaire au lancement du jeu.
        On initialise SDL, SDL_Image, le renderer et on crée la fenêtre de jeu.
        Si on a un problème, un message d'erreur est affiché dans SDL_console.
        */

    int init=1;

    // On initialise SDL et on vérifie immédiatement si la librairie s'est bien lancée

    if (SDL_Init(SDL_INIT_VIDEO) != 0 )
    {
        fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        init=0;
    }

    if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
    {
        printf( "Échec de l'initialisation du filtrage linéaire de textures!" );
    }

    // Création de la fenêtre

    ecran = SDL_CreateWindow("TankWars",SDL_WINDOWPOS_UNDEFINED,    // Position de la fenêtre sur l'écran de l'utilisateur
                                        SDL_WINDOWPOS_UNDEFINED,    // Ici ça ne nous importe pas
                                        DIMENSION,                  // Dimensions
                                        DIMENSION,
                                        SDL_WINDOW_SHOWN);          // On affiche la fenêtre, cachée par défaut

    if( ecran == NULL )
    {
        printf( "Échec de la création de la fenêtre: %s\n", SDL_GetError() );
        init = 0;
    }

    // Création du renderer

    renderer = SDL_CreateRenderer( ecran, -1, SDL_RENDERER_ACCELERATED );

    if( ecran == NULL )
    {
        printf( "Échec de la création du renderer: SDL Error: %s\n", SDL_GetError() );
        init = 0;
    }

    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    // SDL_Image

    int flags=IMG_INIT_JPG|IMG_INIT_PNG;
    int initted=IMG_Init(flags);
    if((initted&flags) != flags) {
        fprintf(stdout,"Echec de l'initialisation du support des png et jpg par IMG_Init (%s)\n",IMG_GetError());
        init=0;
    }

    return init;

}

void loadTextures(SDL_Texture *sp_terrain[],SDL_Texture *sp_tank[],SDL_Texture *sp_tank_mvt[])
{
    /*  Cette procédure charge toutes nos textures en mémoire.*/

    sp_terrain[NORMAL] = SDL_CreateTextureFromSurface( renderer, IMG_Load("normal.png"));
    sp_terrain[MINE] = SDL_CreateTextureFromSurface( renderer, IMG_Load("mine.png"));
    sp_terrain[POLLUE] = SDL_CreateTextureFromSurface( renderer, IMG_Load("toxic.png"));
    sp_terrain[BASE1] = SDL_CreateTextureFromSurface( renderer, IMG_Load("base1.png"));
    sp_terrain[BASE2] = SDL_CreateTextureFromSurface( renderer, IMG_Load("base2.png"));
    sp_tank[TANK1] = SDL_CreateTextureFromSurface( renderer, IMG_Load("tankar.png"));
    sp_tank[TANK2] = SDL_CreateTextureFromSurface( renderer, IMG_Load("tankbr.png"));
    sp_tank_mvt[TANK1] = SDL_CreateTextureFromSurface( renderer, IMG_Load("tankar.png"));
    sp_tank_mvt[TANK2] = SDL_CreateTextureFromSurface( renderer, IMG_Load("tankbr.png"));
}

void generateTextures(SDL_Window *ecran, SDL_Texture *sp_terrain[], SDL_Texture *sp_tank[], int tab_tank[][HAUTEUR], int terrain[][HAUTEUR])
{
    /*  Cette procédure génère nos textures dans notre fenêtre de jeu */

    SDL_Rect source = {0,0,BLOC, BLOC};
    SDL_Rect destination = {0,0,BLOC,BLOC};
    int i,j;

    for (i = 0 ; i < LARGEUR ; i++)
    {
        for (j = 0 ; j < HAUTEUR ; j++)
        {
            destination.x = i * BLOC;
            destination.y = j * BLOC;
            switch(terrain[i][j])
            {
                case 0:
                    SDL_RenderCopy( renderer, sp_terrain[NORMAL], NULL, &destination );
                    break;

                case 1:
                    SDL_RenderCopy( renderer, sp_terrain[MINE], NULL, &destination );
                    break;

                case 2:
                    SDL_RenderCopy( renderer, sp_terrain[POLLUE], NULL, &destination );
                    break;

                case 3:
                    SDL_RenderCopy( renderer, sp_terrain[BASE1], NULL, &destination );
                    break;

                case 4:
                    SDL_RenderCopy( renderer, sp_terrain[BASE2], NULL, &destination );
                    break;
            }

            if (tab_tank[i][j] == 1)
            {
                SDL_RenderCopy( renderer, sp_tank[TANK1], &source, &destination );
            }

            if (tab_tank[i][j] == 2)
            {
                SDL_RenderCopy( renderer, sp_tank[TANK2], &source, &destination );
            }
        }
    }
}

void deplacerTank(int *joueur, SDL_Texture *sp_terrain[], SDL_Texture *sp_tank[], SDL_Texture *sp_tank_mvt[], int tab_tank[][HAUTEUR], int terrain[][HAUTEUR])
{
    /* Cette procédure sert à déplacer les tanks tour par tour */

    int continuer = 1, mouvement = 0;
    SDL_Event event;    // Utilisé pour la gestion des évènements
    SDL_Rect dest_mouvement = {0,0,BLOC,BLOC};  // Utilisé pour déplacer un tank et faire suivre la souris au sprite

    while (continuer)
    {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
        {
            continuer = 0;
        }

        // Quand on ne bouge pas encore un tank, on clique sur un tank et celui-ci disparaît

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            int x=0, y=0;
            SDL_GetMouseState( &x, &y );

            /* ---------debugging----------*/

            printf("%d %d\n",x,y);
            printf("%d %d\n",x/BLOC,y/BLOC);
            printf("%d\n", terrain[x / BLOC][y / BLOC]);
            printf("%d\n\n", tab_tank[x / BLOC][y / BLOC]);

            /* -------------------------- */

            if (mouvement)
            {
                if (*joueur == 1 && tab_tank[x/BLOC][y/BLOC] != TANK1)  // On vérifie qu'on ne clique pas sur un des ses propres tank
                {
                    tab_tank[x / BLOC][y / BLOC] = TANK1;
                    mouvement=0;
                    *joueur = 2;
                }

                else if (*joueur == 2 && tab_tank[x/BLOC][y/BLOC] != TANK2)
                {
                    tab_tank[x / BLOC][y / BLOC] = TANK2;
                    mouvement = 0;
                    *joueur = 1;
                }
            }

            else if (!mouvement)
            {
                // On retient quel tank on a selectionné

                if (*joueur == 1 && tab_tank[x/BLOC][y/BLOC] == TANK1)
                {
                    tab_tank[x / BLOC][y / BLOC] = VIDE;
                    mouvement = 1;
                }

                else if (*joueur == 2 && tab_tank[x/BLOC][y/BLOC] == TANK2)
                {
                    tab_tank[x / BLOC][y / BLOC] = VIDE;
                    mouvement = 1;
                }
            }
        }

        // Quand on a sélectionné un tank et on bouge la souris, le tank suit nos mouvements

        if (event.type == SDL_MOUSEMOTION && mouvement)
        {
            dest_mouvement.x = event.motion.x;
            dest_mouvement.y = event.motion.y;
        }

        SDL_RenderClear( renderer );
        generateTextures(ecran, sp_terrain, sp_tank, tab_tank, terrain);

        if (mouvement && *joueur == 1)
        {
            SDL_RenderCopy( renderer, sp_tank_mvt[TANK1], NULL, &dest_mouvement );
        }

        if (mouvement && *joueur == 2)
        {
            SDL_RenderCopy( renderer, sp_tank_mvt[TANK2], NULL, &dest_mouvement );
        }

        SDL_RenderPresent( renderer );
    }
}

void pause()
{
    /*  Cette procédure permet de pauser le programme quand il a finit d'executer toutes les instructions.
        Cela laisse le temps à l'utilisateur d'intéragir avec le jeu et de quitter le jeu par lui-même
        */

    int continuer = 1;
    SDL_Event event;

    while (continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:  // On quitte le jeu quand on appuie sur la croix rouge habituelle
                continuer = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: // On quitte quand on appuie sur la touche Echap/Escape
                        continuer = 0;
                        break;
                }
                break;
        }
    }
}

void close(SDL_Window *ecran, SDL_Texture *sp_terrain[], SDL_Texture *sp_tank[], SDL_Texture *sp_tank_mvt[])
{
    /* Cette procédure vide la mémoire, pointe nos pointeurs sur NULL et quitte SDL */

    int i;

    for (i = 0 ; i < 5 ; i++)
    {
        SDL_DestroyTexture( sp_terrain[i] );
        SDL_DestroyTexture(sp_tank[i]);
        SDL_DestroyTexture(sp_tank_mvt[i]);
    }
    sp_terrain = {NULL};
    sp_tank = {NULL};
    sp_tank_mvt = {NULL};
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow(ecran);
    renderer= NULL;
    ecran = NULL;
    IMG_Quit();
    SDL_Quit();
}
