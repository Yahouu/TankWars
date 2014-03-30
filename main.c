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

int initialisation();
void generateur(SDL_Window *ecran, SDL_Surface *sp_terrain[], SDL_Surface *sp_tanka, SDL_Surface *sp_tankb, int tab_tank[][HAUTEUR], int terrain[][HAUTEUR]);
void pause();
void close(SDL_Window *ecran, SDL_Surface *sp_terrain[], SDL_Surface *sp_tanka, SDL_Surface *sp_tankb, SDL_Surface *sp_tanka_mvt, SDL_Surface *sp_tankb_mvt);

int main(int argc, char *argv[])
{
    if (!initialisation())
    {
        fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        return -1;
    }

    int i,j;
    int continuer = 1, mouvement = 0, selection_tank;

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

    SDL_Event event;

    // Initialisation de nos pointeurs

    SDL_Window *ecran = NULL;
    SDL_Surface *sp_terrain_tmp[5] = {NULL}, *sp_terrain[5] = {NULL}, *sp_tanka = NULL, *sp_tanka_mvt = NULL, *sp_tankb = NULL, *sp_tankb_mvt = NULL;

    // Création de la fenêtre

    ecran = SDL_CreateWindow("TankWars",SDL_WINDOWPOS_UNDEFINED,    // Position de la fenêtre sur l'écran de l'utilisateur
                                        SDL_WINDOWPOS_UNDEFINED,    // Ici ça ne nous importe pas
                                        DIMENSION,                  // Dimensions
                                        DIMENSION,
                                        SDL_WINDOW_SHOWN);          // On affiche la fenêtre, cachée par défaut

    // Création des sprites

    sp_terrain_tmp[NORMAL] = IMG_Load("normal.png");
    sp_terrain_tmp[MINE] = IMG_Load("mine.png");
    sp_terrain_tmp[POLLUE] = IMG_Load("toxic.png");
    sp_terrain_tmp[BASE1] = IMG_Load("base1.png");
    sp_terrain_tmp[BASE2] = IMG_Load("base2.png");
    sp_tanka = IMG_Load("tankar.png");
    sp_tankb = IMG_Load("tankbr.png");
    sp_tanka_mvt = IMG_Load("tankar.png");
    sp_tankb_mvt = IMG_Load("tankbr.png");

    // On vérifie si la fenêtre puis les différents sprites ont bien été chargés ; ils prennent alors la valeur TRUE et on procède

    if( ecran )
    {
        if ( sp_terrain_tmp && sp_tanka && sp_tanka_mvt && sp_tankb)
        {
            SDL_Rect dest_mouvement = {0,0,0,0};

            // Génération de notre terrain après optimisation

            for (i = 0 ; i < 5 ; i++)
            {
                sp_terrain[i] = SDL_ConvertSurface( sp_terrain_tmp[i], SDL_GetWindowSurface(ecran)->format, NULL );
                SDL_FreeSurface( sp_terrain_tmp[i] );
            }

            generateur(ecran, sp_terrain, sp_tanka, sp_tankb, tab_tank, terrain);
            SDL_UpdateWindowSurface(ecran);

            // Boucle infinie pour bouger un tank
            while (continuer)
            {
                SDL_WaitEvent(&event);
                if (event.type == SDL_QUIT)
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
                    printf("%d", terrain[x / BLOC][y / BLOC]);

                    /* -------------------------- */

                    if (mouvement)
                    {
                        if (selection_tank == 1 && tab_tank[x/BLOC][y/BLOC] != TANK1)  // On vérifie qu'on ne clique pas sur un tank
                        {
                                tab_tank[x / BLOC][y / BLOC] = TANK1;
                                mouvement=0;
                                continuer=0;
                        }

                        if (selection_tank == 2 && tab_tank[x/BLOC][y/BLOC] != TANK2)  // On vérifie qu'on ne clique pas sur un tank
                        {
                                tab_tank[x / BLOC][y / BLOC] = TANK2;
                                mouvement = 0;
                                continuer=0;
                        }
                    }

                    else if (!mouvement)
                    {
                        // On retient quel tank on a selectionné

                        if (tab_tank[x/BLOC][y/BLOC] == TANK1)
                        {
                            selection_tank = TANK1;
                            mouvement = 1;
                        }

                        else if (tab_tank[x/BLOC][y/BLOC] == TANK2)
                        {
                            selection_tank = TANK2;
                            mouvement = 1;
                        }

                        tab_tank[x / BLOC][y / BLOC] = VIDE;
                    }
                }

                // Quand on a sélectionné un tank et on bouge la souris, le tank suit nos mouvements

                if (event.type == SDL_MOUSEMOTION && mouvement)
                {
                    dest_mouvement.x = event.motion.x;
                    dest_mouvement.y = event.motion.y;

                    // On remplit le rectangle dest_mouvement de blanc, on re-génère le terrain et on blitte sp_tank{a,b}_mvt
                    // Sinon, on a une trainée de tanks suivant les mouvements de la souris

                    if (selection_tank == 1)
                    {
                        SDL_FillRect(sp_tanka_mvt, &dest_mouvement, SDL_MapRGB(sp_tanka->format, 255, 255, 255));
                    }

                    if (selection_tank == 2)
                    {
                        SDL_FillRect(sp_tankb_mvt, &dest_mouvement, SDL_MapRGB(sp_tanka->format, 255, 255, 255));
                    }
                }

                generateur(ecran, sp_terrain, sp_tanka, sp_tankb, tab_tank, terrain);

                if (mouvement && selection_tank == 1)
                {
                    SDL_BlitSurface(sp_tanka_mvt,NULL,SDL_GetWindowSurface(ecran),&dest_mouvement);
                }

                if (mouvement && selection_tank == 2)
                {
                    SDL_BlitSurface(sp_tankb_mvt,NULL,SDL_GetWindowSurface(ecran),&dest_mouvement);
                }

                SDL_UpdateWindowSurface(ecran);

            }
        }

        else
        {
                fprintf(stdout,"Échec de chargement du sprite (%s)\n",SDL_GetError());
        }

    }
    else
    {
            fprintf(stderr,"Erreur de création de la fenêtre: %s\n",SDL_GetError());
    }

    pause();
    close(ecran, sp_terrain, sp_tanka, sp_tankb, sp_tanka_mvt, sp_tankb_mvt);

    return 0;
}

