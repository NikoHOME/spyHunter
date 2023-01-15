#include "main.h"

// Check if gameInts[TIME] is greater than gameInts[FRAME_RATE]
int framelimit(int currentTime, int lastTime, int frameRate)
{
    int sleep = (1000/frameRate) - (currentTime - lastTime); // 1000 = 1 second 
    if(sleep>0) 
        return sleep*1000;
    return FALSE;
}

void sleepTillNextFrame(struct game *game, int *currentTime, int *lastTime)
{
    *currentTime = SDL_GetTicks();
    usleep(framelimit(*currentTime, *lastTime, game->gameInts[FRAME_RATE]));
    *lastTime = *currentTime;
}

struct object viewToPosition(struct enemy enemy, int direction)
{
    struct object output;
    initializeObj
    (
            &output,
            enemy.object.pos.x + enemy.view[direction].pos.x,
            enemy.object.pos.y + enemy.view[direction].pos.y,
            enemy.view[direction].pos.h, 
            enemy.view[direction].pos.w
    );
    return output;
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
        if(game->gameInts[POWERUP_DURATION])
            --game->gameInts[POWERUP_DURATION];
        if(game->gameInts[KILLED_CIVILIAN])
        {
            ++game->gameInts[PENALTY_END];
            if(game->gameInts[PENALTY_END] >= KILLED_CIVILIAN_PENALTY)
                game->gameState[KILLED_CIVILIAN] = FALSE;
        }
        else if(game->gameInts[TIME] >= 90 )
            game->gameInts[ENEMY_AMMOUNT_MAX] = 3;
    }
    int index, index2;
    for(index = 0; index < MAX_ENEMY; ++index)
    {
        if(game->enemy[index].dead)
            continue;
        game->enemy[index].object.lastPosX = game->enemy[index].object.pos.x;
        game->enemy[index].object.lastPosY = game->enemy[index].object.pos.y;
        if(game->enemy[index].object.pos.y > SCREEN_HEIGHT*3/2)
        {
            --game->gameInts[ENEMY_AMMOUNT];
            game->enemy[index].dead = TRUE;
            continue;
        }
        if(game->enemy[index].object.pos.y < -SCREEN_HEIGHT/2)
        {
            --game->gameInts[ENEMY_AMMOUNT];
            game->enemy[index].dead = TRUE;
        }
        game->enemy[index].object.pos.y += (game->gameInts[VELOCITY] - game->enemy[index].speed)/6;
        for(index2 = 0; index2 < game->gameInts[WALL_AMMOUNT]; ++index2)
        {
            if(game->enemy[index].stunned)
            {
                --game->enemy[index].stunned;
                continue;
            }
            if(!checkCollision(viewToPosition(game->enemy[index], UP), game->wall[index2].object))
                continue;
            if(index2 & 1)
            {
                 game->enemy[index].object.pos.x -= CAR_BASE_SPEED/5;
            }
            else
            {
                game->enemy[index].object.pos.x += CAR_BASE_SPEED/5;
            }
        }
        int distance;
        if(game->enemy[index].isArmoured && !game->gameState[IS_FROZEN] && game->gameInts[VELOCITY] > CIVILIAN_SPEED)
        {
            distance = game->player.object.pos.y - game->enemy[index].object.pos.y;
            if(abs(game->enemy[index].speed - game->gameInts[VELOCITY]) > CAR_BASE_SPEED*2)
                game->enemy[index].speed = game->gameInts[VELOCITY] - distance/100;
            if(distance > ATTACK_DISTANCE || distance < ATTACK_DISTANCE/2)
            {
                game->enemy[index].speed -= distance/100;
            }
            if(game->enemy[index].attack)
            {
                --game->enemy[index].attack;
                if(game->enemy[index].attack < ATTACK_DURATION_BEGIN && game->enemy[index].attack > ATTACK_DURATION)
                    game->enemy[index].object.pos.x +=  game->enemy[index].attackPower;
                continue;
            }
            game->enemy[index].attackPower = 0;
            if(checkCollision(viewToPosition(game->enemy[index], LEFT), game->player.object))
            {
                game->enemy[index].attackPower = -CAR_BASE_SPEED;
                game->enemy[index].attack = ATTACK_DELAY;
            }
            if(checkCollision(viewToPosition(game->enemy[index], RIGHT), game->player.object))
            {
                game->enemy[index].attackPower = CAR_BASE_SPEED;
                game->enemy[index].attack = ATTACK_DELAY;
            }
        }
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
    for(index = 0; index < MAX_POWERUP; ++index)
    {
        if(game->powerup[index].dead)
            continue;
        game->powerup[index].object.pos.y += (game->gameInts[VELOCITY] - game->powerup[index].speed)/6;
        game->powerup[index].speed = -game->gameInts[VELOCITY]/10;
    }
}

