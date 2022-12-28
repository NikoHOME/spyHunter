#include "main.h"


int main()
{
    struct game game;
    struct gameGFX gameGFX;
    
    if(SDL_Init(SDL_INIT_EVERYTHING)) 
    {
	    printf("SDL_Init error: %s\n", SDL_GetError());
	    return 1;
	}
    if(SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &gameGFX.window, &gameGFX.renderer))
    {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	}
    SDL_SetWindowTitle(gameGFX.window, "Spy Hunter");

    SDL_ShowCursor(SDL_DISABLE);

	run(&game,&gameGFX);

    SDL_Quit();
    return 0;
}



void run(struct game *game, struct gameGFX *gfx)
{
    char quit=0;
    int currentTime, lastTime = SDL_GetTicks();
    int frameRate=60;
    initializeObj(&(*game).player.object,300,250,40,20);

    gfx->screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    SDL_SetRenderDrawColor( gfx->renderer, 0, 0, 0, 0 );
    SDL_RenderFillRect( gfx->renderer, NULL );
    SDL_RenderPresent(gfx->renderer);
    
    while(quit!=QUIT)
    {
        currentTime=SDL_GetTicks();
        if(framelimit(currentTime,lastTime))
            continue;
        lastTime=currentTime;
        draw(*game,gfx);
        input(game,gfx,&quit);
        movement(game);
    }
}