int initialisation()
{
    int init=1;

    // On initialise SDL et on vérifie immédiatement si la librairie s'est bien lancée

    if (SDL_Init(SDL_INIT_VIDEO) != 0 )
    {
        fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        init=0;
    }

    // SDL_Image

    int flags=IMG_INIT_JPG|IMG_INIT_PNG;
    int initted=IMG_Init(flags);
    if((initted&flags) != flags) {
        fprintf(stdout,"Echec de l'initialisation du support des png et jpg par IMG_Init (%s)\n",IMG_GetError());
        init=0;
    }

    return init;

}

void generateur(SDL_Window *ecran, SDL_Surface *sp_terrain[], SDL_Surface *sp_tanka, SDL_Surface *sp_tankb, int tab_tank[][HAUTEUR], int terrain[][HAUTEUR])
{
    SDL_Rect destination;
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
                    SDL_BlitSurface(sp_terrain[0],NULL,SDL_GetWindowSurface(ecran),&destination);
                    break;

                case 1:
                    SDL_BlitSurface(sp_terrain[1],NULL,SDL_GetWindowSurface(ecran),&destination);
                    break;

                case 2:
                    SDL_BlitSurface(sp_terrain[2],NULL,SDL_GetWindowSurface(ecran),&destination);
                    break;

                case 3:
                    SDL_BlitSurface(sp_terrain[3],NULL,SDL_GetWindowSurface(ecran),&destination);
                    break;

                case 4:
                    SDL_BlitSurface(sp_terrain[4],NULL,SDL_GetWindowSurface(ecran),&destination);
                    break;
            }

            if (tab_tank[i][j] == 1)
            {
                SDL_BlitSurface(sp_tanka,NULL,SDL_GetWindowSurface(ecran),&destination);
            }

            if (tab_tank[i][j] == 2)
            {
                SDL_BlitSurface(sp_tankb,NULL,SDL_GetWindowSurface(ecran),&destination);
            }
        }
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


void close(SDL_Window *ecran, SDL_Surface *sp_terrain[], SDL_Surface *sp_tanka, SDL_Surface *sp_tankb, SDL_Surface *sp_tanka_mvt, SDL_Surface *sp_tankb_mvt)
{
    /* Cette procédure vide la mémoire, pointe nos pointeurs sur NULL et quitte SDL */

    int i;

    IMG_Quit();
    SDL_FreeSurface(sp_tanka);
    SDL_FreeSurface(sp_tankb);
    SDL_FreeSurface(sp_tanka_mvt);
    SDL_FreeSurface(sp_tankb_mvt);
    for (i = 0 ; i < 5 ; i++)
    {
        SDL_FreeSurface( sp_terrain[i] );
    }
    sp_terrain = {NULL};
    sp_tanka = NULL;
    sp_tankb = NULL;
    sp_tanka_mvt = NULL;
    sp_tankb_mvt = NULL;
    SDL_DestroyWindow(ecran);
    ecran = NULL;
    SDL_Quit();
}
