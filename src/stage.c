#include "common.h"
#include "background.h"
#include "draw.h"
#include "highscores.h"
#include "sound.h"
#include "stage.h"
#include "text.h"
#include "util.h"
#include "enemies.h"

extern App app;
extern Highscores highscores;
extern Stage stage;

static void logic(void);
static void draw(void);

static void initPlayer(void);
static void doPlayer(void);

static void fireBullet(float angle);
static void fireShotgun(float angle);
static void fireLaser(float angle);
static void fireFlame(float angle);

static void doFighters(void);
static void doBullets(void);
static void drawFighters(void);

static void drawBullets(void);
static int bulletHitFighter(Entity *b);

static void doEnemies(void);
static void fireAlienBullet(Entity *e);

static void clipPlayer(void);

static void resetStage(void);

static void drawExplosions(void);
static void doExplosions(void);
static void addExplosions(int x, int y, int num);

static void addDebris(Entity *e);
static void doDebris(void);
static void drawDebris(void);

static void drawHud(void);

static void doShotgunPods(void);
static void addShotgunPods(int x, int y);
static void drawShotgunPods(void);

static void doLaserPods(void);
static void addLaserPods(int x, int y);
static void drawLaserPods(void);

static void doFlamePods(void);
static void addFlamePods(int x, int y);
static void drawFlamePods(void);

static void doHealthPods(void);
static void addHealthPods(int x, int y);
static void drawHealthPods(void);

static void toggleBoost(bool activate);
static void toggleSystemsDown(bool activate);

static Entity *player;
static SDL_Texture *bulletTexture;
static SDL_Texture *laserTexture;
static SDL_Texture *flameTexture;
static SDL_Texture *enemyTexture;
static SDL_Texture *alienBulletTexture;
static SDL_Texture *playerTexture;
static SDL_Texture *explosionTexture;
static SDL_Texture *pointsTexture;
static SDL_Texture *laserPodTexture;
static SDL_Texture *flamePodTexture;
static SDL_Texture *healthTexture;
static SDL_Texture *currentTexture;
static SDL_Texture *playerBoostTexture1;
static SDL_Texture *playerBoostTexture2;
static SDL_Texture *playerBoostTexture3;
static SDL_Texture *playerBoostTexture4;

static int aim_x;
static int aim_y;
static float aim_threshold;

static int enemySpawnTimer;
static int stageResetTimer;

static int shotgunEnabled = 0;
static int shotgunDuration = 0;
static Uint32 shotgunStartTime = 0;

static int laserEnabled = 0;
static int laserDuration = 0;
static Uint32 laserStartTime = 0;

static int flameEnabled = 0;
static int flameDuration = 0;
static Uint32 flameStartTime = 0;

static float PLAYER_SPEED = 4;
static float aim_angle = 0.0f;

static bool hyperDriveSoundPlayed = false;

static int playerCrashed = 0;
static bool systemsDown = false;

static int flameGifFrame = 0;

static int prevStartButtonState = 0;

Uint32 pauseStartTime = 0;
Uint32 totalPauseDuration = 0;
Uint32 lastPauseTime = 0;

void initStage(void)
{
	app.delegate.logic = logic;
	app.delegate.draw = draw;

	bulletTexture = loadTexture("gfx/playerBullet.png");
	laserTexture = loadTexture("gfx/playerBullet.png");
	flameTexture = loadTexture("gfx/flameBullet.png");
	enemyTexture = loadTexture("gfx/enemy.png");
	alienBulletTexture = loadTexture("gfx/alienBullet.png");
	playerTexture = loadTexture("gfx/player.png");
	explosionTexture = loadTexture("gfx/explosion.png");
	pointsTexture = loadTexture("gfx/points.png");
	laserPodTexture = loadTexture("gfx/laserPod.png");
	flamePodTexture = loadTexture("gfx/flamePod.png");
	healthTexture = loadTexture("gfx/health.png");
	playerBoostTexture1 = loadTexture("gfx/playerFlamegif_0.png");
	playerBoostTexture2 = loadTexture("gfx/playerFlamegif_1.png");
	playerBoostTexture3 = loadTexture("gfx/playerFlamegif_2.png");
	playerBoostTexture4 = loadTexture("gfx/playerFlamegif_3.png");
	currentTexture = playerBoostTexture3;

	memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);
	memset(app.mouse_buttons, 0, sizeof(int) * MAX_MOUSE_BUTTONS);

	resetStage();

	stage.score = 0;

	stage.cameraX = 0;
	stage.cameraY = 0;

	initPlayer();

	enemySpawnTimer = 0;

	stageResetTimer = FPS * 3;

	shotgunDuration = 0;
	laserDuration = 0;
	flameDuration = 0;

	systemsDown = false;
	playerCrashed = 0;

	hyperDriveSoundPlayed = false;

	PLAYER_SPEED = 4;

	prevStartButtonState = 0;

	pauseStartTime = 0;
	totalPauseDuration = 0;
	lastPauseTime = 0;
}

static void resetStage(void)
{
	Entity *e;
	Explosion *ex;
	Debris *d;

	shotgunEnabled = 0;
	laserEnabled = 0;
	flameEnabled = 0;

	systemsDown = false;
	playerCrashed = 0;

	hyperDriveSoundPlayed = false;

	prevStartButtonState = 0;

	pauseStartTime = 0;
	totalPauseDuration = 0;
	lastPauseTime = 0;

	while (stage.fighterHead.next)
	{
		e = stage.fighterHead.next;
		stage.fighterHead.next = e->next;
		free(e);
	}

	while (stage.bulletHead.next)
	{
		e = stage.bulletHead.next;
		stage.bulletHead.next = e->next;
		free(e);
	}

	while (stage.explosionHead.next)
	{
		ex = stage.explosionHead.next;
		stage.explosionHead.next = ex->next;
		free(ex);
	}

	while (stage.debrisHead.next)
	{
		d = stage.debrisHead.next;
		stage.debrisHead.next = d->next;
		free(d);
	}

	while (stage.shotgunPodHead.next)
	{
		e = stage.shotgunPodHead.next;
		stage.shotgunPodHead.next = e->next;
		free(e);
	}

	while (stage.laserPodHead.next)
	{
		e = stage.laserPodHead.next;
		stage.laserPodHead.next = e->next;
		free(e);
	}

	while (stage.flamePodHead.next)
	{
		e = stage.flamePodHead.next;
		stage.flamePodHead.next = e->next;
		free(e);
	}

	while (stage.healthPodHead.next)
	{
		e = stage.healthPodHead.next;
		stage.healthPodHead.next = e->next;
		free(e);
	}

	memset(&stage, 0, sizeof(Stage));
	stage.fighterTail = &stage.fighterHead;
	stage.bulletTail = &stage.bulletHead;
	stage.explosionTail = &stage.explosionHead;
	stage.debrisTail = &stage.debrisHead;
	stage.shotgunPodTail = &stage.shotgunPodHead;
	stage.laserPodTail = &stage.laserPodHead;
	stage.flamePodTail = &stage.flamePodHead;
	stage.healthPodTail = &stage.healthPodHead;
}

static void initPlayer()
{
	player = malloc(sizeof(Entity));
	memset(player, 0, sizeof(Entity));
	stage.fighterTail->next = player;
	stage.fighterTail = player;

	player->maxHealth = 1000;
	player->health = player->maxHealth;
	player->boostCooldown = 0;
	player->boostTimer = 0;
	player->x = 100;
	player->y = 100;
	player->texture = playerTexture;
	SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);

	player->side = SIDE_PLAYER;

	aim_angle = 0.0f;
}

static void logic(void)
{
	updateCamera();

	doBackground();

	doStarfield();

	doPlayer();

	doEnemies();

	doFighters();

	doBullets();

	doExplosions();

	doDebris();

	doShotgunPods();
	doLaserPods();
	doFlamePods();
	doHealthPods();

	if (stage.gamePaused == false)
	{
		spawnEnemyFighters();
		spawnEnemyMosquitos();
		spawnEnemyTheCube();
	}

	clipPlayer();

	if (!app.controller && player == NULL && --stageResetTimer <= 0)
	{
		addHighscore(stage.score);
		initHighscores();
	}

	if (app.controller && player == NULL && --stageResetTimer <= 0)
	{
		addHighscore(stage.score);
		initHighscores();
	}
}

