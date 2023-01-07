#include "main.h"


int main()
{
    struct game game;
    struct gameGFX gfx;
    
    if(SDL_Init(SDL_INIT_EVERYTHING)) 
    {
	    printf("SDL_Init error: %s\n", SDL_GetError());
	    return 1;
	}
    if(SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &gfx.window, &gfx.renderer))
    {
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		return 1;
	}
    SDL_SetWindowTitle(gfx.window, "Spy Hunter");

    SDL_ShowCursor(SDL_DISABLE);
    SDL_RenderSetLogicalSize(gfx.renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    initializeGame(&game);
    initializeGfx(&gfx);
    if(loadBmp(&gfx,"./cs8x8.bmp"))
    {
        quit(&game, &gfx);
        return 1;
    }
    gfx.scrtex= SDL_CreateTextureFromSurface(gfx.renderer,gfx.charset);
    run(&game, &gfx);

    quit(&game, &gfx);
    return 0;
}



void run(struct game *game, struct gameGFX *gfx)
{
    int currentTime, lastTime = SDL_GetTicks();
    
    while(game->quit!=QUIT)
    {
        sleep(1/1000);
        currentTime=SDL_GetTicks();
        if(framelimit(currentTime, lastTime, game->frameRate))
            continue;
        lastTime=currentTime;
        input(game,gfx);
        drawRoad(*game,gfx);
        if(game->dead==TRUE)
        {
            sleep(1);
            respawn(game);
        }
        drawPlayer(*game,gfx);
        drawEnemy(*game,gfx);
        movement(game);
        update(game);
        collision(game);
        spawnEnemy(game);
    }
}

