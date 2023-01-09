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

    initializeGfx(&gfx);
    if(loadBmp(&gfx,"./cs8x8.bmp"))
    {
        quit(&game, &gfx);
        return 1;
    }
    gfx.scrtex= SDL_CreateTextureFromSurface(gfx.renderer,gfx.charset);
    game.newGame=TRUE;
    while(game.newGame==TRUE)
    {
        initializeGame(&game);
        run(&game, &gfx);
    }
    quit(&game, &gfx);
    return 0;
}



void run(struct game *game, struct gameGFX *gfx)
{
    int currentTime, respawnTime,lastTime = SDL_GetTicks();
    
    while(game->quit!=TRUE)
    {
        currentTime=SDL_GetTicks();
        usleep(framelimit(currentTime, lastTime, game->frameRate));
        lastTime=currentTime;
        input(game,gfx);
        if(game->isPaused)
            continue;
        drawRoad(*game,gfx);
        if(game->dead==TRUE)
        {
            respawn(game);
            respawnTime=1000;
            game->isFrozen=TRUE;
            while(1)
            {
                currentTime=SDL_GetTicks();
                usleep(framelimit(currentTime, lastTime, game->frameRate));
                lastTime=currentTime;
                input(game,gfx);
                if(game->isPaused)
                    continue;
                drawRoad(*game,gfx);
                update(game);
                drawPlayer(*game,gfx);
                drawEnemy(*game,gfx);
                collision(game);
                respawnTime-=(1000/game->frameRate);
                if(respawnTime<=0)
                    break;
            }
            game->isFrozen=FALSE;
        }
        drawPlayer(*game,gfx);
        drawEnemy(*game,gfx);
        movement(game);
        update(game);
        collision(game);
        spawnEnemy(game);
    }

    if(game->endGame!=TRUE)
        return;
    drawEndScreen(*game,gfx);
    game->quit=FALSE;
    while(game->quit!=TRUE)
    {
        currentTime=SDL_GetTicks();
        usleep(framelimit(currentTime, lastTime, game->frameRate));
        lastTime=currentTime;
        input(game,gfx);
    }


}