void doPlayer(void)
{
	// Systems logic
	if (player != NULL)
	{
		if (stage.gamePaused == false)
		{
			if (player->systemsTimer > 0)
			{
				player->systemsTimer--;

				if (player->systemsTimer <= 0)
				{
					toggleSystemsDown(false);	   // Deactivate systems when timer expires
					player->systemsCooldown = 120; // Set cooldown to 7 seconds (300 frames, assuming 60 frames per second)
				}
			}
			else if (player->systemsCooldown > 0)
			{
				player->systemsCooldown--;
			}
		}

		// Update player reload
		if (player->reload > 0)
		{
			if (stage.gamePaused == false)
			{
				player->reload--;
			}
		}

		if (stage.gamePaused == false)
		{
			// Boost logic
			if (player->boostTimer > 0)
			{
				player->boostTimer--;

				if (player->boostTimer <= 0)
				{
					toggleBoost(false);			 // Deactivate boost when timer expires
					player->boostCooldown = 180; // Set cooldown to 3 seconds (180 frames, assuming 60 frames per second)
				}
			}
			else if (player->boostCooldown > 0)
			{
				player->boostCooldown--;
			}
		}

		if (systemsDown == false)
		{
			player->dx = player->dy = 0;
		}

		if (app.controller)
		{
			if (stage.gamePaused == false)
			{
				// Handle game controller input
				if (systemsDown == false)
				{
					if (SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_LEFTY) < -8000)
					{
						player->dy = -PLAYER_SPEED;
						printf("Player's coordinates: x = %f, y = %f\n", player->x, player->y);
					}

					if (SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_LEFTY) > 8000)
					{
						player->dy = PLAYER_SPEED;
						printf("Player's coordinates: x = %f, y = %f\n", player->x, player->y);
					}

					if (SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_LEFTX) < -8000)
					{
						player->dx = -PLAYER_SPEED;
						printf("Player's coordinates: x = %f, y = %f\n", player->x, player->y);
					}

					if (SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_LEFTX) > 8000)
					{
						player->dx = PLAYER_SPEED;
						printf("Player's coordinates: x = %f, y = %f\n", player->x, player->y);
					}
				}

				if (!player->boostActive)
				{
					// Calculate aiming direction
					aim_x = SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_RIGHTX);
					aim_y = SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_RIGHTY);
					aim_threshold = 8000.0f;

					if (aim_x * aim_x + aim_y * aim_y > aim_threshold * aim_threshold)
					{
						aim_angle = atan2f((float)aim_y, (float)aim_x);
					}
				}
				else if (player->boostActive)
				{
					// Calculate aiming direction
					aim_x = SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_LEFTX);
					aim_y = SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_LEFTY);
					aim_threshold = 8000.0f;

					if (aim_x * aim_x + aim_y * aim_y > aim_threshold * aim_threshold)
					{
						aim_angle = atan2f((float)aim_y, (float)aim_x);
					}
				}
				if (SDL_GameControllerGetButton(app.controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) && player->reload <= 0)
				{

					if (shotgunEnabled == 1)
					{
						fireShotgun(aim_angle);
						playSound(SND_PLAYER_FIRE, CH_PLAYER);
					}
					else if (laserEnabled == 1)
					{
						fireLaser(aim_angle);
						playSound(SND_PLAYER_FIRE_LASER, CH_PLAYER_FIRE_LASER);
					}
					else if (flameEnabled == 1)
					{
						fireFlame(aim_angle);
					}
					else
					{
						fireBullet(aim_angle);
						playSound(SND_PLAYER_FIRE, CH_PLAYER);
					}
				}
				if (SDL_GameControllerGetButton(app.controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
				{
					if (player->boostTimer <= 0 && player->boostCooldown <= 0)
					{
						toggleBoost(true); // Activate boost
					}
				}
			}
			if (SDL_GameControllerGetButton(app.controller, SDL_CONTROLLER_BUTTON_START))
			{
				// Only toggle the pause state if the button wasn't pressed in the previous frame
				if (prevStartButtonState == 0)
				{
					// Toggle gamePaused state
					stage.gamePaused = !stage.gamePaused;

					// Handle pausing sounds
					if (stage.gamePaused == true)
					{
						Mix_Pause(CH_HYPER_DRIVE);
						Mix_Pause(CH_ANY);
						Mix_Pause(CH_PLAYER);
						Mix_Pause(CH_PLAYER_FIRE_LASER);
						Mix_Pause(CH_ALIEN_FIRE);
						Mix_Pause(CH_POINTS);
						Mix_Pause(CH_SHIP_DOWN);
						pauseMusic();
					}
					else if (stage.gamePaused == false)
					{
						Mix_Resume(CH_HYPER_DRIVE);
						Mix_Resume(CH_ANY);
						Mix_Resume(CH_PLAYER);
						Mix_Resume(CH_PLAYER_FIRE_LASER);
						Mix_Resume(CH_ALIEN_FIRE);
						Mix_Resume(CH_POINTS);
						Mix_Resume(CH_SHIP_DOWN);
						resumeMusic();
					}

					if (stage.gamePaused)
					{
						// Record when the game was paused
						pauseStartTime = SDL_GetTicks();
					}
					else
					{
						// Calculate total pause duration
						lastPauseTime = SDL_GetTicks() - pauseStartTime;
						totalPauseDuration += lastPauseTime;
					}
				}

				// Update the previous button state to indicate it's pressed
				prevStartButtonState = 1;
			}
			else
			{
				// Update the previous button state to indicate it's not pressed
				prevStartButtonState = 0;
			}
		}
		else
		{
			if (stage.gamePaused == false)
			{
				// Handle keyboard and mouse input
				if (app.keyboard[SDL_SCANCODE_W])
				{
					player->dy = -PLAYER_SPEED;
				}

				if (app.keyboard[SDL_SCANCODE_A])
				{
					player->dx = -PLAYER_SPEED;
				}

				if (app.keyboard[SDL_SCANCODE_S])
				{
					player->dy = PLAYER_SPEED;
				}

				if (app.keyboard[SDL_SCANCODE_D])
				{
					player->dx = PLAYER_SPEED;
				}
				if (app.keyboard[SDL_SCANCODE_LSHIFT])
				{
					if (player->boostTimer <= 0 && player->boostCooldown <= 0)
					{
						toggleBoost(true); // Activate boost
					}
				}

				// Track mouse position
				float aim_x = (float)(app.mouse_x - player->x);
				float aim_y = (float)(app.mouse_y - player->y);

				// Calculate aim_angle
				aim_angle = atan2f(aim_y, aim_x);

				if (app.mouse_buttons[SDL_BUTTON_LEFT] && player->reload <= 0)
				{

					if (shotgunEnabled == 1)
					{
						fireShotgun(aim_angle);
						playSound(SND_PLAYER_FIRE, CH_PLAYER);
					}
					else if (laserEnabled == 1)
					{
						fireLaser(aim_angle);
						playSound(SND_PLAYER_FIRE_LASER, CH_PLAYER_FIRE_LASER);
					}
					else if (flameEnabled == 1)
					{
						fireFlame(aim_angle);
					}
					else
					{
						fireBullet(aim_angle);
						playSound(SND_PLAYER_FIRE, CH_PLAYER);
					}
				}
				if (app.keyboard[SDL_SCANCODE_ESCAPE])
				{
					// Only toggle the pause state if the button wasn't pressed in the previous frame
					if (prevStartButtonState == 0)
					{
						// Toggle gamePaused state
						stage.gamePaused = !stage.gamePaused;
					}

					// Update the previous button state to indicate it's pressed
					prevStartButtonState = 1;
				}
				else
				{
					// Update the previous button state to indicate it's not pressed
					prevStartButtonState = 0;
				}
			}
		}
	}
}

