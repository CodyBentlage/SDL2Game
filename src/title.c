#include "common.h"
#include "background.h"
#include "draw.h"
#include "highscores.h"
#include "stage.h"
#include "text.h"
#include "title.h"

extern App app;

static void logic(void);
static void draw(void);
static void drawLogo(void);

static SDL_Texture *sdl2Texture;
static SDL_Texture *shooterTexture;
static int reveal = 0;
static int timeout;

void initTitle(void)
{
	app.delegate.logic = logic;
	app.delegate.draw = draw;

	memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);

	sdl2Texture = loadTexture("gfx/sdl2.png");
	shooterTexture = loadTexture("gfx/shooter.png");

	timeout = FPS * 5;
}

static void logic(void)
{
	doStarfield();

	if (reveal < SCREEN_HEIGHT)
	{
		reveal++;
	}

	if (--timeout <= 0)
	{
		initHighscores();
	}

	if (app.mouse_buttons[SDL_BUTTON_LEFT] || SDL_GameControllerGetButton(app.controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER))
	{
		initStage();
	}
}

static void draw(void)
{
	drawBackground();

	drawStarfield();

	drawLogo();

	if (timeout % 40 < 20)
	{
		drawText(SCREEN_WIDTH / 2, 600, 255, 255, 255, TEXT_CENTER, "PRESS FIRE TO PLAY!");
	}
}

static void drawLogo(void)
{
	SDL_Rect r;

	r.x = 0;
	r.y = 0;

	SDL_QueryTexture(sdl2Texture, NULL, NULL, &r.w, &r.h);

	r.h = MIN(reveal, r.h);

	blitRect(sdl2Texture, &r, (SCREEN_WIDTH / 2) - (r.w / 2), 100);

	SDL_QueryTexture(shooterTexture, NULL, NULL, &r.w, &r.h);

	r.h = MIN(reveal, r.h);

	blitRect(shooterTexture, &r, (SCREEN_WIDTH / 2) - (r.w / 2), 250);
}
