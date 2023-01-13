#include "main.h"

// Check if gameInts[TIME] is greater than gameInts[FRAME_RATE]
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
        game->gameState[direction] = TRUE;
    }
    else 
    {
        game->gameState[direction] = FALSE;
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
                    if(game->gameState[IS_PAUSED])
                    {
                        game->gameState[IS_PAUSED] = FALSE;
                        return;
                    }
                    game->gameState[IS_PAUSED] = TRUE;
                    drawPause(*game,gfx);
                }
                if(keystate[SHOOT_KEY])
                {
                    game->gameState[IS_SHOOTING] = TRUE;
                }
                else
                    game->gameState[IS_SHOOTING] = FALSE;
                if(keystate[QUIT_KEY])
                    game->gameState[QUIT] = TRUE;
                if(keystate[NEW_GAME_KEY])
                {
                    game->gameState[QUIT] = TRUE;
                    game->gameState[NEW_GAME]= TRUE;
                }
                if(keystate[END_GAME_KEY] && game->gameState[END_GAME] == FALSE)
                {
                    game->gameState[QUIT] = TRUE;
                    game->gameState[END_GAME] = TRUE;
                }
                if(keystate[SAVE_KEY])
                {
                    saveInput(game, gfx);
                }
                if(keystate[LOAD_KEY])
                {
                    loadInput(game, gfx);
                }
				keyCheck(game, gfx, keystate, SDL_SCANCODE_UP, MOVE_UP);
                keyCheck(game, gfx, keystate, SDL_SCANCODE_DOWN, MOVE_DOWN);
                keyCheck(game, gfx, keystate, SDL_SCANCODE_LEFT, MOVE_LEFT);
                keyCheck(game, gfx, keystate, SDL_SCANCODE_RIGHT, MOVE_RIGHT);
                break;
			case SDL_QUIT:
                game->gameState[QUIT] = TRUE;
				break;
		}
	}
}

void inputCompute(struct game *game)
{
    if(game->gameState[IS_FROZEN])
        return;
    if(game->gameState[IS_SHOOTING])
        spawnBullet(game, FALSE, PLAYER_BULLET, FALSE);

    game->player.object.lastPosX = game->player.object.pos.x;
    game->player.object.lastPosY = game->player.object.pos.y;

    if(game->gameState[MOVE_UP] && game->player.object.pos.y >= SCREEN_HEIGHT/2)
    {
        game->player.object.pos.y -= CAR_BASE_SPEED;
        game->gameInts[VELOCITY] += CAR_BASE_SPEED;
    }
    if(game->player.object.pos.y >= START_POSY)
        return;

    if(game->gameState[MOVE_DOWN])
    {
        game->player.object.pos.y += CAR_BASE_SPEED/2;
        game->gameInts[VELOCITY] -= CAR_BASE_SPEED/2;
    }
    if(game->gameState[MOVE_LEFT])
        game->player.object.pos.x -= CAR_BASE_SPEED;

    if(game->gameState[MOVE_RIGHT])
        game->player.object.pos.x += CAR_BASE_SPEED;
    
}

void collision(struct game *game)
{
    int index, index2;
    char direction;
    for(index=0; index<game->gameInts[WALL_AMMOUNT]; ++index)
    {
        if(!(direction = checkCollision(game->player.object, game->wall[index].object)))
            continue;
        if(game->wall[index].type == KILL_WALL)
        {
            game->gameState[DEAD] = TRUE;
            continue;
        }
    }
    for(index=0; index < MAX_ENEMY; ++index)
    {
        if(game->enemy[index].dead)
            continue;
        for(index2=0; index2 < game->gameInts[WALL_AMMOUNT]; ++index2)
        {
            if(!(direction = checkCollision(game->enemy[index].object, game->wall[index2].object)))
                continue;
            if(game->wall[index2].type == KILL_WALL)
            {
                game->enemy[index].dead = TRUE;
                --game->gameInts[ENEMY_AMMOUNT];
                continue;
            }
        }
        for(index2=0; index2 < MAX_ENEMY; ++index2)
        {
            if(index == index2)
                continue;
            if(!(direction = checkCollision(game->enemy[index].object, game->enemy[index2].object)))
                continue;
            enemyCollision(game, direction, index, index2);
        }
        for(index2=0; index2 < MAX_BULLETS; ++index2)
        {
            if(game->bullet[index].dead)
                continue;
            if(checkCollision(game->enemy[index].object, game->bullet[index2].object))
            {
                if(!game->enemy[index].isArmoured)
                {
                    if(game->enemy[index].isCivilian)
                    {
                        game->gameState[KILLED_CIVILIAN] = TRUE;
                        game->gameInts[PENALTY_END] = 0;
                    }
                    else
                    {
                    }
                    game->enemy[index].dead = TRUE;
                    game->bullet[index2].dead = TRUE;
                    --game->gameInts[ENEMY_AMMOUNT];
                    --game->gameInts[BULLET_AMMOUNT];
                    continue;
                }
                game->bullet[index2].dead = TRUE;
                --game->gameInts[BULLET_AMMOUNT];
            }
        }
        if(!(direction = checkCollision(game->player.object, game->enemy[index].object)))
            continue;
        playerCollision(game, direction, index);
    }
}