static void fireBullet(float angle)
{
	Entity *bullet;

	bullet = malloc(sizeof(Entity));
	if (!bullet)
	{
		fprintf(stderr, "Failed to allocate memory for bullet\n");
		exit(1); // Handle memory allocation failure gracefully
	}

	memset(bullet, 0, sizeof(Entity));
	stage.bulletTail->next = bullet;
	stage.bulletTail = bullet;

	bullet->texture = bulletTexture;
	SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

	// Calculate spawn position relative to the player's center
	float spawnDistance = player->w / 2.0f + bullet->w / 2.0f; // Distance from player center to bullet spawn point

	// Calculate spawn offsets based on firing angle
	float spawnOffsetX = spawnDistance * cosf(angle);
	float spawnOffsetY = spawnDistance * sinf(angle);

	// Adjust bullet spawn position based on player's facing direction
	bullet->x = player->x + player->w / 2.0f - bullet->w / 2.0f + spawnOffsetX;
	bullet->y = player->y + player->h / 2.0f - bullet->h / 2.0f + spawnOffsetY;

	// Adjust direction based on angle
	bullet->dx = cosf(angle) * PLAYER_BULLET_SPEED;
	bullet->dy = sinf(angle) * PLAYER_BULLET_SPEED;

	bullet->health = 1;
	bullet->side = SIDE_PLAYER;
	bullet->angle = angle; // Store firing angle

	player->reload = 8;
}

static void fireShotgun(float angle)
{
	int i;
	Entity *bullet;
	float bulletSpeed = PLAYER_BULLET_SPEED;
	float spreadAngles[3] = {-0.2f, 0.0f, 0.2f}; // Spread angles for shotgun bullets

	for (i = 0; i < 3; i++)
	{
		bullet = malloc(sizeof(Entity));
		if (!bullet)
		{
			fprintf(stderr, "Failed to allocate memory for shotgun bullet\n");
			exit(1); // Handle memory allocation failure gracefully
		}

		memset(bullet, 0, sizeof(Entity));
		stage.bulletTail->next = bullet;
		stage.bulletTail = bullet;

		bullet->texture = bulletTexture;
		SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

		// Calculate spawn position relative to the player's center
		float spawnDistance = player->w / 2.0f + bullet->w / 2.0f; // Distance from player center to bullet spawn point

		// Calculate spawn offsets based on firing angle and spread
		float spawnOffsetX = spawnDistance * cosf(angle + spreadAngles[i]);
		float spawnOffsetY = spawnDistance * sinf(angle + spreadAngles[i]);

		// Adjust bullet spawn position based on player's center and spawn offsets
		bullet->x = player->x + player->w / 2.0f - bullet->w / 2.0f + spawnOffsetX;
		bullet->y = player->y + player->h / 2.0f - bullet->h / 2.0f + spawnOffsetY;

		// Adjust direction based on angle and spread
		bullet->dx = bulletSpeed * cosf(angle + spreadAngles[i]);
		bullet->dy = bulletSpeed * sinf(angle + spreadAngles[i]);

		bullet->health = 1;
		bullet->side = SIDE_PLAYER;
		bullet->angle = angle + spreadAngles[i]; // Store firing angle with spread

		// Move to the next bullet in the linked list
		stage.bulletTail = bullet;
	}

	player->reload = 8;
}

static void fireLaser(float angle)
{
	Entity *laser;

	laser = malloc(sizeof(Entity));
	if (!laser)
	{
		fprintf(stderr, "Failed to allocate memory for laser\n");
		exit(1); // Handle memory allocation failure gracefully
	}

	memset(laser, 0, sizeof(Entity));
	stage.bulletTail->next = laser;
	stage.bulletTail = laser;

	laser->texture = laserTexture; // Assuming you have a laser texture
	SDL_QueryTexture(laser->texture, NULL, NULL, &laser->w, &laser->h);

	// Define laser size and duration (burst)
	laser->w = 200;			// Example width for a huge space laser
	laser->h = 20;			// Example height for a huge space laser
	int laserDuration = 10; // Frames the laser will last

	// Calculate spawn position relative to the player's center
	float spawnDistance = player->w / 3.0f + laser->w / 3.0f; // Distance from player center to laser spawn point

	// Calculate spawn offsets based on firing angle
	float spawnOffsetX = spawnDistance * cosf(angle);
	float spawnOffsetY = spawnDistance * sinf(angle);

	// Adjust laser spawn position based on player's center and spawn offsets
	laser->x = player->x + player->w / 2.0f - laser->w / 2.0f + spawnOffsetX;
	laser->y = player->y + player->h / 2.0f - laser->h / 2.0f + spawnOffsetY;

	// Adjust direction based on angle
	laser->dx = cosf(angle) * LASER_SPEED; // LASER_SPEED should be defined as a constant
	laser->dy = sinf(angle) * LASER_SPEED;

	laser->health = laserDuration; // Laser lasts for a number of frames
	laser->side = SIDE_PLAYER;
	laser->angle = angle; // Store firing angle

	player->reload = 20; // Adjust reload time for balance, higher for a big burst laser
}

static void fireFlame(float angle)
{
	Entity *flame;

	flame = malloc(sizeof(Entity));
	if (!flame)
	{
		fprintf(stderr, "Failed to allocate memory for flame\n");
		exit(1); // Handle memory allocation failure gracefully
	}

	memset(flame, 0, sizeof(Entity));
	stage.bulletTail->next = flame;
	stage.bulletTail = flame;

	flame->texture = flameTexture; // Assuming you have a flame texture
	SDL_QueryTexture(flame->texture, NULL, NULL, &flame->w, &flame->h);

	// Define flame size and duration (burst)
	flame->w = 200;			// Example width for a huge space flame
	flame->h = 20;			// Example height for a huge space flame
	int flameDuration = 10; // Frames the flame will last

	// Calculate spawn position relative to the player's center
	float spawnDistance = player->w / 3.0f + flame->w / 3.0f; // Distance from player center to flame spawn point

	// Calculate spawn offsets based on firing angle
	float spawnOffsetX = spawnDistance * cosf(angle);
	float spawnOffsetY = spawnDistance * sinf(angle);

	// Adjust flame spawn position based on player's center and spawn offsets
	flame->x = player->x + player->w / 2.0f - flame->w / 2.0f + spawnOffsetX;
	flame->y = player->y + player->h / 2.0f - flame->h / 2.0f + spawnOffsetY;

	// Adjust direction based on angle
	flame->dx = cosf(angle) * FLAME_SPEED; // flame_SPEED should be defined as a constant
	flame->dy = sinf(angle) * FLAME_SPEED;

	flame->health = flameDuration; // flame lasts for a number of frames
	flame->side = SIDE_PLAYER;
	flame->angle = angle; // Store firing angle

	player->reload = 1; // Adjust reload time for balance, higher for a big burst laser
}

static void doEnemies(void)
{
	Entity *e;
	if (stage.gamePaused == false)
	{
		if (player != NULL)
		{
			for (e = stage.fighterHead.next; e != NULL; e = e->next)
			{
				if (e != player) // Ensure the player's position isn't messed with
				{
					// Calculate direction to the player
					float dx = player->x - e->x;
					float dy = player->y - e->y;
					float distance = sqrt(dx * dx + dy * dy);

					// Normalize direction vector and set enemy speed
					if (distance != 0)
					{
						dx /= distance;
						dy /= distance;
					}

					e->x += dx * ENEMY_SPEED;
					e->y += dy * ENEMY_SPEED;

					// Calculate and store the angle of movement
					e->angle = atan2(dy, dx) + M_PI;
					// Use atan2 to get the angle in radians

					// Restrict enemies to within world boundaries
					if (e->x < 10)
					{
						e->x = 10;
					}
					if (e->y < 70)
					{
						e->y = 70;
					}
					if (e->x > WORLD_WIDTH - (e->w + 20))
					{
						e->x = WORLD_WIDTH - (e->w + 20);
					}
					if (e->y > WORLD_HEIGHT - (e->h + 20))
					{
						e->y = WORLD_HEIGHT - (e->h + 20);
					}

					// Fire bullets
					if (player != NULL && --e->reload <= 0)
					{
						fireAlienBullet(e);						  // Example function to fire bullets
						playSound(SND_ALIEN_FIRE, CH_ALIEN_FIRE); // Play firing sound
					}
				}
			}
		}
	}
}

