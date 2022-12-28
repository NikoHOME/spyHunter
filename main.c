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
    free(game.wall);
    SDL_Quit();
    return 0;
}



void run(struct game *game, struct gameGFX *gfx)
{
    game->quit=0;
    game->dead=0;
    int currentTime, lastTime = SDL_GetTicks();
    int frameRate=60;
    game->wall = (struct wall*)(malloc(sizeof(struct wall)*2));
    game->wallAmmount=2;
    initializeObj(&game->player.object,START_POSX,START_POSY,CAR_HEIGHT,CAR_WIDTH);
    initializeObj(&game->wall[0].object,BORDER-CAR_WIDTH*3/2,0,SCREEN_HEIGHT,CAR_WIDTH);
    initializeObj(&game->wall[1].object,SCREEN_WIDTH-BORDER+CAR_WIDTH*2/3,0,SCREEN_HEIGHT,CAR_WIDTH);
    game->wall[0].type=KILL_WALL;
    game->wall[1].type=KILL_WALL;
    
    gfx->screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32, 0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

    SDL_SetRenderDrawColor( gfx->renderer, 0, 0, 0, 0 );
    SDL_RenderFillRect( gfx->renderer, NULL );
    SDL_RenderPresent(gfx->renderer);
    
    while(game->quit!=QUIT)
    {
        sleep(1/1000);
        currentTime=SDL_GetTicks();
        if(framelimit(currentTime,lastTime))
            continue;
        lastTime=currentTime;
        draw(*game,gfx);
        input(game,gfx);
        if(game->dead==DEAD)
            continue;
        movement(game);
        collision(game);
    }
}

