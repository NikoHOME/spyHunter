#include "main.h"

// Check if time is greater than framerate
int framelimit(int currentTime, int lastTime, int frameRate)
{
    int sleep = (1000/frameRate) - (currentTime - lastTime); // 1000 = 1 second 
    if(sleep>0) 
        return sleep*1000;
    return FALSE;
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

char checkCollision(struct object obj1, struct object obj2)
{
    if(MIN(obj1.pos.x + obj1.pos.w, obj2.pos.x + obj2.pos.w) <= MAX(obj1.pos.x, obj2.pos.x))
        return FALSE;
    if(MIN(obj1.pos.y + obj1.pos.h, obj2.pos.y + obj2.pos.h) <= MAX(obj1.pos.y, obj2.pos.y))
        return FALSE;
    if(obj1.lastPosY + obj1.pos.h <= obj2.lastPosY || obj1.pos.y + obj1.pos.h <= obj2.pos.y)
        return DOWN;
    if(obj2.lastPosY + obj2.pos.h <= obj1.lastPosY || obj2.pos.y + obj2.pos.h <= obj1.pos.y)
        return UP;
    if(obj1.pos.x <= obj2.pos.x)
        return RIGHT;
    return LEFT;
    
}

void keyCheck(struct game *game, struct gameGFX *gfx,const Uint8 *keystate, int key, int direction)
{
	if(keystate[key])
    {
        game->move[direction] = TRUE;
    }
    else 
    {
        game->move[direction] = FALSE;
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
                if(keystate[PAUSE_KEY])
                {
                    if(game->isPaused)
                    {
                        game->isPaused = FALSE;
                        return;
                    }
                    game->isPaused = TRUE;
                    drawPause(*game,gfx);
                }
                if(keystate[SHOOT_KEY])
                {

                }
                if(keystate[QUIT_KEY])
                    game->quit = TRUE;
                if(keystate[NEW_GAME_KEY])
                {
                    game->quit = TRUE;
                    game->newGame = TRUE;
                }
                if(keystate[END_GAME_KEY] && game->endGame == FALSE)
                {
                    game->quit = TRUE;
                    game->endGame = TRUE;
                }
                if(keystate[SAVE_KEY])
                {

                }
                if(keystate[LOAD_KEY])
                {

                }
				keyCheck(game, gfx, keystate, SDL_SCANCODE_UP, UP);
                keyCheck(game, gfx, keystate, SDL_SCANCODE_DOWN, DOWN);
                keyCheck(game, gfx, keystate, SDL_SCANCODE_LEFT, LEFT);
                keyCheck(game, gfx, keystate, SDL_SCANCODE_RIGHT, RIGHT);
                break;
			case SDL_QUIT:
                game->quit = TRUE;
				break;
		}
	}
}

void movement(struct game *game)
{
    if(game->isFrozen)
        return;
    game->player.object.lastPosX = game->player.object.pos.x;
    game->player.object.lastPosY = game->player.object.pos.y;

    if(game->move[UP] && game->player.object.pos.y >= SCREEN_HEIGHT/2)
    {
        game->player.object.pos.y -= CAR_BASE_SPEED;
        game->velocity += CAR_BASE_SPEED;
    }
    if(game->player.object.pos.y >= START_POSY)
        return;

    if(game->move[DOWN])
    {
        game->player.object.pos.y += CAR_BASE_SPEED/2;
        game->velocity -= CAR_BASE_SPEED/2;
    }
    if(game->move[LEFT])
        game->player.object.pos.x -= CAR_BASE_SPEED;

    if(game->move[RIGHT])
        game->player.object.pos.x += CAR_BASE_SPEED;
}

void collision(struct game *game)
{
    int index,index2;
    char direction;
    for(index=0; index<game->wallAmmount; ++index)
    {
        if(!(direction = checkCollision(game->player.object, game->wall[index].object)))
            continue;
        if(game->wall[index].type == KILL_WALL)
        {
            game->dead = TRUE;
            continue;
        }
    }
    for(index=0; index < MAX_ENEMY; ++index)
    {
        if(game->enemy[index].dead)
            continue;
        for(index2=0; index2 < game->wallAmmount; ++index2)
        {
            if(!(direction = checkCollision(game->enemy[index].object, game->wall[index2].object)))
                continue;
            if(game->wall[index2].type == KILL_WALL)
            {
                game->enemy[index].dead = TRUE;
                --game->enemyAmmount;
                continue;
            }
        }
        if(!(direction = checkCollision(game->player.object, game->enemy[index].object)))
            continue;
        playerCollison(game, direction, index);
    }
}