static void fireAlienBullet(Entity *e)
{
	Entity *bullet;

	bullet = malloc(sizeof(Entity));
	memset(bullet, 0, sizeof(Entity));
	stage.bulletTail->next = bullet;
	stage.bulletTail = bullet;

	bullet->x = e->x;
	bullet->y = e->y;
	bullet->health = 1;
	bullet->texture = alienBulletTexture;
	bullet->side = SIDE_ALIEN;
	SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

	bullet->x += (e->w / 2) - (bullet->w / 2);
	bullet->y += (e->h / 2) - (bullet->h / 2);

	// Calculate the slope
	calcSlope(player->x + (player->w / 2), player->y + (player->h / 2), e->x, e->y, &bullet->dx, &bullet->dy);

	bullet->dx *= ALIEN_BULLET_SPEED;
	bullet->dy *= ALIEN_BULLET_SPEED;

	e->reload = (rand() % FPS * 2);
}

void blitRotated(SDL_Texture *texture, int x, int y, float angle)
{
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

	SDL_RenderCopyEx(app.renderer, texture, NULL, &dest, angle * (180.0 / M_PI), NULL, SDL_FLIP_NONE);
}

static void doFighters(void)
{
	Entity *e, *prev;

	prev = &stage.fighterHead;

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		if (stage.gamePaused == false)
		{
			e->y += e->dy;
			e->x += e->dx;
		}

		if ((e != player && e->x - (stage.cameraX - 500) < -e->w) || (e != player && e->y - (stage.cameraY - 500) < -e->h))
		{ // If enemy goes out of bounds
			e->health = 0;
			if (stage.currentEnemyCount > 0)
			{
				stage.currentEnemyCount--;
			}
		}

		if (e->health == 0)
		{
			if (e == player)
			{
				player = NULL;
				Mix_Pause(CH_SHIP_DOWN);
				Mix_Pause(CH_HYPER_DRIVE);
				Mix_Pause(CH_POINTS);
			}

			if (e == stage.fighterTail)
			{
				stage.fighterTail = prev;
			}

			prev->next = e->next;
			free(e);
			e = prev;
		}
		else
		{
			// Check collisions with other fighters
			Entity *other;
			for (other = stage.fighterHead.next; other != NULL; other = other->next)
			{
				if (e != other && collisionWithHitbox(e, other, player))
				{
					handleFighterCollision(e, other);
				}
			}
		}

		prev = e;
	}
}

// Function to check collision with hitbox
bool collisionWithHitbox(Entity *e1, Entity *e2, Entity *player)
{
	if (stage.gamePaused == false)
	{
		// Calculate hitbox size (half the width and height)
		float hitboxSizeX = HITBOX_SIZE * 0.5f;
		float hitboxSizeY = HITBOX_SIZE * 0.5f;

		// Calculate centers
		float e1CenterX = e1->x + e1->w / 2.0f;
		float e1CenterY = e1->y + e1->h / 2.0f;
		float e2CenterX = e2->x + e2->w / 2.0f;
		float e2CenterY = e2->y + e2->h / 2.0f;

		// Check if hitboxes overlap
		if (fabs(e1CenterX - e2CenterX) < hitboxSizeX + e1->w / 2.0f + e2->w / 2.0f &&
			fabs(e1CenterY - e2CenterY) < hitboxSizeY + e1->h / 2.0f + e2->h / 2.0f)
		{
			// Determine if e1 is the player
			bool e1IsPlayer = (e1 == player);

			// Handle specific actions based on whether e1 (player or NPC) collided
			if (e1IsPlayer)
			{
				playerCrashed++;
				if (rand() % 30 == 0)
				{
					if ((systemsDown == false) && (player->systemsCooldown <= 0) && (playerCrashed > 10))
					{
						playSound(SND_SHIP_DOWN, CH_SHIP_DOWN);
						toggleSystemsDown(true);
						player->boostCooldown = 300;
						playerCrashed = 0;
					}
				}
			}

			return true; // Collision detected
		}
	}

	return false; // No collision
}

// Function to handle collision response between fighters
void handleFighterCollision(Entity *e1, Entity *e2)
{
	// Calculate centers
	float e1CenterX = e1->x + e1->w / 2.0f;
	float e1CenterY = e1->y + e1->h / 2.0f;
	float e2CenterX = e2->x + e2->w / 2.0f;
	float e2CenterY = e2->y + e2->h / 2.0f;

	// Calculate direction vector from e2 to e1
	float dx = e1CenterX - e2CenterX;
	float dy = e1CenterY - e2CenterY;
	float distance = sqrt(dx * dx + dy * dy);

	// Normalize direction vector
	if (distance != 0.0f)
	{
		dx /= distance;
		dy /= distance;
	}

	// Calculate overlap distance
	float overlap = HITBOX_SIZE;

	// Separate entities based on overlap
	e1->x += overlap * dx;
	e1->y += overlap * dy;
	e2->x -= overlap * dx;
	e2->y -= overlap * dy;

	// Calculate relative velocity along the collision normal
	float relativeVelocityX = e1->dx - e2->dx;
	float relativeVelocityY = e1->dy - e2->dy;
	float dotProduct = relativeVelocityX * dx + relativeVelocityY * dy;

	int e1Mass = 50;
	int e2Mass = 50;

	float impulse = -(1.0f + COLLISION_RESTITUTION) * dotProduct / (1.0f / e1Mass + 1.0f / e2Mass);

	e1->dx += impulse / e1Mass * dx;
	e1->dy += impulse / e1Mass * dy;
	e2->dx -= impulse / e2Mass * dx;
	e2->dy -= impulse / e2Mass * dy;

	// Adjust health
	e1->health -= 25;
	e2->health -= 25;

	// Play collision sound
	playSound(SND_SHIP_HIT, CH_ANY);

	// Handle entity death and effects
	if (e2->health <= 0)
	{
		e2->health = 0;
		if (stage.currentEnemyCount > 0)
		{
			stage.currentEnemyCount--;
		}
		Mix_Pause(CH_ALIEN_DIES);
		playSound(SND_ALIEN_DIE, CH_ALIEN_DIES);
		addExplosions(e2->x, e2->y, 32);
		addDebris(e2);
		if (rand() % 30 == 0)
		{
			addShotgunPods(e2->x + e2->w / 2, e2->y + e2->h / 2);
		}
		if (rand() % 100 == 0)
		{
			addLaserPods(e2->x + e2->w / 2, e2->y + e2->h / 2);
		}
		if (rand() % 200 == 0)
		{
			addFlamePods(e2->x + e2->w / 2, e2->y + e2->h / 2);
		}
		if (rand() % 20 == 0)
		{
			addHealthPods(e2->x + e2->w / 2, e2->y + e2->h / 2);
		}
	}
	if (e1->health <= 0)
	{
		e1->health = 0;
		Mix_Pause(CH_PLAYER_DIES);
		playSound(SND_PLAYER_DIE, CH_PLAYER_DIES);
		addExplosions(e1->x, e1->y, 32);
		addDebris(e1);
	}
}

static void doBullets(void)
{
	Entity *b, *prev;

	prev = &stage.bulletHead;

	for (b = stage.bulletHead.next; b != NULL; b = b->next)
	{
		// Update bullet position based on velocity
		if (stage.gamePaused == false)
		{
			b->x += b->dx;
			b->y += b->dy;
		}

		// Check if the bullet hits an entity or goes out of bounds
		if ((laserEnabled == 1) && (b->x - stage.cameraX > SCREEN_WIDTH || b->y - stage.cameraY > SCREEN_HEIGHT))
		{

			if (b == stage.bulletTail)
			{
				stage.bulletTail = prev;
			}

			prev->next = b->next;
			free(b);
			b = prev;
		}
		else if ((laserEnabled == 1) && (bulletHitFighter(b)))
		{
			// The bullet goes through the enemy and does damage
		}

		if ((flameEnabled == 1) && (b->x - stage.cameraX > SCREEN_WIDTH || b->y - stage.cameraY > SCREEN_HEIGHT))
		{

			if (b == stage.bulletTail)
			{
				stage.bulletTail = prev;
			}

			prev->next = b->next;
			free(b);
			b = prev;
		}
		else if ((flameEnabled == 1) && (bulletHitFighter(b)))
		{
			// The bullet goes through the enemy and does damage
		}

		else if ((laserEnabled != 1) && (bulletHitFighter(b) || b->x - stage.cameraX < -b->w || b->y - stage.cameraY < -b->h || b->x - stage.cameraX > SCREEN_WIDTH || b->y - stage.cameraY > SCREEN_HEIGHT))
		{
			if (b == stage.bulletTail)
			{
				stage.bulletTail = prev;
			}

			prev->next = b->next;
			free(b);
			b = prev;
		}

		prev = b;
	}
}