void changeRoad(struct game *game)
{
    int randomNumber, currentWidth, pushLeft, pushRight;
    if(!game->gameState[TRANSITION] && rand()%100 <= WIDTH_CHANGE_CHANCE)
    {
        randomNumber = rand()%4;
        currentWidth = game->wall[1].object.pos.x - game->wall[0].object.pos.w;
        switch(randomNumber)
        {
            case 0:  // Shrink
                if(currentWidth < WIDTH_MIN )
                    return;
                pushLeft = pushRight = WIDTH_CHANGE_OFFSET;
                break;
            case 1: // Grow
                if(currentWidth > WIDTH_MAX || game->wall[0].object.pos.w < WIDTH_MIN_DIRECTION || game->wall[1].object.pos.w < WIDTH_MIN_DIRECTION)
                    return;
                pushLeft = pushRight = -WIDTH_CHANGE_OFFSET;
                break;
            case 2: //Push left
                if(game->wall[0].object.pos.w < WIDTH_MIN_DIRECTION)
                    return;
                pushLeft = -WIDTH_CHANGE_OFFSET;
                pushRight = WIDTH_CHANGE_OFFSET;
                break;
            case 3: //Push Right
                if(game->wall[1].object.pos.w < WIDTH_MIN_DIRECTION)
                    return;
                pushLeft = WIDTH_CHANGE_OFFSET;
                pushRight = -WIDTH_CHANGE_OFFSET;
                break;
        }

        initializeObj(&game->wall[2].object, 0, 0, 0, game->wall[0].object.pos.w + pushLeft);
        initializeObj(&game->wall[3].object, game->wall[1].object.pos.x - pushRight, 0, 0, game->wall[1].object.pos.w  + pushRight);
        game->gameState[TRANSITION] = TRUE;
        game->gameInts[TRANSITION_Y] = 0;
        game->gameInts[ENEMY_SPAWN_MIN_X] = game->wall[0].object.pos.w + pushLeft;
        game->gameInts[ENEMY_SPAWN_MAX_X] = game->wall[1].object.pos.x - pushRight - CAR_WIDTH;
    }
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
    initializeObj(&game->player.object, (game->gameInts[ENEMY_SPAWN_MAX_X] + game->gameInts[ENEMY_SPAWN_MIN_X] - CAR_WIDTH)/2, START_POSY, CAR_HEIGHT, CAR_WIDTH);
    game->gameInts[VELOCITY] = 0;
    game->gameState[DEAD] = FALSE;
}

void handleRespawn(struct game *game, struct gameGFX *gfx, int *currentTime, int *lastTime)
{
    int respawnTime;
    if(game->gameState[DEAD] == TRUE)
    {
        respawn(game);
        respawnTime = RESPAWN_TIME;
        game->gameState[IS_FROZEN] = TRUE;
        while(1)
        {
            sleepTillNextFrame(game, currentTime, lastTime);
            input(game, gfx);
            if(game->gameState[IS_PAUSED])
                continue;
            drawMain(*game,gfx);
            update(game);
            collision(game);
            respawnTime -= (1000 / game->gameInts[FRAME_RATE]);
            if(respawnTime <= 0)
                break;
        }
        game->gameState[IS_FROZEN] = FALSE;
    }
}



void freeGame(struct game *game)
{
    free(game->wall);
    free(game->enemy);
    free(game->bullet);
}
void freeGFX(struct gameGFX *gfx)
{
	SDL_DestroyTexture(gfx->scrtex);
	SDL_DestroyWindow(gfx->window);
	SDL_DestroyRenderer(gfx->renderer);    
}

int compareTime(const void *s1, const void *s2)
{
  struct entry *e1 = (struct entry *)s1;
  struct entry *e2 = (struct entry *)s2;

  return (e2->score - e1->score);
}
int compareScore(const void *s1, const void *s2)
{
  struct entry *e1 = (struct entry *)s1;
  struct entry *e2 = (struct entry *)s2;

  return (e2->time - e1->time);
}