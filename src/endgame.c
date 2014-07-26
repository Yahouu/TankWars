#include "endgame.h"
#include "render.h"
#include "menu.h"

int checkEndGame(Textures *gameTextures, Tile **mapTiles)
{
	Score score1;
	Score score2;

	countTanks(&score1, mapTiles, TANK1, BASE2, 0); 
	countTanks(&score2, mapTiles, TANK2, BASE1, 10);

	if (score1.count == 0 || score1.count == score1.stuck || score2.count == 0 || score2.count == score2.stuck) {
		countPoints(mapTiles, &score1, TANK1, 10);
		countPoints(mapTiles, &score2, TANK2, 0);

		endGame(gameTextures, score1.total, score2.total);
		mapTiles = generateMap();
		return QUIT;
	}

	return 0;
}

void countTanks(Score *score, Tile **mapTiles, int tank, int base, int origin)
{
	for (size_t i=0; i<WIDTH; i++) {
		for (size_t j=0; j<HEIGHT; j++) {
			if (mapTiles[j][i].tank == tank || mapTiles[j][i].tank == tank+2) {
				++score->count;

				if (mapTiles[j][i].tank == tank && mapTiles[j][i].terrain == base) {
					++score->stuck;
				}

				else if ((i!=origin || j!=origin) && mapTiles[j][i].tank == tank+2 && mapTiles[j][i].terrain == base) {
					++score->command;
				}
			}
		}
	}

	if (mapTiles[origin][origin].tank == tank+2) {
		++score->stuck;
	}
}

void countPoints(Tile **mapTiles, Score *score, int tank, int origin)
{
	if (mapTiles[origin][origin].tank == tank) {
		score->total += 3;
		--score->stuck;
		--score->count;
	} else if (score->command == 1) {
		score->total += 2;
		--score->count;
	}

	for ( ; score->stuck > 0; score->stuck--, score->count--) {
		score->total += 2;
	}

	for ( ; score->count > 0; score->count--) {
		score->total += 1;
	}
}

int endGame(Textures *gameTextures, size_t score1, size_t score2) 
{
	EndScreen endscreen;
	SDL_Event event;

	loadScreen(&endscreen, score1, score2);
	SDL_Rect position = {0,0,DIMENSION,DIMENSION};

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, endscreen.tex_background, NULL, &position);

    SDL_QueryTexture(endscreen.tex_game_over, NULL, NULL, &position.w, &position.h);
    writeText(endscreen.tex_game_over, &position, (DIMENSION/2 - position.w/2), 100);
    writeText(endscreen.tex_score1, &position, 50, 200);
    writeText(endscreen.tex_score2, &position, 50, 250);

    SDL_RenderPresent(renderer);

    while (1) {
    	SDL_WaitEvent(&event);
    	if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_RETURN || event.key.keysym.sym == SDLK_ESCAPE) {
    		SDL_DestroyTexture(endscreen.tex_game_over);
    		SDL_DestroyTexture(endscreen.tex_score1);
    		SDL_DestroyTexture(endscreen.tex_score2);
    		SDL_DestroyTexture(endscreen.tex_background);

    		return QUIT;
    	}
    }

    fprintf(stderr, "How did you get there?");
    return QUIT;

}

void loadScreen(EndScreen *endscreen, int score1, int score2)
{
	TTF_Font *font = TTF_OpenFont("../resources/FORCED_SQUARE.ttf", 55);
	SDL_Color black = {0,0,0,0};

	char s_score1[20], s_score2[20];
	sprintf(s_score1, "Player 1: %d points", score1);
	sprintf(s_score2, "Player 2: %d points", score2);

	if (score1 > score2) {
		endscreen->tex_game_over = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, "Player 1 won the game !", black));
	} else if (score2 > score1) {
		endscreen->tex_game_over = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, "Player 2 won the game !", black));
	} else {
		endscreen->tex_game_over = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, "It's a tie !", black));
	}

	font = TTF_OpenFont("../resources/FORCED_SQUARE.ttf", 35);
	endscreen->tex_score1 = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, s_score1, black));
	endscreen->tex_score2 = SDL_CreateTextureFromSurface(renderer, TTF_RenderText_Blended(font, s_score2, black));

	TTF_CloseFont(font);

	endscreen->tex_background = IMG_LoadTexture( renderer, "../resources/img/menu_bg.png");
}