static int bulletHitFighter(Entity *b)
{
	Entity *e;

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		if (e->side != b->side && collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h))
		{
			if (laserEnabled == 1)
			{
				b->health = 1;
				if (e != player)
				{
					if (stage.gamePaused == false)
					{
						e->health = 0;
					}
				}
			}
			else if (laserEnabled != 1)
			{
				b->health = 0;
			}

			if (flameEnabled == 1)
			{
				b->health = 1;
				if (e != player)
				{
					if (stage.gamePaused == false)
					{
						e->health -= .00000001;
					}
				}
			}
			else if (flameEnabled != 1)
			{
				b->health = 0;
			}

			if (e->health > 5)
			{
				if (flameEnabled != 1)
				{
					if (stage.gamePaused == false)
					{
						e->health -= 5;
					}
				}
			}
			else
			{
				e->health = 0;
				addExplosions(e->x, e->y, 32);
				addDebris(e);
				if (e == player)
				{
					Mix_Pause(CH_PLAYER_DIES);
					playSound(SND_PLAYER_DIE, CH_PLAYER_DIES);
				}
				else
				{
					if (rand() % 30 == 0)
					{
						addShotgunPods(e->x + e->w / 2, e->y + e->h / 2);
					}
					if (rand() % 100 == 0)
					{
						addLaserPods(e->x + e->w / 2, e->y + e->h / 2);
					}
					if (rand() % 200 == 0)
					{
						addFlamePods(e->x + e->w / 2, e->y + e->h / 2);
					}
					if (rand() % 20 == 0)
					{
						addHealthPods(e->x + e->w / 2, e->y + e->h / 2);
					}
					stage.score++;
					Mix_Pause(CH_ALIEN_DIES);
					playSound(SND_ALIEN_DIE, CH_ALIEN_DIES);
					if (stage.currentEnemyCount > 0)
					{
						stage.currentEnemyCount--;
					}
				}
			}

			if (e == player)
			{
				if (stage.gamePaused == false)
				{
					playSound(SND_SHIP_HIT, CH_PLAYER);
				}
			}

			return 1;
		}
	}

	return 0;
}

static void clipPlayer(void)
{
	if (player != NULL)
	{
		if (player->x < 0)
		{
			player->x = 0;
		}
		if (player->y < 70)
		{
			player->y = 70;
		}
		if (player->x > WORLD_WIDTH - player->w)
		{
			player->x = WORLD_WIDTH - player->w;
		}
		if (player->y > WORLD_HEIGHT - player->h)
		{
			player->y = WORLD_HEIGHT - player->h;
		}
	}
}

static void doExplosions(void)
{
	Explosion *e, *prev;

	if (stage.gamePaused == false)
	{

		prev = &stage.explosionHead;

		for (e = stage.explosionHead.next; e != NULL; e = e->next)
		{
			e->x += e->dx;
			e->y += e->dy;

			if (--e->a <= 0)
			{
				if (e == stage.explosionTail)
				{
					stage.explosionTail = prev;
				}

				prev->next = e->next;
				free(e);
				e = prev;
			}

			prev = e;
		}
	}
}

static void doDebris(void)
{
	Debris *d, *prev;

	if (stage.gamePaused == false)
	{

		prev = &stage.debrisHead;

		for (d = stage.debrisHead.next; d != NULL; d = d->next)
		{
			d->x += d->dx;
			d->y += d->dy;

			d->dy += 0.5;

			if (--d->life <= 0)
			{
				if (d == stage.debrisTail)
				{
					stage.debrisTail = prev;
				}

				prev->next = d->next;
				free(d);
				d = prev;
			}

			prev = d;
		}
	}
}

static void doShotgunPods(void)
{
	Entity *e, *prev;
	Uint32 currentTime;

	if (stage.gamePaused == false)
	{
		// Get current time in milliseconds
		currentTime = SDL_GetTicks();

		// Check if shotgun ability should be disabled due to time limit
		if (shotgunEnabled && (currentTime - shotgunStartTime - totalPauseDuration >= shotgunDuration * 1000)) // Convert seconds to milliseconds
		{
			shotgunEnabled = 0;
			// Optionally perform cleanup or notify the player that shotgun ability expired
		}

		prev = &stage.shotgunPodHead;

		for (e = stage.shotgunPodHead.next; e != NULL; e = e->next)
		{
			// Check if entity is out of bounds and adjust direction
			if (e->x < 0)
			{
				e->x = 0;
				e->dx = -e->dx;
			}

			if (e->x + e->w > WORLD_WIDTH)
			{
				e->x = WORLD_WIDTH - e->w;
				e->dx = -e->dx;
			}

			if (e->y < 70)
			{
				e->y = 70;
				e->dy = -e->dy;
			}

			if (e->y + e->h > WORLD_HEIGHT)
			{
				e->y = WORLD_HEIGHT - e->h;
				e->dy = -e->dy;
			}

			// Move entity
			e->x += e->dx;
			e->y += e->dy;

			// Example collision logic that enables shotgun ability
			if (player != NULL && collision(e->x, e->y, e->w, e->h, player->x, player->y, player->w, player->h))
			{
				e->health = 0;

				shotgunEnabled = 1;
				laserEnabled = 0;
				flameEnabled = 0;
				shotgunStartTime = SDL_GetTicks();
				totalPauseDuration = 0; // Reset total pause duration when ability is enabled
				laserDuration = 0;
				shotgunDuration = 10;

				playSound(SND_POINTS, CH_POINTS);
			}

			// Check entity health and remove if necessary
			if (--e->health <= 0)
			{
				if (e == stage.shotgunPodTail)
				{
					stage.shotgunPodTail = prev;
				}

				prev->next = e->next;
				free(e);
				e = prev;
			}

			prev = e;
		}
	}
}

static void doLaserPods(void)
{
	Entity *e, *prev;
	Uint32 currentTime;

	if (stage.gamePaused == false)
	{
		// Get current time in milliseconds
		currentTime = SDL_GetTicks();

		// Check if shotgun ability should be disabled due to time limit
		if (laserEnabled && (currentTime - laserStartTime - totalPauseDuration >= laserDuration * 1000)) // Convert seconds to milliseconds
		{
			laserEnabled = 0;
			// Optionally perform cleanup or notify the player that laser ability expired
		}

		prev = &stage.laserPodHead;

		for (e = stage.laserPodHead.next; e != NULL; e = e->next)
		{
			// Check if entity is out of bounds and adjust direction
			if (e->x < 0)
			{
				e->x = 0;
				e->dx = -e->dx;
			}

			if (e->x + e->w > WORLD_WIDTH)
			{
				e->x = WORLD_WIDTH - e->w;
				e->dx = -e->dx;
			}

			if (e->y < 70)
			{
				e->y = 70;
				e->dy = -e->dy;
			}

			if (e->y + e->h > WORLD_HEIGHT)
			{
				e->y = WORLD_HEIGHT - e->h;
				e->dy = -e->dy;
			}

			// Move entity
			e->x += e->dx;
			e->y += e->dy;

			// Example collision logic that enables laser ability
			if (player != NULL && collision(e->x, e->y, e->w, e->h, player->x, player->y, player->w, player->h))
			{
				e->health = 0;

				laserEnabled = 1;
				shotgunEnabled = 0;
				flameEnabled = 0;
				laserStartTime = SDL_GetTicks();
				totalPauseDuration = 0; // Reset total pause duration when ability is enabled
				shotgunDuration = 0;
				flameDuration = 0;
				laserDuration = 10;

				playSound(SND_POINTS, CH_POINTS);
			}

			// Check entity health and remove if necessary
			if (--e->health <= 0)
			{
				if (e == stage.laserPodTail)
				{
					stage.laserPodTail = prev;
				}

				prev->next = e->next;
				free(e);
				e = prev;
			}

			prev = e;
		}
	}
}

