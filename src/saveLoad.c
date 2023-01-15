#include "main.h"

void save(struct game *game, const char name[])
{
    FILE *file = fopen(name, "wb");

    fwrite(&game->player, sizeof(struct player), 1, file);
    fwrite(&game->gameState, sizeof(game->gameState), 1, file);
    fwrite(&game->gameInts, sizeof(game->gameInts), 1, file);

    fwrite(game->wall, sizeof(struct box), game->gameInts[WALL_AMMOUNT], file);
    fwrite(game->powerup, sizeof(struct box), MAX_POWERUP, file);
    fwrite(game->enemy, sizeof(struct enemy), MAX_ENEMY, file);
    fwrite(game->bullet, sizeof(struct bullet), MAX_BULLETS, file);
    fclose(file);
}

void load(struct game *game, const char name[])
{
    
    FILE *file = fopen(name, "rb");

    fread(&game->player, sizeof(struct player), 1, file);
    fread(&game->gameState, sizeof(game->gameState), 1, file);
    fread(&game->gameInts, sizeof(game->gameInts), 1, file);
    

    game->wall = realloc(game->wall, game->gameInts[WALL_AMMOUNT] * sizeof(struct box));
    game->enemy = realloc(game->enemy, MAX_ENEMY * sizeof(struct enemy));
    game->bullet = realloc(game->bullet, MAX_BULLETS * sizeof(struct bullet));


    fread(game->wall , sizeof(struct box), game->gameInts[WALL_AMMOUNT], file);
    fread(game->powerup, sizeof(struct box), MAX_POWERUP, file);
    fread(game->enemy, sizeof(struct enemy), MAX_ENEMY, file);
    fread(game->bullet, sizeof(struct bullet), MAX_BULLETS, file);

    fclose(file);
}



void saveInput(struct game *game, struct gameGFX *gfx)
{
    
    FILE *file;
    char timef[MAX_FILENAME_SIZE+1], inputf[MAX_FILENAME_SIZE+1];
    int index;
    char exitWrite = FALSE;
    SDL_SetRenderDrawColor(gfx->renderer, 0, 0, 0, 255);
    SDL_RenderFillRect( gfx->renderer, NULL );
    const char textSave[] = "File Saved", textFail[] = "Save Failed";
    int currentTime, lastTime = SDL_GetTicks();
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(timef, "%02d:%02d:%02d\n", tm.tm_hour, tm.tm_min, tm.tm_sec);
    snprintf(inputf, sizeof(inputf), "save/%s.save", timef);

    save(game, inputf);
    //Create save menu
    if(file = fopen(inputf, "r"))
        drawSave(gfx);
    else
        drawWhiteText(gfx, SCREEN_WIDTH/2-strlen(textSave)*TEXT_SIZE, SCREEN_HEIGHT/2 - TEXT_SIZE*2, 2, 2, textFail);
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
                break;
            case SDL_QUIT:
                game->gameState[QUIT] = TRUE;
				break;
        }
    }

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
    //Create load menu
    while(!exitWrite)
    {
        sleepTillNextFrame(game, &currentTime, &lastTime);
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
                if(keystate[SDL_SCANCODE_UP] && fileNumber)
                {
                    if(select > 0)
                        --select;
                    drawLoad(gfx, filesList, select, fileNumber);
                }
                if(keystate[SDL_SCANCODE_DOWN] && fileNumber)
                {
                    if(select < fileNumber-1)
                        ++select;
                    drawLoad(gfx, filesList, select, fileNumber);
                }   
                break;
            case SDL_QUIT:
                game->gameState[QUIT] = TRUE;
				break;
        }
    }
    if(fileNumber)
    {
        snprintf(inputf, sizeof(inputf), "save/%s", filesList[select]);
        load(game, inputf);
    }
    game->gameState[IS_PAUSED] = TRUE;
    drawPause(*game,gfx);
}

void saveScore(struct game *game, struct gameGFX *gfx)
{
    FILE *file;
    int length = 1;
    struct entry entry;
    entry.score = game->gameInts[SCORE];
    entry.time = game->gameInts[TIME];
    char text[] = "Score saved";
    if(!(file = fopen("score.file", "rb+")))
    {
        file = fopen("score.file", "wb");
        fwrite(&length, sizeof(length), 1, file);
        fwrite(&entry, sizeof(entry), 1, file);
        drawWhiteText(gfx, SCREEN_WIDTH/2-strlen(text)*TEXT_SIZE, SCREEN_HEIGHT/2 + TEXT_SIZE*14, 2, 2, text);
        fclose(file);
        return;
    }
    fread(&length, sizeof(length), 1, file);
    fseek(file, 0, SEEK_SET);
    ++length;
    fwrite(&length, sizeof(length), 1, file);
    fseek(file, 0, SEEK_END);
    fwrite(&entry, sizeof(entry), 1, file);

    drawWhiteText(gfx, SCREEN_WIDTH/2-strlen(text)*TEXT_SIZE, SCREEN_HEIGHT/2 + TEXT_SIZE*14, 2, 2, text);
    fclose(file);
}

void loadScore(struct game *game, struct gameGFX *gfx)
{
    FILE *file;
    int length;
    struct entry *entry, *entryPSort, *entryTSort, *display;
    char text[] = "No scores saved";
    if(!(file = fopen("score.file", "rb")))
    {
        drawWhiteText(gfx, SCREEN_WIDTH/2-strlen(text)*TEXT_SIZE, SCREEN_HEIGHT/2 + TEXT_SIZE*12, 2, 2, text);
        return;
    }
    fread(&length, sizeof(length), 1, file);
    entry = (struct entry*)(malloc(sizeof(struct entry) * length));
    entryPSort = (struct entry*)(malloc(sizeof(struct entry) * length));
    entryTSort = (struct entry*)(malloc(sizeof(struct entry) * length));
    fread(entry, sizeof(struct entry), length, file);
    memcpy(entryPSort, entry, sizeof(struct entry) * length );
    memcpy(entryTSort, entry, sizeof(struct entry) * length );
    qsort(entryPSort, length, sizeof(struct entry), compareScore);
    qsort(entryTSort, length, sizeof(struct entry), compareTime);
    int select = 0, currentTime, lastTime;
    char exitWrite = FALSE;
    drawScore(gfx, entry, select, length);
    display = entry;
    while(!exitWrite)
    {
        SDL_PollEvent(&gfx->event);
        sleepTillNextFrame(game, &currentTime, &lastTime);
		switch((gfx->event).type) 
        {
			case SDL_KEYDOWN:
                const Uint8 *keystate = SDL_GetKeyboardState(NULL);
                if(keystate[QUIT_KEY])
                {
                    exitWrite = 2;
                }
                if(keystate[SDL_SCANCODE_P])
                {
                    if(display == entryPSort)
                        display = entry;  
                    else
                        display = entryPSort;
                }
                if(keystate[SDL_SCANCODE_T])
                {
                    if(display == entryTSort)
                        display = entry;
                    else
                        display = entryTSort;
                }
                if(keystate[SDL_SCANCODE_UP])
                {
                    if(select > 0)
                        select-=10;
                }
                if(keystate[SDL_SCANCODE_DOWN])
                {
                    if(select+10 < length)
                        select+=10;
                }   
                drawScore(gfx, display, select, length);
                break;
            case SDL_QUIT:
                game->gameState[QUIT] = TRUE;
				break;
        }
    }
    fclose(file);
    free(entry);
    free(entryPSort);
    free(entryTSort);
    drawEndScreen(*game,gfx);
    
}