/*  ***REMOVED***
    ***REMOVED***

    Projet de programmation en C - 1ère année
    Tank Wars
    Construit avec SDL 2.0
    */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL.h"

// On définit notre tableau de jeu

#define BLOC                46                  // Dimensions en pixels d'un bloc
#define LARGEUR             11                  // Nombre de blocs horizontalement
#define HAUTEUR             11                  // Nombre de blocs verticalement
#define DIMENSION           506                 // Dimensions de notre fenêtre

// On définit nos éléments de jeu. Leur valeur ne nous importe pas.

enum terrain {NORMAL, MINE, POLLUE, BASE1, BASE2, TANK, COMMAND};

void pause();

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0 )
    {
        fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        return -1;
    }

    /* Création de la fenêtre */
    SDL_Window* ecran = NULL;
    ecran = SDL_CreateWindow("Ma première application SDL2",SDL_WINDOWPOS_UNDEFINED,
                                                            SDL_WINDOWPOS_UNDEFINED,
                                                            DIMENSION,
                                                            DIMENSION,
                                                            SDL_WINDOW_SHOWN);

    /* Création du sprite pour le terrain */

    SDL_Surface* terrain = SDL_LoadBMP("./carte.bmp");

    if( ecran )
    {
        if ( terrain )
        {
            SDL_Rect dest = { 0, 0, 0, 0};
            SDL_BlitSurface(terrain,NULL,SDL_GetWindowSurface(ecran),&dest); // Copie du sprite

            SDL_UpdateWindowSurface(ecran); // Mise à jour de la fenêtre pour prendre en compte la copie du sprite

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
    SDL_FreeSurface(terrain);
    SDL_DestroyWindow(ecran);
    SDL_Quit();

    return 0;
}

void pause()
{
    int continuer = 1;
    SDL_Event event;

    while (continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
            case SDL_QUIT:
                continuer = 0;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE: /* Appui sur la touche Echap, on arrête le programme */
                        continuer = 0;
                        break;
                }
                break;
        }
    }
}