static void doFlamePods(void)
{
	Entity *e, *prev;
	Uint32 currentTime;

	if (stage.gamePaused == false)
	{
		// Get current time in milliseconds
		currentTime = SDL_GetTicks();

		// Check if shotgun ability should be disabled due to time limit
		if (flameEnabled && (currentTime - flameStartTime - totalPauseDuration >= flameDuration * 1000)) // Convert seconds to milliseconds
		{
			flameEnabled = 0;
			// Optionally perform cleanup or notify the player that laser ability expired
		}

		prev = &stage.flamePodHead;

		for (e = stage.flamePodHead.next; e != NULL; e = e->next)
		{
			// Check if entity is out of bounds and adjust direction
			if (e->x < 0)
			{
				e->x = 0;
				e->dx = -e->dx;
			}

			if (e->x + e->w > WORLD_WIDTH)
			{
				e->x = WORLD_WIDTH - e->w;
				e->dx = -e->dx;
			}

			if (e->y < 70)
			{
				e->y = 70;
				e->dy = -e->dy;
			}

			if (e->y + e->h > WORLD_HEIGHT)
			{
				e->y = WORLD_HEIGHT - e->h;
				e->dy = -e->dy;
			}

			// Move entity
			e->x += e->dx;
			e->y += e->dy;

			// Example collision logic that enables flame ability
			if (player != NULL && collision(e->x, e->y, e->w, e->h, player->x, player->y, player->w, player->h))
			{
				e->health = 0;

				flameEnabled = 1;
				shotgunEnabled = 0;
				laserEnabled = 0;
				flameStartTime = SDL_GetTicks();
				totalPauseDuration = 0; // Reset total pause duration when ability is enabled
				shotgunDuration = 0;
				laserDuration = 0;
				flameDuration = 10;

				playSound(SND_POINTS, CH_POINTS);
			}

			// Check entity health and remove if necessary
			if (--e->health <= 0)
			{
				if (e == stage.flamePodTail)
				{
					stage.flamePodTail = prev;
				}

				prev->next = e->next;
				free(e);
				e = prev;
			}

			prev = e;
		}
	}
}

static void doHealthPods(void)
{
	Entity *e, *prev;

	if (stage.gamePaused == false)
	{

		prev = &stage.healthPodHead;

		for (e = stage.healthPodHead.next; e != NULL; e = e->next)
		{
			// Check if entity is out of bounds and adjust direction
			if (e->x < 0)
			{
				e->x = 0;
				e->dx = -e->dx;
			}

			if (e->x + e->w > WORLD_WIDTH)
			{
				e->x = WORLD_WIDTH - e->w;
				e->dx = -e->dx;
			}

			if (e->y < 70)
			{
				e->y = 70;
				e->dy = -e->dy;
			}

			if (e->y + e->h > WORLD_HEIGHT)
			{
				e->y = WORLD_HEIGHT - e->h;
				e->dy = -e->dy;
			}

			// Move entity
			e->x += e->dx;
			e->y += e->dy;

			// If you collide with the pod you get health
			if (player != NULL && collision(e->x, e->y, e->w, e->h, player->x, player->y, player->w, player->h))
			{
				e->health = 0; // Delete the health pod
				player->health += 200;

				playSound(SND_POINTS, CH_POINTS);
			}

			// Check entity health and remove if necessary
			if (--e->health <= 0)
			{
				if (e == stage.healthPodTail)
				{
					stage.healthPodTail = prev;
				}

				prev->next = e->next;
				free(e);
				e = prev;
			}

			prev = e;
		}
	}
}

static void addExplosions(int x, int y, int num)
{
	Explosion *e;
	int i;

	for (i = 0; i < num; i++)
	{
		e = malloc(sizeof(Explosion));
		memset(e, 0, sizeof(Explosion));
		stage.explosionTail->next = e;
		stage.explosionTail = e;

		e->x = x + (rand() % 32) - (rand() % 32);
		e->y = y + (rand() % 32) - (rand() % 32);
		e->dx = (rand() % 10) - (rand() % 10);
		e->dy = (rand() % 10) - (rand() % 10);

		e->dx /= 10;
		e->dy /= 10;

		switch (rand() % 4)
		{
		case 0:
			e->r = 255;
			break;

		case 1:
			e->r = 255;
			e->g = 128;
			break;

		case 2:
			e->r = 255;
			e->g = 255;
			break;

		default:
			e->r = 255;
			e->g = 255;
			e->b = 255;
			break;
		}

		e->a = rand() % FPS * 3;
	}
}

static void addDebris(Entity *e)
{
	Debris *d;
	int x, y, w, h;

	w = e->w / 2;
	h = e->h / 2;

	for (y = 0; y <= h; y += h)
	{
		for (x = 0; x <= w; x += w)
		{
			d = malloc(sizeof(Debris));
			memset(d, 0, sizeof(Debris));
			stage.debrisTail->next = d;
			stage.debrisTail = d;

			d->x = e->x + e->w / 2;
			d->y = e->y + e->h / 2;
			d->dx = (rand() % 5) - (rand() % 5);
			d->dy = -(5 + (rand() % 12));
			d->life = FPS * 2;
			d->texture = e->texture;

			d->rect.x = x;
			d->rect.y = y;
			d->rect.w = w;
			d->rect.h = h;
		}
	}
}

static void addShotgunPods(int x, int y)
{
	Entity *e;

	e = malloc(sizeof(Entity));
	memset(e, 0, sizeof(Entity));
	stage.shotgunPodTail->next = e;
	stage.shotgunPodTail = e;

	e->x = x;
	e->y = y;
	e->dx = -(rand() % 5);
	e->dy = (rand() % 5) - (rand() % 5);
	e->health = FPS * 10;
	e->texture = pointsTexture;

	SDL_QueryTexture(e->texture, NULL, NULL, &e->w, &e->h);

	e->x -= e->w / 2;
	e->y -= e->h / 2;
}

static void addLaserPods(int x, int y)
{
	Entity *e;

	e = malloc(sizeof(Entity));
	memset(e, 0, sizeof(Entity));
	stage.laserPodTail->next = e;
	stage.laserPodTail = e;

	e->x = x;
	e->y = y;
	e->dx = -(rand() % 5);
	e->dy = (rand() % 5) - (rand() % 5);
	e->health = FPS * 10;
	e->texture = laserPodTexture;

	SDL_QueryTexture(e->texture, NULL, NULL, &e->w, &e->h);

	e->x -= e->w / 2;
	e->y -= e->h / 2;
}

static void addFlamePods(int x, int y)
{
	Entity *e;

	e = malloc(sizeof(Entity));
	memset(e, 0, sizeof(Entity));
	stage.flamePodTail->next = e;
	stage.flamePodTail = e;

	e->x = x;
	e->y = y;
	e->dx = -(rand() % 5);
	e->dy = (rand() % 5) - (rand() % 5);
	e->health = FPS * 10;
	e->texture = flamePodTexture;

	SDL_QueryTexture(e->texture, NULL, NULL, &e->w, &e->h);

	e->x -= e->w / 2;
	e->y -= e->h / 2;
}

static void addHealthPods(int x, int y)
{
	Entity *e;

	e = malloc(sizeof(Entity));
	memset(e, 0, sizeof(Entity));
	stage.healthPodTail->next = e;
	stage.healthPodTail = e;

	e->x = x;
	e->y = y;
	e->dx = -(rand() % 5);
	e->dy = (rand() % 5) - (rand() % 5);
	e->health = FPS * 10;
	e->texture = healthTexture;

	SDL_QueryTexture(e->texture, NULL, NULL, &e->w, &e->h);

	e->x -= e->w / 2;
	e->y -= e->h / 2;
}

