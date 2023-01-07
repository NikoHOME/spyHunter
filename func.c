#include "main.h"

// Check if time is greater than framerate
char framelimit(int currentTime, int lastTime, int frameRate)
{
    if( (currentTime - lastTime) * frameRate < 1000 ) // 1000 = 1 second 
        return 1;
    return 0;
}


void initializeObj(struct object *obj,int posX,int posY,int height,int width)
{
    obj->pos.x=posX;
    obj->pos.y=posY;
    obj->pos.h=height;
    obj->pos.w=width;
    obj->lastPosX=posX;
    obj->lastPosY=posY;
}

char checkCollision(struct object obj1, struct object obj2)
{
    //debugInt(MIN(obj1.pos.x+obj1.pos.w,obj2.pos.x+obj2.pos.w));
    //debugInt(MAX(obj1.pos.x,obj2.pos.x));
    //debugChar("\n");
    if(MIN(obj1.pos.x+obj1.pos.w,obj2.pos.x+obj2.pos.w)<=MAX(obj1.pos.x,obj2.pos.x))
        return 0;
    if(MIN(obj1.pos.y+obj1.pos.h,obj2.pos.y+obj2.pos.h)<=MAX(obj1.pos.y,obj2.pos.y))
        return 0;
    if(obj1.lastPosY<obj2.lastPosY-obj2.pos.h)
        return DOWN;
    if(obj2.lastPosY<obj1.lastPosY-obj1.pos.h)
        return UP;
    if(obj1.lastPosX+obj1.pos.w<obj2.lastPosX)
        return RIGHT;
    return LEFT;
    
}

void keyCheck(struct game *game, struct gameGFX *gfx,const Uint8 *keystate, int key, int direction)
{
	if(keystate[key])
    {
        game->move[direction]=1;
    }
    else 
    {
        game->move[direction]=0;
    }
}


void input(struct game *game, struct gameGFX *gfx)
{
    
    while(SDL_PollEvent(&gfx->event)) 
    {
		switch((gfx->event).type) 
        {
			case SDL_KEYDOWN: case SDL_KEYUP:
                const Uint8 *keystate = SDL_GetKeyboardState(NULL);
				keyCheck(game,gfx,keystate,SDL_SCANCODE_UP,UP);
                keyCheck(game,gfx,keystate,SDL_SCANCODE_DOWN,DOWN);
                keyCheck(game,gfx,keystate,SDL_SCANCODE_LEFT,LEFT);
                keyCheck(game,gfx,keystate,SDL_SCANCODE_RIGHT,RIGHT);
                break;
			case SDL_QUIT:
                game->quit=QUIT;
				break;
		}
	}
}

void movement(struct game *game)
{
    game->player.object.lastPosX=game->player.object.pos.x;
    game->player.object.lastPosY=game->player.object.pos.y;
    if(game->move[UP] && game->player.object.pos.y>=SCREEN_HEIGHT/2)
    {
        game->player.object.pos.y-=CAR_BASE_SPEED;
        game->velocity+=CAR_BASE_SPEED;
    }
    if(game->player.object.pos.y>=START_POSY)
        return;
    if(game->move[DOWN])
    {
        game->player.object.pos.y+=CAR_BASE_SPEED/2;
        game->velocity-=CAR_BASE_SPEED/2;
    }
    if(game->move[LEFT])
    {
        game->player.object.pos.x-=CAR_BASE_SPEED;
    }
    if(game->move[RIGHT])
    {
        game->player.object.pos.x+=CAR_BASE_SPEED;
    }
}

void collision(struct game *game)
{
    int i;
    char j;
    for(i=0;i<game->wallAmmount;++i)
    {
        if(!(j=checkCollision(game->player.object, game->wall[i].object)))
            continue;
        if(game->wall[i].type==KILL_WALL)
        {
            game->dead=TRUE;
            continue;
        }
    }
}

void update(struct game *game)
{
    game->distance += game->velocity;
    if(game->distance >= SCORE_DISTANCE)
    {
        game->distance %= SCORE_DISTANCE;
        game->score += SCORE_PER_DISTANCE;
    }
}

void spawnEnemy(struct game *game)
{
    if(game->enemyAmmount>=game->enemyMax)
        return;
    int spawnPositionX=game->enemySpawnMinX + rand()%(game->enemySpawnMaxX - game->enemySpawnMinX);
    struct object newEnemy;
    if(game->velocity>40)
        initializeObj(&newEnemy,spawnPositionX,-CAR_HEIGHT/2,CAR_HEIGHT,CAR_WIDTH);
    else
        initializeObj(&newEnemy,spawnPositionX,SCREEN_HEIGHT-CAR_HEIGHT/2,CAR_HEIGHT,CAR_WIDTH);
    int i;
    char j;
    for(i=0;i<game->enemyAmmount;++i)
    {
        if(j=checkCollision(newEnemy, game->enemy[i].object))
            return;
    }
    game->enemy[game->enemyAmmount].object=newEnemy;
    ++game->enemyAmmount;
}

void respawn(struct game *game)
{
    initializeObj(&game->player.object,START_POSX,START_POSY,CAR_HEIGHT,CAR_WIDTH);
    game->velocity=0;
    game->dead=FALSE;
}


void initializeGame(struct game *game)
{
    game->quit=0;
    game->dead=0;
    game->wall = (struct wall*)(malloc(sizeof(struct wall)*2));
    game->wallAmmount=2;
    initializeObj(&game->player.object,START_POSX,START_POSY,CAR_HEIGHT,CAR_WIDTH);
    initializeObj(&game->wall[0].object,BORDER-CAR_WIDTH*3/2,0,SCREEN_HEIGHT,CAR_WIDTH);
    initializeObj(&game->wall[1].object,SCREEN_WIDTH-BORDER+CAR_WIDTH*2/3,0,SCREEN_HEIGHT,CAR_WIDTH);
    game->wall[0].type=KILL_WALL;
    game->wall[1].type=KILL_WALL;
    
    game->velocity=0;
    game->score=0;
    game->distance=0;
    game->frameRate=FRAME_RATE;
    
    game->enemyAmmount=0;
    game->enemyCooldown=ENEMY_COOLDOWN_START;
    game->enemyNext=ENEMY_COOLDOWN_START;
    game->enemyMax=MAX_ENEMY_START;
    game->enemySpawnMinX=BORDER;
    game->enemySpawnMaxX=SCREEN_WIDTH-BORDER;
    game->enemy = (struct enemy *)(malloc(MAX_ENEMY*sizeof(struct enemy)));
    memset(game->move,0,sizeof(game->move));
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

void quit(struct game *game, struct gameGFX *gfx)
{
	SDL_DestroyTexture(gfx->scrtex);
	SDL_DestroyWindow(gfx->window);
	SDL_DestroyRenderer(gfx->renderer);
    free(game->wall);
    free(game->enemy);
    SDL_Quit();
}