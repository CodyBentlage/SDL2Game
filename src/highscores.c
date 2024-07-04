#include "common.h"
#include "background.h"
#include "highscores.h"
#include "stage.h"
#include "text.h"
#include "title.h"
#include <stdio.h>

extern App app;
extern Highscores highscores;

static void logic(void);
static void draw(void);
static int highscoreComparator(const void *a, const void *b);
static void drawHighscores(void);
static void doNameInput(void);
static void drawNameInput(void);

static Highscore *newHighscore;
static int cursorBlink;
static int timeout;

void saveHighscores(void)
{
    FILE *file = fopen(HIGHSCORE_FILE, "w");
    if (file)
    {
        for (int i = 0; i < NUM_HIGHSCORES; i++)
        {
            fprintf(file, "%s %d\n", highscores.highscore[i].name, highscores.highscore[i].score);
        }
        fclose(file);
    }
    else
    {
        printf("Failed to open highscore file for writing.\n");
    }
}

void loadHighscores(void)
{
    FILE *file = fopen(HIGHSCORE_FILE, "r");
    if (file)
    {
        for (int i = 0; i < NUM_HIGHSCORES; i++)
        {
            if (fscanf(file, "%s %d", highscores.highscore[i].name, &highscores.highscore[i].score) != 2)
            {
                // If reading fails, fill the rest with default values
                STRNCPY(highscores.highscore[i].name, "ANONYMOUS", MAX_SCORE_NAME_LENGTH);
                highscores.highscore[i].score = NUM_HIGHSCORES - i;
            }
        }
        fclose(file);
    }
    else
    {
        // If file does not exist, initialize with default values
        for (int i = 0; i < NUM_HIGHSCORES; i++)
        {
            highscores.highscore[i].score = NUM_HIGHSCORES - i;
            STRNCPY(highscores.highscore[i].name, "ANONYMOUS", MAX_SCORE_NAME_LENGTH);
        }
    }
}

void initHighscoreTable(void)
{
    memset(&highscores, 0, sizeof(Highscores));
    loadHighscores(); // Load highscores from file
    newHighscore = NULL;
    cursorBlink = 0;
}

void initHighscores(void)
{
    app.delegate.logic = logic;
    app.delegate.draw = draw;

    memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);

    timeout = FPS * 5;
}

static void logic(void)
{
    doStarfield();

    if (newHighscore != NULL)
    {
        doNameInput();
    }
    else
    {
        if (--timeout <= 0)
        {
            initTitle();
        }

        if (app.mouse_buttons[SDL_BUTTON_LEFT] || SDL_GameControllerGetButton(app.controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
        {
            initStage();
        }
    }

    if (++cursorBlink >= FPS)
    {
        cursorBlink = 0;
    }
}

static void doNameInput(void)
{
    int i, n;
    char c;

    n = strlen(newHighscore->name);

    for (i = 0; i < strlen(app.inputText); i++)
    {
        c = toupper(app.inputText[i]);

        if (n < MAX_SCORE_NAME_LENGTH - 1 && c >= ' ' && c <= 'Z')
        {
            newHighscore->name[n++] = c;
            newHighscore->name[n] = '\0'; // Ensure string is null-terminated
        }
    }

    if (n > 0 && app.keyboard[SDL_SCANCODE_BACKSPACE])
    {
        newHighscore->name[--n] = '\0';
        app.keyboard[SDL_SCANCODE_BACKSPACE] = 0;
    }

    if (app.keyboard[SDL_SCANCODE_RETURN] || SDL_GameControllerGetButton(app.controller, SDL_CONTROLLER_BUTTON_START))
    {
        if (strlen(newHighscore->name) == 0)
        {
            if (!app.controller)
            {
                STRNCPY(newHighscore->name, "ANONYMOUS", MAX_SCORE_NAME_LENGTH);
            }
            else
            {
                STRNCPY(newHighscore->name, "CONSOLE USER", MAX_SCORE_NAME_LENGTH);
            }
        }
        saveHighscores(); // Save highscores to file after name input is finished
        newHighscore = NULL;
    }
}

static void draw(void)
{
    drawBackground();

    drawStarfield();

    if (newHighscore != NULL)
    {
        drawNameInput();
    }
    else
    {
        drawHighscores();

        if (timeout % 40 < 20)
        {
            drawText(SCREEN_WIDTH / 2, 600, 255, 255, 255, TEXT_CENTER, "PRESS FIRE TO PLAY!");
        }
    }
}

static void drawNameInput(void)
{
    SDL_Rect r;

    drawText(SCREEN_WIDTH / 2, 70, 255, 255, 255, TEXT_CENTER, "CONGRATULATIONS, YOU'VE GAINED A HIGHSCORE!");

    drawText(SCREEN_WIDTH / 2, 120, 255, 255, 255, TEXT_CENTER, "ENTER YOUR NAME BELOW:");

    drawText(SCREEN_WIDTH / 2, 250, 128, 255, 128, TEXT_CENTER, newHighscore->name);

    if (cursorBlink < FPS / 2)
    {
        r.x = ((SCREEN_WIDTH / 2) + (strlen(newHighscore->name) * GLYPH_WIDTH) / 2) + 5;
        r.y = 250;
        r.w = GLYPH_WIDTH;
        r.h = GLYPH_HEIGHT;

        SDL_SetRenderDrawColor(app.renderer, 0, 255, 0, 255);
        SDL_RenderFillRect(app.renderer, &r);
    }

    drawText(SCREEN_WIDTH / 2, 625, 255, 255, 255, TEXT_CENTER, "PRESS RETURN WHEN FINISHED");
}

static void drawHighscores(void)
{
    int i, y, r, g, b;

    y = 150;

    drawText(SCREEN_WIDTH / 2, 70, 255, 255, 255, TEXT_CENTER, "HIGHSCORES");

    for (i = 0; i < NUM_HIGHSCORES; i++)
    {
        r = 255;
        g = 255;
        b = 255;

        if (highscores.highscore[i].recent)
        {
            b = 0;
        }

        drawText(SCREEN_WIDTH / 2, y, r, g, b, TEXT_CENTER, "#%d. %-15s ...... %03d", (i + 1), highscores.highscore[i].name, highscores.highscore[i].score);

        y += 50;
    }
}

void addHighscore(int score)
{
    Highscore newHighscores[NUM_HIGHSCORES + 1];
    int i;

    memset(newHighscores, 0, sizeof(Highscore) * (NUM_HIGHSCORES + 1));

    for (i = 0; i < NUM_HIGHSCORES; i++)
    {
        newHighscores[i] = highscores.highscore[i];
        newHighscores[i].recent = 0;
    }

    newHighscores[NUM_HIGHSCORES].score = score;
    newHighscores[NUM_HIGHSCORES].recent = 1;

    qsort(newHighscores, NUM_HIGHSCORES + 1, sizeof(Highscore), highscoreComparator);

    newHighscore = NULL;

    for (i = 0; i < NUM_HIGHSCORES; i++)
    {
        highscores.highscore[i] = newHighscores[i];

        if (highscores.highscore[i].recent)
        {
            newHighscore = &highscores.highscore[i];
        }
    }
}

static int highscoreComparator(const void *a, const void *b)
{
    Highscore *h1 = ((Highscore *)a);
    Highscore *h2 = ((Highscore *)b);

    return h2->score - h1->score;
}