static void draw(void)
{
	drawBackground();

	drawStarfield();

	drawShotgunPods();
	drawLaserPods();
	drawFlamePods();
	drawHealthPods();

	drawFighters();

	drawDebris();

	drawExplosions();

	drawBullets();

	drawHud();

	if (player != NULL)
	{
		// Calculate rotation angle in degrees (SDL requires degrees)
		float rotationAngle = aim_angle * (180.0f / M_PI);

		// Center of rotation (if needed)
		SDL_Point rotationCenter = {player->w / 2, player->h / 2};

		// Render the player's texture with rotation
		SDL_Rect playerRect = {player->x - stage.cameraX, player->y - stage.cameraY, player->w, player->h};

		// Draw boost effect if boost is active
		if (player->boostActive)
		{
			if (hyperDriveSoundPlayed == false)
			{
				playSound(SND_HYPER_DRIVE, CH_HYPER_DRIVE);
			}
			hyperDriveSoundPlayed = true;

			if (flameGifFrame == 41)
			{
				flameGifFrame = 0;
			}

			flameGifFrame++;

			if (flameGifFrame <= 10)
			{
				currentTexture = playerBoostTexture1;
			}
			else if (flameGifFrame <= 20)
			{
				currentTexture = playerBoostTexture2;
			}
			else if (flameGifFrame <= 30)
			{
				currentTexture = playerBoostTexture3;
			}
			else if (flameGifFrame <= 40)
			{
				currentTexture = playerBoostTexture4;
			}
			SDL_RenderCopyEx(app.renderer, currentTexture, NULL, &playerRect, rotationAngle, &rotationCenter, SDL_FLIP_NONE);
		}
		else
		{ // Draw the player normally by default
			SDL_RenderCopyEx(app.renderer, playerTexture, NULL, &playerRect, rotationAngle, &rotationCenter, SDL_FLIP_NONE);
		}
	}
}

static void drawShotgunPods(void)
{
	Entity *e;

	for (e = stage.shotgunPodHead.next; e != NULL; e = e->next)
	{
		if (e->health > (FPS * 2) || e->health % 12 < 6)
		{
			blit(e->texture, e->x - stage.cameraX, e->y - stage.cameraY);
		}
	}
}

static void drawLaserPods(void)
{
	Entity *e;

	for (e = stage.laserPodHead.next; e != NULL; e = e->next)
	{
		if (e->health > (FPS * 2) || e->health % 12 < 6)
		{
			blit(e->texture, e->x - stage.cameraX, e->y - stage.cameraY);
		}
	}
}

static void drawFlamePods(void)
{
	Entity *e;

	for (e = stage.flamePodHead.next; e != NULL; e = e->next)
	{
		if (e->health > (FPS * 2) || e->health % 12 < 6)
		{
			blit(e->texture, e->x - stage.cameraX, e->y - stage.cameraY);
		}
	}
}

static void drawHealthPods(void)
{
	Entity *e;

	for (e = stage.healthPodHead.next; e != NULL; e = e->next)
	{
		if (e->health > (FPS * 2) || e->health % 12 < 6)
		{
			blit(e->texture, e->x - stage.cameraX, e->y - stage.cameraY);
		}
	}
}

static void drawFighters(void)
{
	Entity *e;

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		if (e->side == SIDE_ALIEN) // Draw only aliens (not the player)
		{
			blitRotated(e->texture, e->x - stage.cameraX, e->y - stage.cameraY, e->angle); // Draw enemies with rotation
		}
	}
}

static void drawBullets(void)
{
	Entity *b;

	for (b = stage.bulletHead.next; b != NULL; b = b->next)
	{
		// Calculate rotation angle in degrees (SDL requires degrees)
		float rotationAngle = b->angle * (180.0f / M_PI);

		// Center of rotation (if needed)
		SDL_Point rotationCenter = {b->w / 2, b->h / 2};

		// Render the bullet's texture with rotation
		SDL_Rect bulletRect = {(int)b->x - stage.cameraX, (int)b->y - stage.cameraY, b->w, b->h};
		SDL_RenderCopyEx(app.renderer, b->texture, NULL, &bulletRect, rotationAngle, &rotationCenter, SDL_FLIP_NONE);
	}
}

static void drawDebris(void)
{
	Debris *d;

	for (d = stage.debrisHead.next; d != NULL; d = d->next)
	{
		blitRect(d->texture, &d->rect, d->x - stage.cameraX, d->y - stage.cameraY);
	}
}

static void drawExplosions(void)
{
	Explosion *e;

	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(explosionTexture, SDL_BLENDMODE_ADD);

	for (e = stage.explosionHead.next; e != NULL; e = e->next)
	{
		SDL_SetTextureColorMod(explosionTexture, e->r, e->g, e->b);
		SDL_SetTextureAlphaMod(explosionTexture, e->a);

		blit(explosionTexture, e->x - stage.cameraX, e->y - stage.cameraY);
	}

	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
}