void enemyCollision(struct game *game, char direction, int index, int index2)
{
    switch(direction)
    {
        case UP:
            if(abs(game->enemy[index].speed - game->enemy[index2].speed) >= KILL_SPEED)
            {    
                game->enemy[index].dead = TRUE;
            }
            else
            {
                if(game->enemy[index].object.pos.x <= game->enemy[index2].object.pos.x)
                {
                    game->enemy[index].object.pos.x -= CAR_BASE_SPEED*5;
                    break;
                }
                game->enemy[index].object.pos.x += CAR_BASE_SPEED*5;
                game->enemy[index2].object.pos.y -= CAR_BASE_SPEED*2;
            }
            break;
        case DOWN:
            if(abs(game->enemy[index].speed - game->enemy[index2].speed) >= KILL_SPEED)
            {
                game->enemy[index2].dead = TRUE;
            }
            else
            {
                if(game->enemy[index].object.pos.x <= game->enemy[index2].object.pos.x)
                {
                    game->enemy[index].object.pos.x -= CAR_BASE_SPEED*5;
                    break;
                }
                game->enemy[index].object.pos.x += CAR_BASE_SPEED*5;
                game->enemy[index2].object.pos.y += CAR_BASE_SPEED*2;
            }
            break;
        case LEFT:
            game->enemy[index].object.pos.x += CAR_BASE_SPEED*5;
            game->enemy[index2].object.pos.x -= CAR_BASE_SPEED*5;             
            break;
        case RIGHT:
            game->enemy[index].object.pos.x -= CAR_BASE_SPEED*5;
            game->enemy[index2].object.pos.x += CAR_BASE_SPEED*5;           
            break;
    }
}