void playerCollison(struct game *game, char direction, int index)
{
    switch(direction)
    {
        case UP:
            if(game->velocity >= KILL_SPEED)
            {    
                game->dead = TRUE;
            }
            else
            {
                if(game->player.object.pos.x <= game->enemy[index].object.pos.x)
                {
                    game->player.object.pos.x -= CAR_BASE_SPEED*5;
                    break;
                }
                game->player.object.pos.x += CAR_BASE_SPEED*5;
                game->enemy[index].object.pos.y -= CAR_BASE_SPEED*2;
            }
            break;
        case DOWN:
            if(game->velocity >= CIVILIAN_SPEED*2)
            {
                game->enemy[index].dead = TRUE;
            }
            else
            {
                if(game->player.object.pos.x <= game->enemy[index].object.pos.x)
                {
                    game->player.object.pos.x -= CAR_BASE_SPEED*5;
                    break;
                }
                game->player.object.pos.x += CAR_BASE_SPEED*5;
                game->enemy[index].object.pos.y += CAR_BASE_SPEED*2;
            }
            break;
        case LEFT:
            game->player.object.pos.x += CAR_BASE_SPEED*5;
            game->enemy[index].object.pos.x -= CAR_BASE_SPEED*5;             
            break;
        case RIGHT:
            game->player.object.pos.x -= CAR_BASE_SPEED*5;
            game->enemy[index].object.pos.x += CAR_BASE_SPEED*5;   
            break;
    }
}

void update(struct game *game)
{
    ++game->ticks;
    game->distance += game->velocity;
    if(game->distance >= SCORE_DISTANCE)
    {
        game->distance %= SCORE_DISTANCE;
        game->score += SCORE_PER_DISTANCE;
    }
    if(game->ticks >= game->frameRate)
    {
        game->ticks = 0;
        ++game->time;
        ++game->enemyNext;
    }
    int i;
    for(i=0;i<MAX_ENEMY;++i)
    {
        if(game->enemy[i].dead)
            continue;
        game->enemy[i].object.lastPosX = game->enemy[i].object.pos.x;
        game->enemy[i].object.lastPosY = game->enemy[i].object.pos.y;
        if(game->enemy[i].object.pos.y > SCREEN_HEIGHT+CAR_HEIGHT)
        {
            --game->enemyAmmount;
            game->enemy[i].dead = TRUE;
            continue;
        }
        if(game->enemy[i].object.pos.y < -CAR_HEIGHT)
        {
            --game->enemyAmmount;
            game->enemy[i].dead = TRUE;
        }
        game->enemy[i].object.pos.y += (game->velocity - game->enemy[i].speed)/10;
    }
}

void spawnEnemy(struct game *game)
{
    if(game->enemyAmmount >= game->enemyMax)
        return;
    if(game->enemyNext < game->enemyCooldown)
        return;
    if(game->isFrozen)
        return;
    int spawnPositionX = game->enemySpawnMinX + rand()%(game->enemySpawnMaxX - game->enemySpawnMinX);
    struct enemy newEnemy;
    if(rand()%100 <= ENEMY_CHANCE)
    {
        newEnemy.isCivilian = FALSE;
        if(rand()%100 <= ENEMY_ARMOUR_CHANCE)
            newEnemy.isArmoured = TRUE;
        else
            newEnemy.isArmoured = FALSE;
    }
    else
    {
        newEnemy.isCivilian = TRUE;
        newEnemy.isArmoured = FALSE;
    }
    if(game->velocity >= CIVILIAN_SPEED)
    {
        initializeObj(&newEnemy.object, spawnPositionX, -CAR_HEIGHT/2, CAR_HEIGHT, CAR_WIDTH);
        newEnemy.speed = game->velocity - CAR_BASE_SPEED*6;
    }
    else
    {
        initializeObj(&newEnemy.object, spawnPositionX, SCREEN_HEIGHT-CAR_HEIGHT/2, CAR_HEIGHT, CAR_WIDTH);
        newEnemy.speed = game->velocity + CAR_BASE_SPEED*4;
    }
    newEnemy.dead = FALSE;
    int index;
    char direction;
    for(index=0; index < MAX_ENEMY; ++index)
    {
        if(game->enemy[index].dead)
            continue;
        if(direction=checkCollision(newEnemy.object, game->enemy[index].object))
            return;
    }
    for(index=0; index < MAX_ENEMY; ++index)
    {
        if(game->enemy[index].dead)
        {
            direction=index;
            break;
        }
    }
    game->enemy[direction]=newEnemy;
    ++game->enemyAmmount;
    game->enemyNext=0;
}

void respawn(struct game *game)
{
    initializeObj(&game->player.object, START_POSX, START_POSY, CAR_HEIGHT, CAR_WIDTH);
    game->velocity = 0;
    game->dead = FALSE;
}


void initializeGame(struct game *game)
{
    game->quit=FALSE;
    game->newGame=FALSE;
    game->endGame=FALSE;
    game->dead=FALSE;
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
    game->time=0;
    game->ticks=0;
    game->frameRate=FRAME_RATE;
    game->isPaused=FALSE;
    game->isFrozen=FALSE;
    
    game->player.canShoot=FALSE;
    game->player.shootCooldown=SHOOT_COOLDOWN;

    game->enemyAmmount=0;
    game->enemyCooldown=ENEMY_COOLDOWN_START;
    game->enemyNext=0;
    game->enemyMax=MAX_ENEMY_START;
    game->enemySpawnMinX=BORDER;
    game->enemySpawnMaxX=SCREEN_WIDTH-BORDER;
    game->enemy = (struct enemy *)(malloc(MAX_ENEMY*sizeof(struct enemy)));
    int i;
    for(i=0;i<MAX_ENEMY;++i)
    {
        game->enemy[i].dead=TRUE;
    }
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