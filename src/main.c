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
    loadBmp(&gfx, "./bmp/sh8x8.bmp");
    SDL_SetColorKey(gfx.charset, 1, 0x000000);
    //Load Textures
    gfx.objectTexture[TEXTURE_PLAYER] = IMG_LoadTexture(gfx.renderer, "./bmp/player.png");
    gfx.objectTexture[TEXTURE_ENEMY] = IMG_LoadTexture(gfx.renderer, "./bmp/enemy.png");
    gfx.objectTexture[TEXTURE_BOX] = IMG_LoadTexture(gfx.renderer, "./bmp/box.png");
    gfx.objectTexture[TEXTURE_BULLET] = IMG_LoadTexture(gfx.renderer, "./bmp/bullet.png");
    gfx.objectTexture[TEXTURE_POWERUP] = IMG_LoadTexture(gfx.renderer, "./bmp/powerup.png");
    gfx.objectTexture[TEXTURE_ARMOUR] = IMG_LoadTexture(gfx.renderer, "./bmp/armour.png");
    gfx.objectTexture[TEXTURE_CIVIL] = IMG_LoadTexture(gfx.renderer, "./bmp/civilian.png");

    //Convert to texture
    gfx.scrtex = SDL_CreateTextureFromSurface(gfx.renderer,gfx.charset); 
    
    game.gameState[NEW_GAME] = TRUE;
    //Game loop
    while(game.gameState[NEW_GAME] == TRUE)
    {
        initializeGame(&game);
        run(&game, &gfx);
        freeGame(&game);
    }
    freeGFX(&gfx);
    SDL_Quit();
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
    //End game screen
    drawEndScreen(*game, gfx);
    game->gameState[QUIT] = FALSE;
    while(game->gameState[QUIT] != TRUE)
    {
        sleepTillNextFrame(game, &currentTime, &lastTime);
        input(game, gfx);
    }

}

