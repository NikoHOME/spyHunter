#include "main.h"

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