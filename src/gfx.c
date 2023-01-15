#include "main.h"


void drawMain(struct game game, struct gameGFX *gfx)
{
    drawRoad(game,gfx);
    drawPlayer(game, gfx);
    drawEnemy(game, gfx);
    drawBullet(game, gfx);
    drawPowerup(game, gfx);
}

void drawPlayer(struct game game, struct gameGFX *gfx)
{
    
    struct object temporary = game.player.object;
    temporary.pos.x -=(CAR_TEXTURE_SIZE-CAR_WIDTH)/2; 
    temporary.pos.w = CAR_TEXTURE_SIZE;
    temporary.pos.h = CAR_TEXTURE_SIZE;
    SDL_RenderCopy(gfx->renderer, gfx->objectTexture[TEXTURE_PLAYER], NULL, &temporary.pos);
    SDL_RenderPresent(gfx->renderer);
}

void drawPowerup(struct game game, struct gameGFX *gfx)
{
    int index;
    for(index = 0; index < MAX_POWERUP; ++index)
    {
        if(game.powerup[index].dead)
            continue;
        switch(game.powerup[index].type)
        {
            case POWERUP_RANGE:
                SDL_SetRenderDrawColor(gfx->renderer, 220, 120, 220, 255);
                break;
            case POWERUP_SCORE:
                SDL_SetRenderDrawColor(gfx->renderer, 220, 0, 220, 255);
                break;
        }
        SDL_RenderCopy(gfx->renderer, gfx->objectTexture[TEXTURE_BOX], NULL, &game.powerup[index].object.pos);
    }
    SDL_RenderPresent(gfx->renderer);
}

void drawEnemy(struct game game, struct gameGFX *gfx)
{
    int index;
    for(index = 0; index < MAX_ENEMY; ++index)
    {
        if(game.enemy[index].dead)
            continue;
        SDL_Rect temporary = game.enemy[index].object.pos; 
        temporary.x -=(CAR_TEXTURE_SIZE-CAR_WIDTH)/2; 
        temporary.w = CAR_TEXTURE_SIZE;
        temporary.h = CAR_TEXTURE_SIZE;
        if(game.enemy[index].isCivilian)
            SDL_RenderCopy(gfx->renderer, gfx->objectTexture[TEXTURE_CIVIL], NULL, &temporary);
        else if(game.enemy[index].isArmoured)
            SDL_RenderCopy(gfx->renderer, gfx->objectTexture[TEXTURE_ARMOUR], NULL, &temporary);
        else if(game.enemy[index].isPowerup)
            SDL_RenderCopy(gfx->renderer, gfx->objectTexture[TEXTURE_POWERUP], NULL, &temporary);
        else if(game.enemy[index].isPowerupScore)
            SDL_RenderCopy(gfx->renderer, gfx->objectTexture[TEXTURE_POWERUP], NULL, &temporary);
        else
            SDL_RenderCopy(gfx->renderer, gfx->objectTexture[TEXTURE_ENEMY], NULL, &temporary);

    }
    SDL_RenderPresent(gfx->renderer);
}

void drawBullet(struct game game, struct gameGFX *gfx)
{
    int index;
    for(index = 0; index < MAX_BULLETS; ++index)
    {
        if(game.bullet[index].dead)
            continue;
        SDL_RenderCopy(gfx->renderer, gfx->objectTexture[TEXTURE_BULLET], NULL, &game.bullet[index].object.pos);
        //SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 255);
        //SDL_RenderFillRect(gfx->renderer, &game.bullet[index].object.pos);
    }
    SDL_RenderPresent(gfx->renderer);
}

