#include "main.h"

void initializeGame(struct game *game)
{
    int index;
    memset(game->gameState,0,sizeof(game->gameState));
    memset(game->gameInts,0,sizeof(game->gameInts));

    game->wall = (struct wall*)(malloc(sizeof(struct wall) * 4));
    game->gameInts[WALL_AMMOUNT] = 4;

    initializeObj(&game->player.object, START_POSX, START_POSY, CAR_HEIGHT, CAR_WIDTH);
    initializeObj(&game->wall[0].object, 0, 0, SCREEN_HEIGHT, BORDER);
    initializeObj(&game->wall[1].object, SCREEN_WIDTH - BORDER, 0, SCREEN_HEIGHT, BORDER);

    game->wall[0].type = KILL_WALL;
    game->wall[1].type = KILL_WALL;
    game->wall[2].type = KILL_WALL;
    game->wall[3].type = KILL_WALL;

    
    game->gameInts[PLAYER_LIVES] = STARTING_LIVES; 
    game->gameInts[FRAME_RATE] = BASE_FRAME_RATE;
    game->player.bulletCooldown=SHOOT_COOLDOWN;
    game->gameInts[ENEMY_NEXT_COOLDOWN] = ENEMY_COOLDOWN_START;
    game->gameInts[ENEMY_AMMOUNT_MAX] = MAX_ENEMY_START;
    game->gameInts[ENEMY_SPAWN_MIN_X] = BORDER;
    game->gameInts[ENEMY_SPAWN_MAX_X] = SCREEN_WIDTH-BORDER;

    game->enemy = (struct enemy *)(malloc(MAX_ENEMY * sizeof(struct enemy)));
    for(index = 0; index < MAX_ENEMY; ++index)
        game->enemy[index].dead = TRUE;

    game->gameInts[BULLET_AMMOUNT] = 0;
    game->bullet = (struct bullet *)(malloc(MAX_BULLETS * sizeof(struct bullet)));
    for(index = 0; index < MAX_BULLETS; ++index)
        game->bullet[index].dead = TRUE;
}

void initializeObj(struct object *obj,int posX,int posY,int height,int width)
{
    obj->pos.x = posX;
    obj->pos.y = posY;
    obj->pos.h = height;
    obj->pos.w = width;
    obj->lastPosX = posX;
    obj->lastPosY = posY;
}

void initializeGfx(struct gameGFX *gfx)
{

    SDL_SetRenderDrawColor( gfx->renderer, 0, 0, 0, 0 );
    SDL_RenderFillRect( gfx->renderer, NULL );
    SDL_RenderPresent(gfx->renderer);
}

char loadBmp(struct gameGFX *gfx, const char *file)
{
    gfx->charset = SDL_LoadBMP(file);
	if(gfx->charset == NULL) 
    {
		printf("SDL_LoadBMP(%s) error: %s\n",file, SDL_GetError());
		return 1;
	}
	SDL_SetColorKey(gfx->charset, 1, 0x000000);
    return 0;
}