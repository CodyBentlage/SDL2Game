#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "common.h"
#include "background.h"

extern App app;
extern Stage stage;

static int backgroundX;
static int backgroundY;
static Star stars[MAX_STARS];
SDL_Texture *baseBackground;
static SDL_Texture *background1;
static SDL_Texture *backgroundTextures[1];
static bool orangeBackground;
static bool greenBackground;
static bool blueBackground;
static bool purpleBackground;

void setRandomBackground(void)
{
	// Seed the random number generator
	srand(time(NULL));

	// Reset all backgrounds to false
	orangeBackground = false;
	greenBackground = false;
	blueBackground = false;
	purpleBackground = false;

	// Choose a random background to set to true
	int randomIndex = rand() % 4;

	switch (randomIndex)
	{
	case 0:
		orangeBackground = true;
		break;
	case 1:
		greenBackground = true;
		break;
	case 2:
		blueBackground = true;
		break;
	case 3:
		purpleBackground = true;
		break;
	}
}

// Function to load a texture from file
SDL_Texture *loadRandomTexture(const char *path, SDL_Renderer *renderer)
{
	SDL_Texture *newTexture = NULL;
	SDL_Surface *loadedSurface = IMG_Load(path);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError());
	}
	else
	{
		newTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path, SDL_GetError());
		}
		SDL_FreeSurface(loadedSurface);
	}
	return newTexture;
}

// Function to get random position within window bounds
SDL_Rect getRandomPosition(int windowWidth, int windowHeight, int textureWidth, int textureHeight)
{
	SDL_Rect pos;
	pos.x = rand() % (windowWidth - textureWidth);
	pos.y = rand() % (windowHeight - textureHeight);
	pos.w = textureWidth;
	pos.h = textureHeight;
	return pos;
}