static void drawHud(void)
{
	// Draw the score
	drawText(SCREEN_WIDTH - 10, 40, 255, 255, 255, TEXT_RIGHT, "SCORE: %03d", stage.score);

	// Draw the high score
	if (stage.score < highscores.highscore[0].score)
	{
		drawText(SCREEN_WIDTH - 10, 10, 255, 255, 255, TEXT_RIGHT, "HIGHSCORE: %03d", highscores.highscore[0].score);
	}
	else
	{
		drawText(SCREEN_WIDTH - 10, 10, 0, 255, 0, TEXT_RIGHT, "HIGHSCORE: %03d", stage.score);
	}

	// Check if player is valid
	if (player != NULL && player->health > 0)
	{
		// Draw the health bar background
		SDL_Rect healthBarBgRect = {HEALTH_BAR_X, HEALTH_BAR_Y, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT};
		SDL_SetRenderDrawColor(app.renderer, 40, 40, 40, 255); // Black color
		SDL_RenderFillRect(app.renderer, &healthBarBgRect);

		// Draw the current health bar
		float healthPercentage = (float)player->health / player->maxHealth;
		int healthBarWidth = (int)(HEALTH_BAR_WIDTH * healthPercentage);

		SDL_Rect healthBarRect = {HEALTH_BAR_X, HEALTH_BAR_Y, healthBarWidth, HEALTH_BAR_HEIGHT};
		SDL_SetRenderDrawColor(app.renderer, 0, 255, 0, 255); // Green color for health bar
		SDL_RenderFillRect(app.renderer, &healthBarRect);

		// Draw enemy health bars
		Entity *enemy;
		for (enemy = stage.fighterHead.next; enemy != NULL; enemy = enemy->next)
		{
			if (enemy != player) // Only draw the health bar for enemies
			{
				// Calculate health percentage
				float enemyHealthPercentage = (float)enemy->health / enemy->maxHealth;

				// Calculate width of health bar based on percentage
				int enemyHealthBarWidth = (int)(enemy->w * enemyHealthPercentage);
				int initialHealthBarWidth = (int)(enemy->w * (float)50 / enemy->maxHealth);

				// Calculate position below the enemy sprite
				int enemyHealthBarX = enemy->x - stage.cameraX + (enemy->w - initialHealthBarWidth) / 2; // Center health bar horizontally under the enemy
				int enemyHealthBarY = enemy->y - stage.cameraY + enemy->h + 5;							 // Adjust vertically below the enemy sprite, e.g., 5 pixels down

				// Draw grey background for health bar
				SDL_Rect enemyHealthBarBgRect = {enemyHealthBarX, enemyHealthBarY, enemy->w, 4}; // Adjust height and position as needed
				SDL_SetRenderDrawColor(app.renderer, 40, 40, 40, 255);							 // Dark grey color for background
				SDL_RenderFillRect(app.renderer, &enemyHealthBarBgRect);

				// Draw health bar based on current health
				SDL_Rect enemyHealthBarRect = {enemyHealthBarX, enemyHealthBarY, enemyHealthBarWidth, 4}; // Adjust height and position as needed
				SDL_SetRenderDrawColor(app.renderer, 255, 0, 0, 255);									  // Red color for health bar
				SDL_RenderFillRect(app.renderer, &enemyHealthBarRect);
			}
		}

		if (shotgunEnabled == 1)
		{
			Uint32 currentTime;
			Uint32 elapsedShotgunTime;
			float remainingTimePercentage;

			// Get current time in milliseconds
			currentTime = SDL_GetTicks();

			if (!stage.gamePaused)
			{
				// Calculate elapsed time only when game is not paused
				elapsedShotgunTime = currentTime - shotgunStartTime - totalPauseDuration;
			}
			else
			{
				// When the game is paused, use the last calculated elapsed time
				elapsedShotgunTime = pauseStartTime - shotgunStartTime - totalPauseDuration;
			}

			remainingTimePercentage = 1.0f - (float)elapsedShotgunTime / (shotgunDuration * 1000);

			// Clamp the percentage to [0, 1] range
			if (remainingTimePercentage < 0)
				remainingTimePercentage = 0;
			else if (remainingTimePercentage > 1)
				remainingTimePercentage = 1;

			// Draw the timer bar for shotgun ability
			int timerBarWidth = (int)(ABILITY_BAR_WIDTH * remainingTimePercentage);
			SDL_Rect timerBarRect = {ABILITY_BAR_X, ABILITY_BAR_Y, timerBarWidth, ABILITY_BAR_HEIGHT};
			SDL_SetRenderDrawColor(app.renderer, 0, 0, 255, 255); // Blue color for timer bar
			SDL_RenderFillRect(app.renderer, &timerBarRect);
		}

		else if (laserEnabled == 1)
		{
			Uint32 currentTime;
			Uint32 elapsedLaserTime;
			float remainingTimePercentage;

			// Get current time in milliseconds
			currentTime = SDL_GetTicks();

			if (!stage.gamePaused)
			{
				// Calculate elapsed time only when game is not paused
				elapsedLaserTime = currentTime - laserStartTime - totalPauseDuration;
			}
			else
			{
				// When the game is paused, use the last calculated elapsed time
				elapsedLaserTime = pauseStartTime - laserStartTime - totalPauseDuration;
			}

			remainingTimePercentage = 1.0f - (float)elapsedLaserTime / (laserDuration * 1000);

			// Clamp the percentage to [0, 1] range
			if (remainingTimePercentage < 0)
				remainingTimePercentage = 0;
			else if (remainingTimePercentage > 1)
				remainingTimePercentage = 1;

			// Draw the timer bar for shotgun ability
			int timerBarWidth = (int)(ABILITY_BAR_WIDTH * remainingTimePercentage);
			SDL_Rect timerBarRect = {ABILITY_BAR_X, ABILITY_BAR_Y, timerBarWidth, ABILITY_BAR_HEIGHT};
			SDL_SetRenderDrawColor(app.renderer, 0, 0, 255, 255); // Blue color for timer bar
			SDL_RenderFillRect(app.renderer, &timerBarRect);
		}

		else if (flameEnabled == 1)
		{
			Uint32 currentTime;
			Uint32 elapsedFlameTime;
			float remainingTimePercentage;

			// Get current time in milliseconds
			currentTime = SDL_GetTicks();

			if (!stage.gamePaused)
			{
				// Calculate elapsed time only when game is not paused
				elapsedFlameTime = currentTime - flameStartTime - totalPauseDuration;
			}
			else
			{
				// When the game is paused, use the last calculated elapsed time
				elapsedFlameTime = pauseStartTime - flameStartTime - totalPauseDuration;
			}

			remainingTimePercentage = 1.0f - (float)elapsedFlameTime / (flameDuration * 1000);

			// Clamp the percentage to [0, 1] range
			if (remainingTimePercentage < 0)
				remainingTimePercentage = 0;
			else if (remainingTimePercentage > 1)
				remainingTimePercentage = 1;

			// Draw the timer bar for shotgun ability
			int timerBarWidth = (int)(ABILITY_BAR_WIDTH * remainingTimePercentage);
			SDL_Rect timerBarRect = {ABILITY_BAR_X, ABILITY_BAR_Y, timerBarWidth, ABILITY_BAR_HEIGHT};
			SDL_SetRenderDrawColor(app.renderer, 0, 0, 255, 255); // Blue color for timer bar
			SDL_RenderFillRect(app.renderer, &timerBarRect);
		}

		// Calculate boost availability percentage
		float boostPercentage = (float)player->boostTimer / 120.0f; // Assuming boost lasts 2 seconds (120 frames)

		// Clamp the boost percentage to [0, 1] range
		if (boostPercentage < 0)
			boostPercentage = 0;
		else if (boostPercentage > 1)
			boostPercentage = 1;

		// Draw the boost bar background
		SDL_Rect boostBarBgRect = {BOOST_BAR_X, BOOST_BAR_Y, BOOST_BAR_WIDTH, BOOST_BAR_HEIGHT};
		SDL_SetRenderDrawColor(app.renderer, 40, 40, 40, 255); // Dark grey background
		SDL_RenderFillRect(app.renderer, &boostBarBgRect);

		bool boostAvailable = (player->boostTimer <= 0 && player->boostCooldown <= 0);
		if (boostAvailable)
		{
			// Boost is available, draw yellow bar
			SDL_Rect boostBarRect = {BOOST_BAR_X, BOOST_BAR_Y, BOOST_BAR_WIDTH, BOOST_BAR_HEIGHT};
			SDL_SetRenderDrawColor(app.renderer, 255, 255, 0, 255); // Yellow color for boost bar
			SDL_RenderFillRect(app.renderer, &boostBarRect);
		}
		else if (player->boostTimer > 0)
		{
			// Boost is active, draw yellow bar decreasing
			int boostBarWidth = (int)(BOOST_BAR_WIDTH * boostPercentage);
			SDL_Rect boostBarRect = {BOOST_BAR_X, BOOST_BAR_Y, boostBarWidth, BOOST_BAR_HEIGHT};
			SDL_SetRenderDrawColor(app.renderer, 255, 255, 0, 255); // Yellow color for boost bar
			SDL_RenderFillRect(app.renderer, &boostBarRect);
		}
		else if (player->boostCooldown > 0)
		{
			float cooldownPercentage;
			// Boost is on cooldown, draw grey bar with charging effect
			if (systemsDown == false)
			{
				cooldownPercentage = 1.0f - ((float)player->boostCooldown / 180.0f);
			} // Cooldown is 3 seconds (180 frames)
			else if (systemsDown == true)
			{
				cooldownPercentage = 1.0f - ((float)player->boostCooldown / 300.0f);
			} // Cooldown is 5 seconds (300 frames)
			int cooldownBarWidth = (int)(BOOST_BAR_WIDTH * cooldownPercentage);
			SDL_Rect cooldownBarRect = {BOOST_BAR_X, BOOST_BAR_Y, cooldownBarWidth, BOOST_BAR_HEIGHT};
			SDL_SetRenderDrawColor(app.renderer, 128, 128, 128, 255); // Light grey color for cooldown bar
			SDL_RenderFillRect(app.renderer, &cooldownBarRect);
		}
	}
}

static void toggleBoost(bool activate)
{
	if (activate)
	{
		if (player->boostCooldown <= 0) // Check if boost is not on cooldown
		{
			player->boostTimer = 120;	// Set boost timer to 2 seconds (assuming 60 frames per second)
			PLAYER_SPEED += 7;			// Increment player speed for boost
			player->boostActive = true; // Set boost active flag
		}
	}
	else
	{
		PLAYER_SPEED = 4;			 // Reset player speed to base speed
		player->boostActive = false; // Reset boost active flag
		hyperDriveSoundPlayed = false;
	}
}

static void toggleSystemsDown(bool activate)
{
	if (activate)
	{
		if (player->systemsCooldown <= 0) // Check if boost is not on cooldown
		{
			player->systemsTimer = 300; // Set boost timer to 5 seconds (assuming 60 frames per second)
			player->boostTimer = 0;
			player->boostActive = false;
			systemsDown = true; // Set boost active flag
		}
	}
	else
	{
		PLAYER_SPEED = 4; // Reset player speed to base speed
		hyperDriveSoundPlayed = false;
		systemsDown = false; // Reset boost active flag
	}
}

// Update camera position based on player's movement
void updateCamera()
{
	if (player != NULL)
	{
		// Example: Center camera on player
		stage.cameraX = player->x - (SCREEN_WIDTH / 2);
		stage.cameraY = player->y - (SCREEN_HEIGHT / 2);

		// Clamp camera position to game world boundaries
		if (stage.cameraX < 0)
		{
			stage.cameraX = 0;
		}
		if (stage.cameraY < 0)
		{
			stage.cameraY = 0;
		}
		if (stage.cameraX > WORLD_WIDTH - SCREEN_WIDTH)
		{
			stage.cameraX = WORLD_WIDTH - SCREEN_WIDTH;
		}
		if (stage.cameraY > WORLD_HEIGHT - SCREEN_HEIGHT)
		{
			stage.cameraY = WORLD_HEIGHT - SCREEN_HEIGHT;
		}
	}
}