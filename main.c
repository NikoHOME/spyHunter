#include "main.h"


int main()
{
    struct game game;      
    struct gameGFX gfx; //SDL graphics
    
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

    initializeGfx(&gfx); 
    //Create charset
    if(loadBmp(&gfx,"./sh8x8.bmp"))
    {
        quit(&game, &gfx);
        return 1;
    }
    //Convert to texture
    gfx.scrtex= SDL_CreateTextureFromSurface(gfx.renderer,gfx.charset); 
    
    game.gameState[NEW_GAME] = TRUE;
    
    //Game loop
    while(game.gameState[NEW_GAME] == TRUE)
    {
        initializeGame(&game);
        run(&game, &gfx);
    }
    quit(&game, &gfx);
    return 0;
}



void run(struct game *game, struct gameGFX *gfx)
{
    int currentTime, respawnTime, lastTime = SDL_GetTicks();
    
    while(game->gameState[QUIT] != TRUE)
    {
        sleepTillNextFrame(game, &currentTime, &lastTime);
        input(game, gfx);
        if(game->gameState[IS_PAUSED])
            continue;
        handleRespawn(game, gfx, &currentTime, &lastTime);
        drawMain(*game,gfx);
        inputCompute(game);
        update(game);
        collision(game);
        spawnEnemy(game);
    }

    if(game->gameState[END_GAME] != TRUE)
        return;
    drawEndScreen(*game, gfx);
    game->gameState[QUIT] = FALSE;
    while(game->gameState[QUIT] != TRUE)
    {
        sleepTillNextFrame(game, &currentTime, &lastTime);
        input(game, gfx);
    }


}