void initBackground(void)
{
	setRandomBackground();
	if (orangeBackground == true)
	{
		srand(time(NULL)); // Seed the random number generator

		// Initialize textures
		SDL_Texture *textures[5];
		const char *textureFiles[] = {
			"gfx/orange-stars.png",
			"gfx/orange-planet-1.png",
			"gfx/orange-planet-2.png",
			"gfx/orange-asteroid-1.png",
			"gfx/orange-asteroid-2.png"};

		for (int i = 0; i < 5; ++i)
		{
			textures[i] = loadRandomTexture(textureFiles[i], app.renderer);
			if (textures[i] == NULL)
			{
				printf("Failed to load texture %s\n", textureFiles[i]);
				exit(1); // Exit if any texture fails to load
			}
		}

		// Create a target texture to render the random background
		background1 = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
		backgroundTextures[0] = background1;

		if (background1 == NULL)
		{
			printf("Unable to create target texture! SDL Error: %s\n", SDL_GetError());
			exit(1);
		}

		// Set the render target to the new texture
		SDL_SetRenderTarget(app.renderer, background1);

		// Load and render the base background
		SDL_Texture *baseBackground = loadRandomTexture("gfx/orange-back.png", app.renderer);
		if (baseBackground == NULL)
		{
			printf("Failed to load base background texture gfx/orange-back.png\n");
			exit(1);
		}
		SDL_RenderCopy(app.renderer, baseBackground, NULL, NULL);
		SDL_DestroyTexture(baseBackground);

		// Render other elements at random positions
		for (int i = 0; i < 5; ++i)
		{
			int textureWidth, textureHeight;
			SDL_QueryTexture(textures[i], NULL, NULL, &textureWidth, &textureHeight);
			SDL_Rect randomPos = getRandomPosition(SCREEN_WIDTH, SCREEN_HEIGHT, textureWidth, textureHeight);
			SDL_RenderCopy(app.renderer, textures[i], NULL, &randomPos);
		}

		// Reset the render target to the default
		SDL_SetRenderTarget(app.renderer, NULL);

		// Clean up individual textures
		for (int i = 0; i < 5; ++i)
		{
			SDL_DestroyTexture(textures[i]);
		}
	}
	else if (greenBackground == true)
	{
		srand(time(NULL)); // Seed the random number generator

		// Initialize textures
		SDL_Texture *textures[6];
		const char *textureFiles[] = {
			"gfx/blue-planet.png",
			"gfx/green-planet.png",
			"gfx/green-ring-planet.png",
			"gfx/green-stars.png",
			"gfx/green-asteroid-1.png",
			"gfx/green-asteroid-2.png"};

		for (int i = 0; i < 6; ++i)
		{
			textures[i] = loadRandomTexture(textureFiles[i], app.renderer);
			if (textures[i] == NULL)
			{
				printf("Failed to load texture %s\n", textureFiles[i]);
				exit(1); // Exit if any texture fails to load
			}
		}

		// Create a target texture to render the random background
		background1 = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
		backgroundTextures[0] = background1;

		if (background1 == NULL)
		{
			printf("Unable to create target texture! SDL Error: %s\n", SDL_GetError());
			exit(1);
		}

		// Set the render target to the new texture
		SDL_SetRenderTarget(app.renderer, background1);

		// Load and render the base background
		SDL_Texture *baseBackground = loadRandomTexture("gfx/green-back.png", app.renderer);
		if (baseBackground == NULL)
		{
			printf("Failed to load base background texture gfx/orange-back.png\n");
			exit(1);
		}
		SDL_RenderCopy(app.renderer, baseBackground, NULL, NULL);
		SDL_DestroyTexture(baseBackground);

		// Render other elements at random positions
		for (int i = 0; i < 6; ++i)
		{
			int textureWidth, textureHeight;
			SDL_QueryTexture(textures[i], NULL, NULL, &textureWidth, &textureHeight);
			SDL_Rect randomPos = getRandomPosition(SCREEN_WIDTH, SCREEN_HEIGHT, textureWidth, textureHeight);
			SDL_RenderCopy(app.renderer, textures[i], NULL, &randomPos);
		}

		// Reset the render target to the default
		SDL_SetRenderTarget(app.renderer, NULL);

		// Clean up individual textures
		for (int i = 0; i < 6; ++i)
		{
			SDL_DestroyTexture(textures[i]);
		}
	}
	else if (blueBackground == true)
	{
		srand(time(NULL)); // Seed the random number generator

		// Initialize textures
		SDL_Texture *textures[5];
		const char *textureFiles[] = {
			"gfx/blue-prop-planet-big.png",
			"gfx/blue-prop-planet-small.png",
			"gfx/blue-stars.png",
			"gfx/blue-asteroid-1.png",
			"gfx/blue-asteroid-2.png"};

		for (int i = 0; i < 5; ++i)
		{
			textures[i] = loadRandomTexture(textureFiles[i], app.renderer);
			if (textures[i] == NULL)
			{
				printf("Failed to load texture %s\n", textureFiles[i]);
				exit(1); // Exit if any texture fails to load
			}
		}

		// Create a target texture to render the random background
		background1 = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
		backgroundTextures[0] = background1;

		if (background1 == NULL)
		{
			printf("Unable to create target texture! SDL Error: %s\n", SDL_GetError());
			exit(1);
		}

		// Set the render target to the new texture
		SDL_SetRenderTarget(app.renderer, background1);

		// Load and render the base background
		SDL_Texture *baseBackground = loadRandomTexture("gfx/blue-back.png", app.renderer);
		if (baseBackground == NULL)
		{
			printf("Failed to load base background texture gfx/orange-back.png\n");
			exit(1);
		}
		SDL_RenderCopy(app.renderer, baseBackground, NULL, NULL);
		SDL_DestroyTexture(baseBackground);

		// Render other elements at random positions
		for (int i = 0; i < 5; ++i)
		{
			int textureWidth, textureHeight;
			SDL_QueryTexture(textures[i], NULL, NULL, &textureWidth, &textureHeight);
			SDL_Rect randomPos = getRandomPosition(SCREEN_WIDTH, SCREEN_HEIGHT, textureWidth, textureHeight);
			SDL_RenderCopy(app.renderer, textures[i], NULL, &randomPos);
		}

		// Reset the render target to the default
		SDL_SetRenderTarget(app.renderer, NULL);

		// Clean up individual textures
		for (int i = 0; i < 5; ++i)
		{
			SDL_DestroyTexture(textures[i]);
		}
	}
	else if (purpleBackground == true)
	{
		srand(time(NULL)); // Seed the random number generator

		// Initialize textures
		SDL_Texture *textures[7];
		const char *textureFiles[] = {
			"gfx/purple-planet-1.png",
			"gfx/purple-planet-2.png",
			"gfx/purple-planets.png",
			"gfx/purple-swirl.png",
			"gfx/blue-asteroid-1.png",
			"gfx/blue-asteroid-2.png",
			"gfx/blue-planet.png"};

		for (int i = 0; i < 7; ++i)
		{
			textures[i] = loadRandomTexture(textureFiles[i], app.renderer);
			if (textures[i] == NULL)
			{
				printf("Failed to load texture %s\n", textureFiles[i]);
				exit(1); // Exit if any texture fails to load
			}
		}

		// Create a target texture to render the random background
		background1 = SDL_CreateTexture(app.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
		backgroundTextures[0] = background1;

		if (background1 == NULL)
		{
			printf("Unable to create target texture! SDL Error: %s\n", SDL_GetError());
			exit(1);
		}

		// Set the render target to the new texture
		SDL_SetRenderTarget(app.renderer, background1);

		// Load and render the base background
		SDL_Texture *baseBackground = loadRandomTexture("gfx/blue-back.png", app.renderer);
		if (baseBackground == NULL)
		{
			printf("Failed to load base background texture gfx/purple-back.png\n");
			exit(1);
		}
		SDL_RenderCopy(app.renderer, baseBackground, NULL, NULL);
		SDL_DestroyTexture(baseBackground);

		// Render other elements at random positions
		for (int i = 0; i < 7; ++i)
		{
			int textureWidth, textureHeight;
			SDL_QueryTexture(textures[i], NULL, NULL, &textureWidth, &textureHeight);
			SDL_Rect randomPos = getRandomPosition(SCREEN_WIDTH, SCREEN_HEIGHT, textureWidth, textureHeight);
			SDL_RenderCopy(app.renderer, textures[i], NULL, &randomPos);
		}

		// Reset the render target to the default
		SDL_SetRenderTarget(app.renderer, NULL);

		// Clean up individual textures
		for (int i = 0; i < 7; ++i)
		{
			SDL_DestroyTexture(textures[i]);
		}
	}

	backgroundX = 0;
}

void initStarfield(void)
{
	int i;

	for (i = 0; i < MAX_STARS; i++)
	{
		stars[i].x = rand() % WORLD_WIDTH;
		stars[i].y = rand() % WORLD_HEIGHT;
		stars[i].speed = 1 + rand() % 8;
	}
}

void doStarfield(void)
{
	int i;

	if (stage.gamePaused == false)
	{
		for (i = 0; i < MAX_STARS; i++)
		{
			stars[i].x -= stars[i].speed;

			if (stars[i].x < 0)
			{
				stars[i].x = SCREEN_WIDTH + stars[i].x;
			}
		}
	}
}

void drawStarfield(void)
{
	int i, c;

	for (i = 0; i < MAX_STARS; i++)
	{
		c = 32 * stars[i].speed;

		SDL_SetRenderDrawColor(app.renderer, c, c, c, 255);

		SDL_RenderDrawLine(app.renderer, stars[i].x, stars[i].y, stars[i].x + 3, stars[i].y);
	}
}

void drawBackground(void)
{
	SDL_Rect dest;
	int x = backgroundX - stage.cameraX;
	int y = backgroundY - stage.cameraY;

	// Render backgrounds until they cover at least WORLD_WIDTH and WORLD_HEIGHT
	while (x < WORLD_WIDTH)
	{
		while (y < WORLD_HEIGHT)
		{
			dest.x = x;
			dest.y = y;
			dest.w = SCREEN_WIDTH;
			dest.h = SCREEN_HEIGHT;

			SDL_RenderCopy(app.renderer, backgroundTextures[0], NULL, &dest);

			y += SCREEN_HEIGHT;
		}
		x += SCREEN_WIDTH;
		y = backgroundY - stage.cameraY; // Reset y for the next column
	}
	y += SCREEN_HEIGHT;
}