void playerCollision(struct game *game, char direction, int index)
{
    switch(direction)
    {
        case UP:
            if(game->gameInts[VELOCITY] >= KILL_SPEED)
            {    
                game->gameState[DEAD] = TRUE;
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
            if(game->gameInts[VELOCITY] >= CIVILIAN_SPEED*2)
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
    ++game->gameInts[TICKS];
    ++game->player.bulletNext;
    game->gameInts[DISTANCE] += game->gameInts[VELOCITY];
    if(game->gameInts[DISTANCE] >= SCORE_DISTANCE && !game->gameState[KILLED_CIVILIAN])
    {
        game->gameInts[DISTANCE] %= SCORE_DISTANCE;
        game->gameInts[SCORE] += SCORE_PER_DISTANCE;
        if(game->gameInts[PLAYER_LIVES]<STARTING_LIVES)
            if(game->gameInts[SCORE] - game->gameInts[LAST_DEATH_SCORE] > SCORE_PER_CAR)
            {
                game->gameInts[LAST_DEATH_SCORE] = game->gameInts[SCORE];
                ++game->gameInts[PLAYER_LIVES];
            }
        
    }
    if(game->gameState[TRANSITION] == TRUE)
    {
        game->wall[0].object.pos.y += game->gameInts[VELOCITY]/10;
        game->wall[1].object.pos.y += game->gameInts[VELOCITY]/10;
        game->wall[2].object.pos.h += game->gameInts[VELOCITY]/10;
        game->wall[3].object.pos.h += game->gameInts[VELOCITY]/10;
        game-> gameInts[TRANSITION_Y] += game->gameInts[VELOCITY]/10;
        if(game->gameInts[TRANSITION_Y] >= SCREEN_HEIGHT)
        {
            game->gameState[TRANSITION] = FALSE;
            game->wall[0] = game->wall[2];
            game->wall[1] = game->wall[3];
        }
    }
    if(game->gameInts[TICKS] >= game->gameInts[FRAME_RATE])
    {
        game->gameInts[TICKS] = 0;
        ++game->gameInts[TIME];
        ++game->gameInts[ENEMY_NEXT];
        if(!game->gameState[TRANSITION]) 
            changeRoad(game);
        if(game->gameInts[KILLED_CIVILIAN])
        {
            ++game->gameInts[PENALTY_END];
            if(game->gameInts[PENALTY_END] >= KILLED_CIVILIAN_PENALTY)
                game->gameState[KILLED_CIVILIAN] = FALSE;
        }
        if(game->gameInts[TIME] <= 0)
            game->gameState[END_GAME] = TRUE;
        else if(game->gameInts[TIME] <= 90 )
            game->gameInts[ENEMY_AMMOUNT_MAX] = 3;
    }
    int index;
    for(index = 0; index < MAX_ENEMY; ++index)
    {
        if(game->enemy[index].dead)
            continue;
        game->enemy[index].object.lastPosX = game->enemy[index].object.pos.x;
        game->enemy[index].object.lastPosY = game->enemy[index].object.pos.y;
        if(game->enemy[index].object.pos.y > SCREEN_HEIGHT + CAR_HEIGHT)
        {
            --game->gameInts[ENEMY_AMMOUNT];
            game->enemy[index].dead = TRUE;
            continue;
        }
        if(game->enemy[index].object.pos.y < -CAR_HEIGHT)
        {
            --game->gameInts[ENEMY_AMMOUNT];
            game->enemy[index].dead = TRUE;
        }
        game->enemy[index].object.pos.y += (game->gameInts[VELOCITY] - game->enemy[index].speed)/10;
    }
    for(index = 0; index < MAX_BULLETS; ++index)
    {
        if(game->bullet[index].dead)
            continue;
        if(game->bullet->direction == DOWN)
        {
            game->bullet[index].object.pos.y += game->bullet[index].speed;
        }
        else
        {
            game->bullet[index].object.pos.y -= game->bullet[index].speed;
        }
        game->bullet[index].distance += game->bullet[index].speed;
        if(game->bullet[index].distance >= game->bullet[index].maxDistance)
        {
            game->bullet[index].dead = TRUE;
            --game->gameInts[BULLET_AMMOUNT];
        }
    }
}

void changeRoad(struct game *game)
{
    int randomNumber;
    if(rand()%100 <= WIDTH_CHANGE_CHANCE)
    {
        randomNumber = rand() % WIDTH_CHANGE_OFFSET*2 - WIDTH_CHANGE_OFFSET;
        initializeObj(&game->wall[2].object, 0, 0, 0, BORDER + randomNumber);
        initializeObj(&game->wall[3].object, SCREEN_WIDTH - BORDER - randomNumber, 0, 0, BORDER + randomNumber);
        game->gameState[TRANSITION] = TRUE;
        game->gameInts[TRANSITION_Y] = 0;
    }
}

void spawnEnemy(struct game *game)
{
    if(game->gameInts[ENEMY_AMMOUNT] >= game->gameInts[ENEMY_AMMOUNT_MAX])
        return;
    if(game->gameInts[ENEMY_NEXT] < game->gameInts[ENEMY_NEXT_COOLDOWN])
        return;
    if(game->gameState[IS_FROZEN])
        return;
    int spawnPositionX = game->gameInts[ENEMY_SPAWN_MIN_X] + rand()%(game->gameInts[ENEMY_SPAWN_MAX_X] - game->gameInts[ENEMY_SPAWN_MIN_X] );
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
    if(game->gameInts[VELOCITY] >= CIVILIAN_SPEED)
    {
        initializeObj(&newEnemy.object, spawnPositionX, -CAR_HEIGHT/2, CAR_HEIGHT, CAR_WIDTH);
        newEnemy.speed = game->gameInts[VELOCITY] - CAR_BASE_SPEED*6;
    }
    else
    {
        initializeObj(&newEnemy.object, spawnPositionX, SCREEN_HEIGHT-CAR_HEIGHT/2, CAR_HEIGHT, CAR_WIDTH);
        newEnemy.speed = game->gameInts[VELOCITY] + CAR_BASE_SPEED*4;
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
    ++game->gameInts[ENEMY_AMMOUNT];
    game->gameInts[ENEMY_NEXT]=0;
}
void spawnBullet(struct game *game, int ownerIndex, char owner, char direction)
{
    int index;
    if(game->gameInts[BULLET_AMMOUNT] >= MAX_BULLETS)
        return;
    for(index = 0; index < MAX_BULLETS; ++index)
    {
        if(game->bullet[index].dead)
            break;
    }
    if(owner == PLAYER_BULLET)
    {
        if(game->player.bulletNext < game->player.bulletCooldown)
            return;
        game->player.bulletNext = 0;
        game->bullet[index].direction = UP;
        initializeObj(&game->bullet[index].object, game->player.object.pos.x + CAR_WIDTH/4, game->player.object.pos.y - BULLET_HEIGHT, BULLET_HEIGHT, BULLET_WIDTH);
    }
    else
    {
        game->bullet[index].direction = DOWN;
        initializeObj(&game->bullet[index].object, game->enemy[ownerIndex].object.pos.x + CAR_WIDTH/4, game->enemy[ownerIndex].object.pos.y + CAR_HEIGHT + BULLET_HEIGHT, BULLET_HEIGHT, BULLET_WIDTH);
    }
    game->bullet[index].distance = 0;
    game->bullet[index].maxDistance = BULLET_BASE_SPEED * 10;
    game->bullet[index].dead = FALSE;
    game->bullet[index].speed = BULLET_BASE_SPEED;
    game->bullet[index].object.lastPosX = game->bullet[index].object.lastPosY = 0;
    ++game->gameInts[BULLET_AMMOUNT];
}
void respawn(struct game *game)
{
    if(game->gameInts[TIME] - game->gameInts[LAST_DEATH] > 5)
        --game->gameInts[PLAYER_LIVES];
    game->gameInts[LAST_DEATH] = game->gameInts[TIME];
    game->gameInts[LAST_DEATH_SCORE] = game->gameInts[SCORE];
    if(game->gameInts[PLAYER_LIVES] <= 0)
    {
        game->gameState[END_GAME] = TRUE;
        game->gameState[QUIT] = TRUE;
    }
    initializeObj(&game->player.object, START_POSX, START_POSY, CAR_HEIGHT, CAR_WIDTH);
    game->gameInts[VELOCITY] = 0;
    game->gameState[DEAD] = FALSE;
}


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

void save(struct game *game, const char name[])
{
    FILE *file = fopen(name, "wb");

    fwrite(&game->player, sizeof(struct player), 1, file);
    fwrite(&game->gameState, sizeof(game->gameState), 1, file);
    fwrite(&game->gameInts, sizeof(game->gameInts), 1, file);

    fwrite(game->wall, sizeof(struct wall), WALL_AMMOUNT, file);
    fwrite(game->enemy, sizeof(struct enemy), game->gameInts[ENEMY_AMMOUNT], file);
    fwrite(game->bullet, sizeof(struct bullet), MAX_BULLETS, file);
    fclose(file);
}

void load(struct game *game, struct wall *wall, struct enemy *enemy, struct bullet *bullet, const char name[])
{
    
    FILE *file = fopen(name, "rb");

    fread(&game->player, sizeof(struct player), 1, file);
    fread(&game->gameState, sizeof(game->gameState), 1, file);
    fread(&game->gameInts, sizeof(game->gameInts), 1, file);
    
    //free(wall);
    //wall = (struct wall*)(malloc(sizeof(struct wall) * length));
    fread(wall, sizeof(struct wall), WALL_AMMOUNT, file);
    //free(enemy);
    //enemy = (struct enemy *)(malloc(sizeof(struct enemy) * length));
    fread(enemy, sizeof(struct enemy), game->gameInts[ENEMY_AMMOUNT], file);

    //free(bullet);
    //bullet = (struct bullet *)(malloc(sizeof(struct bullet) * length));
    fread(bullet, sizeof(struct bullet), MAX_BULLETS, file);

    fclose(file);
}



void saveInput(struct game *game, struct gameGFX *gfx)
{
    
    FILE *file;
    char input[MAX_FILENAME_SIZE+1], inputf[MAX_FILENAME_SIZE+1];
    input[0]=' ';
    input[1]='\0';
    int nameSize = 0, index;
    char exitWrite = FALSE;
    drawSave(gfx, input);
    const char textSave[] = "Save already exists";
    int currentTime, lastTime = SDL_GetTicks();
    while(!exitWrite)
    {
        currentTime = SDL_GetTicks();
        usleep(framelimit(currentTime, lastTime, game->gameInts[FRAME_RATE]));
        lastTime = currentTime;
        SDL_PollEvent(&gfx->event);
		switch((gfx->event).type) 
        {
			case SDL_KEYDOWN:
                const Uint8 *keystate = SDL_GetKeyboardState(NULL);
                if(keystate[QUIT_KEY])
                {
                    exitWrite = 2;
                }
                if(keystate[CONFIM_KEY])
                {
                    snprintf(inputf, sizeof(inputf), "save/%s.save", input);
                    if(file = fopen(inputf, "r"))
                    {
                        drawWhiteText(gfx, SCREEN_WIDTH/2-strlen(textSave)*TEXT_SIZE, SCREEN_HEIGHT/2+TEXT_SIZE*3, 2, 2, textSave);
                    }
                    else
                        exitWrite = TRUE;
                }
                for(index = SDL_SCANCODE_1; index <= SDL_SCANCODE_0; ++index)
                {
                    if(keystate[index])
                    {
                        if(index == SDL_SCANCODE_0)
                            input[nameSize]='0';
                        else
                            input[nameSize]='0'+index-SDL_SCANCODE_1+1;
                        ++nameSize;
                        input[nameSize]='\0';
                        drawSave(gfx,input);
                    }
                }
                for(index = SDL_SCANCODE_A; index <= SDL_SCANCODE_Z; ++index)
                {
                    if(keystate[index])
                    {
                        input[nameSize]='A'+index-SDL_SCANCODE_A;
                        ++nameSize;
                        input[nameSize]='\0';
                        drawSave(gfx,input);
                    }
                }
                if(keystate[SDL_SCANCODE_BACKSPACE])
                {
                    if(nameSize>0)
                    {
                        nameSize--;
                        input[nameSize]='\0';
                        drawSave(gfx,input);
                    }
                }
                break;
            case SDL_QUIT:
                game->gameState[QUIT] = TRUE;
				break;
        }
    }
    if(exitWrite == TRUE)
        save(game, inputf);
    game->gameState[IS_PAUSED] = TRUE;
    drawPause(*game,gfx);
}

void loadInput(struct game *game, struct gameGFX *gfx)
{
    FILE *file;
    DIR *dir = opendir("save");
    struct dirent *read;
    int fileNumber = 0, index = 0;
    while((read = readdir(dir)) != NULL) 
    {
        if ( !strcmp(read->d_name, ".") || !strcmp(read->d_name, "..") )
            continue;
        ++fileNumber;
    }
    rewinddir(dir);
    char *filesList[fileNumber];
    while((read = readdir(dir)) != NULL) {
        if ( !strcmp(read->d_name, ".") || !strcmp(read->d_name, "..") )
            continue;
        filesList[index] = (char*) malloc (strlen(read->d_name)+1);
        strncpy (filesList[index],read->d_name, strlen(read->d_name) );
        filesList[index] = read->d_name;
        ++index;
    }
    rewinddir(dir);

    char inputf[MAX_FILENAME_SIZE+1];
    drawLoad(gfx, filesList, 0, fileNumber);
    char exitWrite = FALSE;
    int select = 0;
    int currentTime, lastTime = SDL_GetTicks();
    while(!exitWrite)
    {
        currentTime = SDL_GetTicks();
        usleep(framelimit(currentTime, lastTime, game->gameInts[FRAME_RATE]));
        lastTime = currentTime;
        SDL_PollEvent(&gfx->event);
		switch((gfx->event).type) 
        {
			case SDL_KEYDOWN:
                const Uint8 *keystate = SDL_GetKeyboardState(NULL);
                if(keystate[QUIT_KEY])
                {
                    exitWrite = 2;
                }
                if(keystate[CONFIM_KEY])
                {
                    exitWrite = TRUE;
                }
                if(keystate[SDL_SCANCODE_UP])
                {
                    if(select > 0)
                        --select;
                    drawLoad(gfx, filesList, select, fileNumber);
                }
                if(keystate[SDL_SCANCODE_DOWN])
                {
                    if(select < fileNumber)
                        ++select;
                    drawLoad(gfx, filesList, select, fileNumber);
                }   
                break;
            case SDL_QUIT:
                game->gameState[QUIT] = TRUE;
				break;
        }
    }
    snprintf(inputf, sizeof(inputf), "save/%s", filesList[select]);
    load(game, game->wall, game->enemy, game->bullet, inputf);
}

void quit(struct game *game, struct gameGFX *gfx)
{
	SDL_DestroyTexture(gfx->scrtex);
	SDL_DestroyWindow(gfx->window);
	SDL_DestroyRenderer(gfx->renderer);
    free(game->wall);
    free(game->enemy);
    free(game->bullet);
    SDL_Quit();
}