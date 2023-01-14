#include "main.h"


void drawMain(struct game game, struct gameGFX *gfx)
{
    drawRoad(game,gfx);
    drawPlayer(game, gfx);
    drawEnemy(game, gfx);
    drawBullet(game, gfx);
}

void drawPlayer(struct game game, struct gameGFX *gfx)
{
    
    SDL_SetRenderDrawColor(gfx->renderer,255, 255, 255, 255);
    SDL_RenderFillRect(gfx->renderer,&game.player.object.pos);
    SDL_SetRenderDrawColor(gfx->renderer,0, 0, 255, 255);
    SDL_RenderDrawRect(gfx->renderer,&game.player.object.pos);
    SDL_RenderPresent(gfx->renderer);
}

void drawEnemy(struct game game, struct gameGFX *gfx)
{
    int index;
    for(index = 0; index < MAX_ENEMY; ++index)
    {
        if(game.enemy[index].dead)
            continue;
        if(game.enemy[index].isCivilian)
            SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 0, 255);
        else
            SDL_SetRenderDrawColor(gfx->renderer, 0, 45, 210, 255);
        if(game.enemy[index].isArmoured)
            SDL_SetRenderDrawColor(gfx->renderer, 115, 0, 210, 255);
        SDL_RenderFillRect(gfx->renderer, &game.enemy[index].object.pos);

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
        SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 255);
        SDL_RenderFillRect(gfx->renderer, &game.bullet[index].object.pos);
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
    sprintf(bufferTime, "%d", MAX_TIME - game.gameInts[TIME]);
    sprintf(bufferLives, "%d", game.gameInts[PLAYER_LIVES]);
    
    drawString(gfx->renderer, 20, 20, bufferScore, gfx->scrtex, 2, 2);
    drawString(gfx->renderer, 20, 40, bufferVelocity, gfx->scrtex, 2, 2);
    drawString(gfx->renderer, 20, SCREEN_HEIGHT-40, bufferTime, gfx->scrtex, 2, 2);
    drawString(gfx->renderer, 20 + TEXT_SIZE*4, 60, bufferLives, gfx->scrtex, 2, 2);
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
    static const char menu[8][20] = 
    {
        "arrows: move",
        "esc: quit",
        "n: new game",
        "s: save",
        "l: load",
        "p: pause",
        "space: shoot",
        "f: end game"
    };
    
    for(index = 0;index<8;++index,index2+=15)
    {
        drawString(gfx->renderer, SCREEN_WIDTH-BORDER+10, SCREEN_HEIGHT/2+100+index2, menu[index], gfx->scrtex, 1.5, 1.5);
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
    SDL_Rect box;

    box.x = SCREEN_WIDTH/4;
    box.y = SCREEN_HEIGHT/4;
    box.h = SCREEN_HEIGHT/2;
    box.w = SCREEN_WIDTH/2;

    static const char text[]= "PAUSE";
    SDL_SetRenderDrawBlendMode(gfx->renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect( gfx->renderer, NULL );
    SDL_SetRenderDrawColor(gfx->renderer,0, 0, 20, 100);
    SDL_RenderFillRect(gfx->renderer, &box);
    SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 255);
    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(text)*TEXT_SIZE, SCREEN_HEIGHT/2-TEXT_SIZE, text, gfx->scrtex, 2, 2);

    SDL_RenderPresent(gfx->renderer);
}

void drawSave(struct gameGFX *gfx, const char input[])
{
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect( gfx->renderer, NULL );
    SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 255);

    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(input)*TEXT_SIZE, SCREEN_HEIGHT/2-TEXT_SIZE, input, gfx->scrtex, 2, 2);
    SDL_RenderPresent(gfx->renderer);
}

void drawLoad(struct gameGFX *gfx, char *input[], int select, int fileNumber)
{
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect( gfx->renderer, NULL );
    SDL_SetRenderDrawColor(gfx->renderer, 255, 255, 255, 255);
    int index = 0, offset = 40;
    SDL_Rect box;
    for(;index < fileNumber; ++index, offset+=20)
    {
        if(index == select)
        {
            box.h = TEXT_SIZE + 4;
            box.w = strlen(input[index])*TEXT_SIZE + 4;
            box.x = SCREEN_WIDTH/2 - strlen(input[index])*TEXT_SIZE - 2;
            box.y = TEXT_SIZE+offset - 2;
            SDL_RenderDrawRect( gfx->renderer, &box );
        }
        drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(input[index])*TEXT_SIZE, TEXT_SIZE+offset, input[index], gfx->scrtex, 1, 1);
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
        static const char menu[2][20]= 
    {
        "esc: quit",
        "n: new game",
    };
    sprintf(bufferScore, "%s%d", text[0], game.gameInts[SCORE]);
    sprintf(bufferTime, "%s%d", text[1], game.gameInts[TIME]); 
    SDL_SetRenderDrawColor(gfx->renderer,0,0,0,255);
    SDL_RenderFillRect( gfx->renderer, NULL );
    SDL_SetRenderDrawColor(gfx->renderer,255,255,255,255);
    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(text[2])*TEXT_SIZE*2, SCREEN_HEIGHT/2-TEXT_SIZE*8, text[2], gfx->scrtex, 4, 4);
    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(bufferScore)*TEXT_SIZE, SCREEN_HEIGHT/2-TEXT_SIZE, bufferScore, gfx->scrtex, 2, 2);
    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(bufferTime)*TEXT_SIZE, SCREEN_HEIGHT/2+TEXT_SIZE, bufferTime, gfx->scrtex, 2, 2);

    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(menu[0])*TEXT_SIZE, SCREEN_HEIGHT/2+TEXT_SIZE*3, menu[0], gfx->scrtex, 2, 2);
    drawString(gfx->renderer, SCREEN_WIDTH/2-strlen(menu[1])*TEXT_SIZE, SCREEN_HEIGHT/2+TEXT_SIZE*5, menu[1], gfx->scrtex, 2, 2);

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