void drawRoad(struct game game, struct gameGFX *gfx)
{
    SDL_SetRenderDrawColor( gfx->renderer, 0, 0, 0, 255 );
    SDL_RenderFillRect( gfx->renderer, NULL );

    SDL_SetRenderDrawColor(gfx->renderer, 0, 255, 0, 255);
    SDL_Rect leftSide, rightSide, leftEdge, rightEdge;
    SDL_Rect leftSide2, rightSide2, leftEdge2, rightEdge2;
    leftEdge.x =  game.wall[0].object.pos.w;
    rightEdge.x = game.wall[1].object.pos.x - BORDER/6;
    leftEdge.y = rightEdge.y = 0;
    leftEdge.h = rightEdge.h = SCREEN_HEIGHT;
    leftEdge.w = rightEdge.w = BORDER/6;

    int index = 0, index2 = 0;
    for(;index < game.gameInts[WALL_AMMOUNT]; ++index)
    {
        SDL_RenderFillRect(gfx->renderer, &game.wall[index].object.pos);
    }
    SDL_SetRenderDrawColor(gfx->renderer, 200, 200, 0, 255);
    //SDL_RenderFillRect(gfx->renderer, &leftEdge);
    ///SDL_RenderFillRect(gfx->renderer, &rightEdge);

    static char bufferVelocity[33], bufferScore[33], bufferTime[33], bufferLives[33];
    sprintf(bufferVelocity, "%d", game.gameInts[VELOCITY]);
    sprintf(bufferScore, "%d", game.gameInts[SCORE]);
    sprintf(bufferTime, "%d", game.gameInts[TIME]);
    sprintf(bufferLives, "%d", game.gameInts[PLAYER_LIVES]);
    
    drawString(gfx->renderer, 20, 20, bufferScore, gfx->scrtex, 2, 2);
    drawString(gfx->renderer, 20, 40, bufferVelocity, gfx->scrtex, 2, 2);
    drawString(gfx->renderer, 20, SCREEN_HEIGHT-40, bufferTime, gfx->scrtex, 2, 2);
    drawString(gfx->renderer, 20 + TEXT_SIZE*4, 60, bufferLives, gfx->scrtex, 2, 2);
    //Prepare car icon
    SDL_Rect s, d;
    s.w = 8;
	s.h = 8;
	d.w = 8*3;
	d.h = 8*3;

    s.x = 0;
	s.y = 8*8;
	d.x = 20;
	d.y = 50;
    SDL_RenderCopy(gfx->renderer, gfx->scrtex, &s, &d);
    static const char menu[10][20] = 
    {
        "arrows: move",
        "esc: quit",
        "n: new game",
        "s: save",
        "l: load",
        "p: pause",
        "space: shoot",
        "f: end game",
        "a,b,c,d,e,f,g,h",
        "i,j,k,l,m,n,o,r"
    };
    index2 = SCREEN_HEIGHT/8;
    for(index = 0;index < 10;++index,index2 += 15)
    {
        drawString(gfx->renderer, SCREEN_WIDTH-BORDER+10, SCREEN_HEIGHT/2 + index2, menu[index], gfx->scrtex, 1.5, 1.5);
    }
    SDL_RenderPresent(gfx->renderer);
}

void drawPause(struct game game, struct gameGFX *gfx)
{
    drawRoad(game, gfx);
    drawPlayer(game, gfx);
    drawEnemy(game, gfx);
    SDL_RenderPresent(gfx->renderer);
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 100);


    static const char text[2][20] =
    {
        "PAUSE",
        "p: unpause"
    };
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect( gfx->renderer, NULL );
    SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 255);
    drawString(gfx->renderer, SCREEN_WIDTH/2 - strlen(text[0])*TEXT_SIZE/2*3, SCREEN_HEIGHT/2 - TEXT_SIZE*2, text[0], gfx->scrtex, 3, 3);
    drawString(gfx->renderer, SCREEN_WIDTH/2 - strlen(text[1])*TEXT_SIZE, SCREEN_HEIGHT/2 + TEXT_SIZE*2, text[1], gfx->scrtex, 2, 2);

    SDL_RenderPresent(gfx->renderer);
}

void drawSave(struct gameGFX *gfx)
{
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect( gfx->renderer, NULL);
    SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 255);
    static const char text[] = "Game Saved";
    static const char menu[] = "esc: exit";
    drawString(gfx->renderer, SCREEN_WIDTH/2 - strlen(text)*TEXT_SIZE*2, SCREEN_HEIGHT/2 - TEXT_SIZE*8, text, gfx->scrtex, 4, 4);

    drawString(gfx->renderer, SCREEN_WIDTH/2 - strlen(menu)*TEXT_SIZE, SCREEN_HEIGHT/2 + TEXT_SIZE*3, menu, gfx->scrtex, 2, 2);
    
    SDL_RenderPresent(gfx->renderer);
}

void drawLoad(struct gameGFX *gfx, char *input[], int select, int fileNumber)
{
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect( gfx->renderer, NULL );
    SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 255);
    int index, indexEnd, offset = 40, perPage = 10;
    char namef[MAX_FILENAME_SIZE+1];
    SDL_Rect box;
    //Paging calculation
    index = (select / perPage) * perPage;
    indexEnd = index + perPage; 
    static const char text[] = "Load a game";
    static const char menu[2][20] = 
    {
        "esc: exit",
        "Enter: confirm"
    };
    drawString(gfx->renderer, SCREEN_WIDTH/2 - strlen(text)*TEXT_SIZE/2*3, TEXT_SIZE*2, text, gfx->scrtex, 3, 3);
    drawString(gfx->renderer, SCREEN_WIDTH/2 - strlen(menu[0])*TEXT_SIZE, TEXT_SIZE*5, menu[0], gfx->scrtex, 2, 2);
    drawString(gfx->renderer, SCREEN_WIDTH/2 - strlen(menu[1])*TEXT_SIZE, TEXT_SIZE*7, menu[1], gfx->scrtex, 2, 2);
    //Paging selection
    for(;index < indexEnd && index < fileNumber; ++index, offset+=20)
    {
        sprintf(namef, "%d: %s", index, input[index]);
        if(index == select)
        {
            box.h = TEXT_SIZE + 4;
            box.w = strlen(namef)*TEXT_SIZE + 4;
            box.x = SCREEN_WIDTH/2 - strlen(namef)*TEXT_SIZE/2 - 2;
            box.y = TEXT_SIZE*9 + offset - 2;
            SDL_RenderDrawRect( gfx->renderer, &box );
        }
        drawString(gfx->renderer, SCREEN_WIDTH/2 - strlen(namef)*TEXT_SIZE/2, TEXT_SIZE*9 +offset, namef, gfx->scrtex, 1, 1);
    }
    SDL_RenderPresent(gfx->renderer);
}

