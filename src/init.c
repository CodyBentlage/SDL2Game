#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_gamecontroller.h>
#include "common.h"
#include "background.h"
#include "highscores.h"
#include "init.h"
#include "sound.h"
#include "text.h"

extern App app;
extern Stage stage;

const char *musicTracks[] = {
    "music/BossMain.ogg",
    "music/Map.ogg",
    "music/Mars.ogg",
    "music/Venus.ogg"
};

void initSDL(void)
{
	int rendererFlags, windowFlags;

	rendererFlags = SDL_RENDERER_ACCELERATED;

	windowFlags = 0;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) // Initialize game controller subsystem
	{
		printf("Couldn't initialize SDL: %s\n", SDL_GetError());
		exit(1);
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1)
	{
		printf("Couldn't initialize SDL Mixer\n");
		exit(1);
	}

	Mix_AllocateChannels(MAX_SND_CHANNELS);

	app.window = SDL_CreateWindow("Boomin", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

	app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);

	IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

	SDL_ShowCursor(0);

	// Initialize the game controller
	if (SDL_NumJoysticks() < 1)
	{
		printf("Warning: No joysticks connected!\n");
	}
	else
	{
		// Load game controller
		for (int i = 0; i < SDL_NumJoysticks(); i++)
		{
			if (SDL_IsGameController(i))
			{
				app.controller = SDL_GameControllerOpen(i);
				if (app.controller)
				{
					printf("Game Controller %d connected: %s\n", i, SDL_GameControllerName(app.controller));
					break; // Assuming you only want to use the first available controller
				}
				else
				{
					printf("Could not open gamecontroller %i: %s\n", i, SDL_GetError());
				}
			}
		}
	}
}

void initGame(void)
{
	initBackground();

	initStarfield();

	initSounds();

	initFonts();

	initHighscoreTable();

	memset(&stage, 0, sizeof(Stage));

	// Seed the random number generator
	srand(time(NULL));

	// Choose a random index for musicTracks array
	int trackIndex = rand() % 4;

	loadMusic(musicTracks[trackIndex]);

	playMusic(1);
}

void cleanup(void)
{
	if (app.controller)
	{
		SDL_GameControllerClose(app.controller);
		app.controller = NULL;
	}

	Mix_Quit();

	IMG_Quit();

	SDL_DestroyRenderer(app.renderer);

	SDL_DestroyWindow(app.window);

	SDL_Quit();
}
