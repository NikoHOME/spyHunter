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
    newEnemy.isPowerupScore = FALSE;
    newEnemy.isPowerup = FALSE;
    newEnemy.isArmoured = FALSE;
    // Randomize type
    if(rand()%100 <= ENEMY_CHANCE)
    {
        newEnemy.isCivilian = FALSE;
        if(rand()%100 <= ENEMY_ARMOUR_CHANCE)
        {
            newEnemy.isArmoured = TRUE;
        }
        else if(rand()%100 <= ENEMY_POWERUP_RANGE_CHANCE)
        {
            newEnemy.isPowerup = TRUE;
        }
        else if(rand()%100 <= ENEMY_POWERUP_SCORE_CHANCE)
        {
            newEnemy.isPowerupScore = TRUE;
        }
    }
    else
    {
        newEnemy.isCivilian = TRUE;
    }
    // If player is faster spawn at the top otherwise at the bottom
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
    //Initialize enemy
    newEnemy.dead = FALSE;
    newEnemy.stunned = 0;
    newEnemy.attack = 0;
    newEnemy.isAttacking = FALSE; 
    newEnemy.attackPower = 0;
    initializeObj(&newEnemy.view[UP], -CAR_WIDTH, -CAR_HEIGHT*2, CAR_HEIGHT*2, CAR_WIDTH*3);
    initializeObj(&newEnemy.view[LEFT], -CAR_WIDTH*2, 0, CAR_HEIGHT, CAR_WIDTH*2);
    initializeObj(&newEnemy.view[RIGHT], CAR_WIDTH, 0, CAR_HEIGHT, CAR_WIDTH*2);
    int index;
    char direction;
    //Find free stop in array (i not dead)
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
    //Find free stop in array (is not dead)
    for(index = 0; index < MAX_BULLETS; ++index)
    {
        if(game->bullet[index].dead)
            break;
    }
    //Spawn above player if player shot it
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
    // Declare bullet lifetime (max distance)
    if(game->gameInts[POWERUP_DURATION])
        game->bullet[index].maxDistance = BULLET_BASE_SPEED * 20;
    else
        game->bullet[index].maxDistance = BULLET_BASE_SPEED * 10;
    game->bullet[index].dead = FALSE;
    game->bullet[index].speed = BULLET_BASE_SPEED;
    game->bullet[index].object.lastPosX = game->bullet[index].object.lastPosY = 0;
    ++game->gameInts[BULLET_AMMOUNT];
}

void spawnBox(struct game *game, int ownerIndex, char type)
{
    int index;
    if(game->gameInts[POWERUP_AMMOUNT] >= MAX_POWERUP)
        return;
    //Find free stop in array (is not dead)
    for(index = 0; index < MAX_POWERUP; ++index)
    {
        if(game->bullet[index].dead)
            break;
    }
    initializeObj(&game->powerup[index].object, game->enemy[ownerIndex].object.pos.x, game->enemy[ownerIndex].object.pos.y, BULLET_HEIGHT*2, BULLET_WIDTH*2);
    game->powerup[index].speed = -game->gameInts[VELOCITY]/10;
    game->powerup[index].dead = FALSE;
    game->powerup[index].type = type;
    ++game->gameInts[POWERUP_AMMOUNT];
}