void drawScore(struct gameGFX *gfx, struct entry *entry, int select, int length)
{
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect( gfx->renderer, NULL );
    SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 255);
    int index, indexEnd, offset = 40, perPage = 10;
    char namef[MAX_FILENAME_SIZE+1];
    SDL_Rect box;
    //Paging calculation
    index = (select / perPage) * perPage;
    indexEnd = index + perPage; 
    static const char text[] = "Score display";
    static const char menu[] = "esc: exit";

    drawString(gfx->renderer, SCREEN_WIDTH/2 - strlen(text)*TEXT_SIZE/2*3, TEXT_SIZE*2, text, gfx->scrtex, 3, 3);
    drawString(gfx->renderer, SCREEN_WIDTH/2 - strlen(menu)*TEXT_SIZE, TEXT_SIZE*5, menu, gfx->scrtex, 2, 2);
    //Paging selection
    for(;index < indexEnd && index < length; ++index, offset+=20)
    {
        sprintf(namef, "%d: %d %d", index, entry[index].score, entry[index].time);
        drawString(gfx->renderer, SCREEN_WIDTH/2 - strlen(namef)*TEXT_SIZE/2, TEXT_SIZE*9 +offset, namef, gfx->scrtex, 1, 1);
    }
    SDL_RenderPresent(gfx->renderer);    
}

void drawWhiteText(struct gameGFX *gfx, int x, int y, float scaleX, float scaleY, const char input[])
{
    SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 255);
    drawString(gfx->renderer, x, y, input, gfx->scrtex, scaleX, scaleY);
    SDL_RenderPresent(gfx->renderer);
}

void drawEndScreen(struct game game, struct gameGFX *gfx)
{
    static char bufferScore[40],bufferTime[40];
    static const char text[3][10] = 
    {
        "Score: ",
        "Time: ",
        "Game Over"
    };
    static const char menu[4][20]= 
    {
        "esc: quit",
        "n: new game",
        "s: save score",
        "l: load score"
    };
    sprintf(bufferScore, "%s%d", text[0], game.gameInts[SCORE]);
    sprintf(bufferTime, "%s%d", text[1], game.gameInts[TIME]); 
    SDL_SetRenderDrawColor(gfx->renderer,0,0,0,255);
    SDL_RenderFillRect( gfx->renderer, NULL );
    SDL_SetRenderDrawColor(gfx->renderer,255,255,255,255);
    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(text[2])*TEXT_SIZE*2, SCREEN_HEIGHT/2-TEXT_SIZE*8, text[2], gfx->scrtex, 4, 4);
    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(bufferScore)*TEXT_SIZE, SCREEN_HEIGHT/2-TEXT_SIZE, bufferScore, gfx->scrtex, 2, 2);
    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(bufferTime)*TEXT_SIZE, SCREEN_HEIGHT/2+TEXT_SIZE, bufferTime, gfx->scrtex, 2, 2);
    int index = 0, offset = TEXT_SIZE*3;
    for(; index < 4; ++index, offset+=TEXT_SIZE*2)
    {
        drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(menu[index])*TEXT_SIZE, SCREEN_HEIGHT/2+offset, menu[index], gfx->scrtex, 2, 2);
    }

    SDL_RenderPresent(gfx->renderer);
}
void drawString(SDL_Renderer *screen, int x, int y, const char *text, SDL_Texture *charset, float scaleX, float scaleY)
{
    x/=scaleX;
    y/=scaleY;
	int px, py, c;
	SDL_Rect s, d;
	s.w = 8;
	s.h = 8;
	d.w = 8*scaleX;
	d.h = 8*scaleY;
	while(*text)
    {
		c = *text & 255;
		px = (c % 16) * 8;
		py = (c / 16) * 8;
		s.x = px;
		s.y = py;
		d.x = x*scaleX;
		d.y = y*scaleY;
		SDL_RenderCopy(screen, charset, &s, &d);
		x += 8;
		text++;
	}
}