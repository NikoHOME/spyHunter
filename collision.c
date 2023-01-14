#include "main.h"


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