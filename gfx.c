#include "main.h"

void draw(struct game game, struct gameGFX *gfx)
{
    SDL_SetRenderDrawColor( gfx->renderer, 0, 0, 0, 255 );
    SDL_RenderFillRect( gfx->renderer, NULL );

    SDL_SetRenderDrawColor(gfx->renderer,0,255,0,255);
    SDL_Rect leftSide,rightSide;
    leftSide.x=0;
    rightSide.x=SCREEN_WIDTH-BORDER;
    leftSide.y=rightSide.y=0;
    leftSide.h=rightSide.h=SCREEN_HEIGHT;
    leftSide.w=rightSide.w=BORDER;
    SDL_RenderFillRect(gfx->renderer,&leftSide);
    SDL_RenderFillRect(gfx->renderer,&rightSide);
    
    SDL_SetRenderDrawColor(gfx->renderer,255,255,255,255);


    SDL_RenderDrawRect(gfx->renderer,&game.player.object.pos);
    SDL_RenderPresent(gfx->renderer);
}