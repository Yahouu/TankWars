/** Projet de programmation en C - 1ère année
 *  Tank Wars
 *  Construit avec SDL 2.0
 */

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "SDL.h"
#include "SDL_image.h"

// On définit notre tableau de jeu

#define BLOC                65                  // Dimensions en pixels d'un bloc
#define DIMTANK             64                  // Dimensions d'un tank
#define LARGEUR             11                  // Nombre de blocs horizontalement
#define HAUTEUR             11                  // Nombre de blocs verticalement
#define DIMENSION           716                 // Dimensions de notre carte
#define DIMEXTRA            598                 // Largeur de la fenêtre

// On définit nos éléments de jeu. Leur valeur ne nous importe pas.

enum liste_terrain {NORMAL, MINE, POLLUE, BASE1, BASE2};
enum liste_tank {VIDE, TANK1, TANK2, TANK1_CMD, TANK2_CMD};
enum liste_deplacement {INVALIDE, VALIDE, ENNEMI};

// Notre terrain de jeu

typedef struct struct_terrain{

    int tab_terrain[LARGEUR][HAUTEUR] = {
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

    int tab_tank[LARGEUR][HAUTEUR] = {
        { 3,1,1,1,1,0,0,0,0,0,0 },
        { 1,0,1,1,0,0,0,0,0,0,0 },
        { 1,1,1,0,0,0,0,0,0,0,0 },
        { 1,1,0,0,0,0,0,0,0,0,0 },
        { 1,0,0,0,0,0,0,0,0,0,0 },
        { 0,0,0,0,0,0,0,0,0,0,0 },
        { 0,0,0,0,0,0,0,0,0,0,2 },
        { 0,0,0,0,0,0,0,0,0,2,2 },
        { 0,0,0,0,0,0,0,0,2,2,2 },
        { 0,0,0,0,0,0,0,2,2,0,2 },
        { 0,0,0,0,0,0,2,2,2,2,4 },
    };
}struct_terrain;

// Nos textures

typedef struct struct_textures{
    SDL_Texture *tex_terrain = NULL;
    SDL_Texture *tex_tank[5] = {NULL};
    SDL_Texture *tex_tank_mvt[5] = {NULL};
}struct_textures;

// Prototypes

int initialisation();
void loadTextures( struct_textures *textures );
void generateTextures( struct_textures *textures, struct_terrain terrain );
void collision(int joueur, int x, int y, struct_terrain terrain, int deplacement_possibles[][HAUTEUR]);
void deplacerTank(int *joueur, struct_textures *textures, struct_terrain *terrain);
void pause();
void close( struct_textures *textures );

// Fenêtre de jeu et renderer

SDL_Window *ecran = NULL;
SDL_Renderer *renderer = NULL;



/************************************************************************************************
 *                                          FONCTIONS                                           *
 ************************************************************************************************/


int main(int argc, char *argv[])
{

    // On vérifie que tout s'est bien initialisé

    if (!initialisation())
    {
        fprintf(stdout,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
        return -1;
    }

    srand(time(NULL));
    int joueur = ((rand()%2)+1);   // Joueur 1 ou 2 commence au hasard

    struct_terrain terrain;
    struct_textures textures;

    loadTextures( &textures );

    // On génère notre terrain pour la première fois

    SDL_RenderClear( renderer );
    generateTextures( &textures, terrain );
    SDL_RenderPresent( renderer );

    // Boucle infinie pour bouger un tank
    deplacerTank( &joueur, &textures, &terrain );

    pause();
    close( &textures );

    return 0;
}

int initialisation()
{
    /** Cette fonction permet d'initialiser toutes les autres fonctions nécessaire au lancement du jeu.
     *  On initialise SDL, SDL_Image, le renderer et on crée la fenêtre de jeu.
     *  Si on a un problème, un message d'erreur est affiché dans la console.
     *
     *  Retourne 1 si tout a été initialisé, 0 sinon
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
        fprintf(stdout, "Échec de l'initialisation du filtrage linéaire de textures!" );
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

    // La couleur par défaut dans la fenêtre est le blanc

    SDL_SetRenderDrawColor( renderer, 0xFF, 0xFF, 0xFF, 0xFF );

    // Initialisation de SDL_Image

    int flags=IMG_INIT_JPG|IMG_INIT_PNG;
    int initted=IMG_Init(flags);
    if((initted&flags) != flags) {
        fprintf(stdout,"Echec de l'initialisation du support des png et jpg par IMG_Init (%s)\n",IMG_GetError());
        init=0;
    }

    return init;

}

void loadTextures( struct_textures *textures )
{
    /** Cette procédure charge toutes nos textures en mémoire.
     *
     *  Entrée: structure contenant les pointeurs vers les textures
     */

    textures->tex_terrain = IMG_LoadTexture( renderer, "./img/carte.png");
    textures->tex_tank[TANK1] = IMG_LoadTexture( renderer, "./img/tanka.png");
    textures->tex_tank[TANK2] = IMG_LoadTexture( renderer, "./img/tankb.png");
    textures->tex_tank[TANK1_CMD] = IMG_LoadTexture( renderer, "./img/commanda.png");
    textures->tex_tank[TANK2_CMD] = IMG_LoadTexture( renderer, "./img/commandb.png");
}

void generateTextures( struct_textures *textures, struct_terrain terrain )
{
    /** Cette procédure génère nos textures dans notre fenêtre de jeu
     *
     *  Entrée: structure contenant nos textures et structure contenant le tableau de jeu
     */

    SDL_Rect rect_tank = {0,0,BLOC,BLOC};
    SDL_Rect rect_carte = {0,0,DIMENSION,DIMENSION};
    int i,j;

    SDL_RenderCopy( renderer, textures->tex_terrain, NULL, &rect_carte );

    for (i = 0 ; i < LARGEUR ; i++)
    {
        for (j = 0 ; j < HAUTEUR ; j++)
        {
            rect_tank.y = i * BLOC;
            rect_tank.x = j * BLOC;
            switch(terrain.tab_tank[i][j])
            {
                case 1:
                    SDL_RenderCopy( renderer, textures->tex_tank[TANK1], NULL, &rect_tank );
                    break;

                case 2:
                    SDL_RenderCopy( renderer, textures->tex_tank[TANK2], NULL, &rect_tank );
                    break;

                case 3:
                    SDL_RenderCopy( renderer, textures->tex_tank[TANK1_CMD], NULL, &rect_tank );
                    break;

                case 4:
                    SDL_RenderCopy( renderer, textures->tex_tank[TANK2_CMD], NULL, &rect_tank );
                    break;
            }
        }
    }
}

void collision( int joueur, int x, int y, struct_terrain terrain, int deplacement_possibles[][HAUTEUR] )
{
    memset(deplacement_possibles, 0, HAUTEUR*LARGEUR*sizeof deplacement_possibles[0][0]);   // Très utile pour réinitialiser notre tableau des déplacements
    int i,j;
    deplacement_possibles[y][x] = 1;    // On peut reposer le tank sur sa position de départ

    /* VERS LE BAS */

    for (i=y+1; i<HAUTEUR; i++)
    {
        if ((joueur == 1 && terrain.tab_tank[i][x] == TANK1)
            || (joueur == 2 && terrain.tab_tank[i][x] == TANK2)
            || terrain.tab_terrain[i][x] == MINE
            || terrain.tab_terrain[i][x] == POLLUE)
        {
            break;
        }

        else if ((joueur == 1 && terrain.tab_tank[i][x] == TANK2)
                 || (joueur == 2 && terrain.tab_tank[i][x] == TANK1))
        {
            deplacement_possibles[i][x] = 1;
            break;
        }

        else
        {
            deplacement_possibles[i][x] = 1;
        }
    }

    /* VERS LE HAUT */

    for (i=y-1; i>=0; i--)
    {
        if ((joueur == 1 && terrain.tab_tank[i][x] == TANK1)
            || (joueur == 2 && terrain.tab_tank[i][x] == TANK2)
            || terrain.tab_terrain[i][x] == MINE
            || terrain.tab_terrain[i][x] == POLLUE)
        {
            break;
        }

        else if ((joueur == 1 && terrain.tab_tank[i][x] == TANK2)
                 || (joueur == 2 && terrain.tab_tank[i][x] == TANK1))
        {
            deplacement_possibles[i][x] = 1;
            break;
        }

        else
        {
            deplacement_possibles[i][x] = 1;
        }
    }

    /* VERS LA GAUCHE */

    for (i=x-1; i>=0; i--)
    {
        if ((joueur == 1 && terrain.tab_tank[y][i] == TANK1)
            || (joueur == 2 && terrain.tab_tank[y][i] == TANK2)
            || terrain.tab_terrain[y][i] == MINE
            || terrain.tab_terrain[y][i] == POLLUE)
        {
            break;
        }

        else if ((joueur == 1 && terrain.tab_tank[y][i] == TANK2)
                 || (joueur == 2 && terrain.tab_tank[y][i] == TANK1))
        {
            deplacement_possibles[y][i] = 1;
            break;
        }

        else
        {
            deplacement_possibles[y][i] = 1;
        }
    }

    /* VERS LA DROITE */

    for (i=x+1; i<LARGEUR; i++)
    {
        if ((joueur == 1 && terrain.tab_tank[y][i] == TANK1)
            || (joueur == 2 && terrain.tab_tank[y][i] == TANK2)
            || terrain.tab_terrain[y][i] == MINE
            || terrain.tab_terrain[y][i] == POLLUE)
        {
            break;
        }

        else if ((joueur == 1 && terrain.tab_tank[y][i] == TANK2)
                 || (joueur == 2 && terrain.tab_tank[y][i] == TANK1))
        {
            deplacement_possibles[y][i] = 1;
            break;
        }

        else
        {
            deplacement_possibles[y][i] = 1;
        }
    }

    /* Déplacement en diagonale vers la droite et vers le bas de la position actuelle */

    for (i=y+1, j=x+1; i<HAUTEUR && j<LARGEUR; i++,j++)
    {
        if ((joueur == 1 && terrain.tab_tank[i][j] == TANK1)
            || (joueur == 2 && terrain.tab_tank[i][j] == TANK2)
            || terrain.tab_terrain[i][j] == MINE
            || terrain.tab_terrain[i][j] == POLLUE)
        {
            break;
        }

        else if ((joueur == 1 && terrain.tab_tank[i][j] == TANK2)
                 || (joueur == 2 && terrain.tab_tank[i][j] == TANK1))
        {
            deplacement_possibles[i][j] = 1;
            break;
        }

        else
        {
            deplacement_possibles[i][j] = 1;
        }
    }

    /* Déplacement en diagonale vers la droite et vers le haut de la position actuelle */

    for (i=y-1, j=x+1; i>=0 && j<LARGEUR; i--,j++)
    {
        if ((joueur == 1 && terrain.tab_tank[i][j] == TANK1)
            || (joueur == 2 && terrain.tab_tank[i][j] == TANK2)
            || terrain.tab_terrain[i][j] == MINE
            || terrain.tab_terrain[i][j] == POLLUE)
        {
            break;
        }

        else if ((joueur == 1 && terrain.tab_tank[i][j] == TANK2)
                 || (joueur == 2 && terrain.tab_tank[i][j] == TANK1))
        {
            deplacement_possibles[i][j] = 1;
            break;
        }

        else
        {
            deplacement_possibles[i][j] = 1;
        }
    }

    /* Déplacement en diagonale vers la gauche et vers le bas de la position actuelle */

    for (i=y+1, j=x-1; i<HAUTEUR && j>=0; i++,j--)
    {
        if ((joueur == 1 && terrain.tab_tank[i][j] == TANK1)
            || (joueur == 2 && terrain.tab_tank[i][j] == TANK2)
            || terrain.tab_terrain[i][j] == MINE
            || terrain.tab_terrain[i][j] == POLLUE)
        {
            break;
        }

        else if ((joueur == 1 && terrain.tab_tank[i][j] == TANK2)
                 || (joueur == 2 && terrain.tab_tank[i][j] == TANK1))
        {
            deplacement_possibles[i][j] = 1;
            break;
        }

        else
        {
            deplacement_possibles[i][j] = 1;
        }
    }

    /* Déplacement en diagonale vers la gauche et vers le haut de la position actuelle */

    for (i=y-1, j=x-1; i>=0 && j>=0; i--,j--)
    {
        if ((joueur == 1 && terrain.tab_tank[i][j] == TANK1)
            || (joueur == 2 && terrain.tab_tank[i][j] == TANK2)
            || terrain.tab_terrain[i][j] == MINE
            || terrain.tab_terrain[i][j] == POLLUE)
        {
            break;
        }

        else if ((joueur == 1 && terrain.tab_tank[i][j] == TANK2)
                 || (joueur == 2 && terrain.tab_tank[i][j] == TANK1))
        {
            deplacement_possibles[i][j] = 1;
            break;
        }

        else
        {
            deplacement_possibles[i][j] = 1;
        }
    }
}


void deplacerTank(int *joueur, struct_textures *textures, struct_terrain *terrain)
{
    /** Cette procédure sert à déplacer les tanks tour par tour
     *  Tant qu'on ne sort pas de la boucle infinie volontairement ou en fin de partie
     *
     *  Entrée: Joueur qui commence à jouer, notre structure de textures et notre structure contenant le terrain de jeu
     */

    int continuer = 1, mouvement = 0;
    int i,j;
    int x=0, y=0;
    int x_tmp=0, y_tmp=0;

    /* Nos possibilités de déplacement */

    int deplacement_possibles[LARGEUR][HAUTEUR] = {{INVALIDE},{INVALIDE}};

    SDL_Event event;    // Utilisé pour la gestion des évènements
    SDL_Rect dest_mouvement = {0,0,BLOC,BLOC};  // Utilisé pour déplacer un tank et faire suivre la souris au sprite

    while (continuer)
    {
        x_tmp=0;
        y_tmp=0;
        SDL_WaitEvent(&event);

        // Si on quitte la fenêtre ou on appuie sur Echap, on sort de la boucle

        if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE)
        {
            continuer = 0;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN)
        {
            SDL_GetMouseState( &x_tmp, &y_tmp );    // On récupère les coordonnées du clic

            /* ---------debugging----------*/

            printf("joueur:%d\n", *joueur);
            printf("x:%dpx y:%dpx\n",x_tmp,y_tmp);
            printf("x:%d y:%d\n",x_tmp/BLOC,y_tmp/BLOC);
            printf("x saved:%d y saved:%d\n",x,y);
            printf("terrain: %d\n", terrain->tab_terrain[x_tmp / BLOC][y_tmp / BLOC]);
            printf("tank: %d\n\n", terrain->tab_tank[x_tmp / BLOC][y_tmp / BLOC]);
            for (i=0; i<HAUTEUR; i++)
            {
                for (j=0; j<LARGEUR; j++)
                {
                    printf(" %d |", deplacement_possibles[i][j]);
                }
                printf("\n");
            }

            /* -------------------------- */

            if (!mouvement)
            {
                x = x_tmp/BLOC;
                y = y_tmp/BLOC;

                if (*joueur == 1 && terrain->tab_tank[y][x] == TANK1)
                {
                    printf("x_tmp:%d y_tmp:%d\n",x,y);
                    collision( *joueur, x, y, *terrain, deplacement_possibles );

                    /* ------- Debugging ------- */

                    for (i=0; i<HAUTEUR; i++)
                    {
                        for (j=0; j<LARGEUR; j++)
                        {
                            printf(" %d |", deplacement_possibles[i][j]);
                        }
                        printf("\n");
                    }

                    /* ------------------------ */
                    terrain->tab_tank[y][x] = VIDE;
                    mouvement = 1;
                }

                else if (*joueur == 2 && terrain->tab_tank[y][x] == TANK2)
                {
                    printf("x_tmp:%d y_tmp:%d\n",x_tmp,y_tmp);
                    collision( *joueur, x, y, *terrain, deplacement_possibles );

                    /* ------- Debugging ------- */

                    for (i=0; i<HAUTEUR; i++)
                    {
                        for (j=0; j<LARGEUR; j++)
                        {
                            printf(" %d |", deplacement_possibles[i][j]);
                        }
                        printf("\n");
                    }

                    /* ------------------------ */
                    terrain->tab_tank[y][x] = VIDE;
                    mouvement = 1;
                }
            }

            else if (mouvement)
            {
                if (*joueur == 1 && deplacement_possibles[y_tmp/BLOC][x_tmp/BLOC] != INVALIDE)
                {
                    terrain->tab_tank[y_tmp / BLOC][x_tmp / BLOC] = TANK1;
                    mouvement = 0;
                    *joueur = 2;
                }

                else if (*joueur == 2 && deplacement_possibles[y_tmp/BLOC][x_tmp/BLOC] != INVALIDE)
                {
                    terrain->tab_tank[y_tmp / BLOC][x_tmp / BLOC] = TANK2;
                    mouvement = 0;
                    *joueur = 1;
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
        generateTextures( textures, *terrain );

        if (mouvement && *joueur == 1)
        {
            SDL_RenderCopy( renderer, textures->tex_tank[TANK1], NULL, &dest_mouvement );
        }

        if (mouvement && *joueur == 2)
        {
            SDL_RenderCopy( renderer, textures->tex_tank[TANK2], NULL, &dest_mouvement );
        }

        SDL_RenderPresent( renderer );
    }
}

void pause()
{
    /**  Cette procédure permet de pauser le programme quand il a finit d'executer toutes les instructions.
     *   Cela laisse le temps à l'utilisateur d'intéragir avec le jeu et de quitter le jeu par lui-même
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

void close( struct_textures *textures )
{
    /** Cette procédure vide la mémoire, pointe nos pointeurs sur NULL et quitte SDL
     *
     *  Entrée: structure contenant nos textures
     */

    int i;

    for (i = 0 ; i < 5 ; i++)
    {
        SDL_DestroyTexture( textures->tex_tank[i] );
        textures->tex_tank[i] = NULL;
    }
    SDL_DestroyTexture( textures->tex_terrain );
    textures->tex_terrain = NULL;
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( ecran );
    renderer= NULL;
    ecran = NULL;
    IMG_Quit();
    SDL_Quit();
}
