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

static void initiateScreenShake(int duration, int magnitude);
static void updateScreenShake(void);
static void drawEnemyBossMessage(int x, int y, int r, int g, int b, char *text);

static void initPlayer(void);
static void initCrossHair(void);
static void initLittleBuddyCrossHair(void);
static void drawCrosshair(void);
static void doPlayer(void);

static void initLittleBuddy(void);
static void doLittleBuddy(void);
static void fireLittleBuddyBullet(Entity *shooter, float targetX, float targetY);

static void fireBullet(float angle);
static void fireShotgun(float angle);
static void fireLaser(float angle);
static void fireFlame(float angle);
static void fireSpaceBeam(float angle);

static void doFighters(void);
static void doBullets(void);
static void doBeams(void);
static void drawFighters(void);

static void drawBullets(void);
static void drawBeams(void);
static int bulletHitFighter(Entity *b);
static int beamHitFighter(Beam *b);

static void doEnemies(void);
static void fireAlienBullet(Entity *e);

static void clipPlayer(void);
static void clipCrosshair(void);

static void resetStage(void);

static void drawExplosions(void);
static void doExplosions(void);
static void addExplosions(int x, int y, int num);

static void addDebris(Entity *e);
static void doDebris(void);
static void drawDebris(void);

static void drawHud(void);
static void drawPauseMenu(void);

static void restartGame(void);
static void quitGame(void);

static void doShotgunPods(void);
static void addShotgunPods(int x, int y);
static void drawShotgunPods(void);

static void doLaserPods(void);
static void addLaserPods(int x, int y);
static void drawLaserPods(void);

static void doFlamePods(void);
static void addFlamePods(int x, int y);
static void drawFlamePods(void);

static void doSpaceBeamPods(void);
static void addSpaceBeamPods(int x, int y);
static void drawSpaceBeamPods(void);

static void doHealthPods(void);
static void addHealthPods(int x, int y);
static void drawHealthPods(void);

static void doLittleBuddyPods(void);
static void addLittleBuddyPods(int x, int y);
static void drawLittleBuddyPods(void);

static void toggleBoost(bool activate);
static void toggleSystemsDown(bool activate);

static Entity *player;
static Entity *crossHair;
static Entity *littleBuddy;
static Entity *littleBuddyCrosshair;
static SDL_Texture *bulletTexture;
static SDL_Texture *laserTexture;
static SDL_Texture *flameTexture;
static SDL_Texture *spaceBeamTexture;
static SDL_Texture *enemyTexture;
static SDL_Texture *alienBulletTexture;
static SDL_Texture *playerTexture;
static SDL_Texture *crossHairTexture;
static SDL_Texture *explosionTexture;
static SDL_Texture *pointsTexture;
static SDL_Texture *laserPodTexture;
static SDL_Texture *flamePodTexture;
static SDL_Texture *spaceBeamPodTexture;
static SDL_Texture *healthTexture;
static SDL_Texture *littleBuddyPodTexture;
static SDL_Texture *currentTexture;
static SDL_Texture *playerBoostTexture1;
static SDL_Texture *playerBoostTexture2;
static SDL_Texture *playerBoostTexture3;
static SDL_Texture *playerBoostTexture4;
static SDL_Texture *littleBuddyTexture;
static SDL_Texture *littleBuddyCrosshairTexture;

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

static int spaceBeamEnabled = 0;
static int spaceBeamDuration = 0;
static Uint32 spaceBeamStartTime = 0;

static int littleBuddyEnabled = 0;

static float PLAYER_SPEED = 4;
static float CROSSHAIR_SPEED = 10;
static float aim_angle = 0.0f;

static bool hyperDriveSoundPlayed = false;
static bool spaceBeamSoundPlaying = false;

static int playerCrashed = 0;
static bool systemsDown = false;

static int flameGifFrame = 0;

static int prevStartButtonState;
static int prevRightShoulderState;

static bool playerQuit;

static int flameDamage = 0;

static int crosshairDistance;
static bool autoAim;
static bool playerWantsAutoAim;

Uint32 pauseStartTime = 0;
Uint32 totalPauseDuration = 0;
Uint32 lastPauseTime = 0;

static int shakeDuration = 0;
static int shakeMagnitude = 0;

static bool bossMessageShown;
static int bossMessageDuration;
static int secondBossMessageDuration;

void initStage(void)
{
	app.delegate.logic = logic;
	app.delegate.draw = draw;

	bulletTexture = loadTexture("gfx/playerBullet.png");
	laserTexture = loadTexture("gfx/playerBullet.png");
	flameTexture = loadTexture("gfx/beam.png");
	spaceBeamTexture = loadTexture("gfx/beam.png");
	enemyTexture = loadTexture("gfx/enemy.png");
	alienBulletTexture = loadTexture("gfx/alienBullet.png");
	playerTexture = loadTexture("gfx/player.png");
	crossHairTexture = loadTexture("gfx/crosshair.png");
	explosionTexture = loadTexture("gfx/explosion.png");
	pointsTexture = loadTexture("gfx/shotgun.png");
	laserPodTexture = loadTexture("gfx/laserPod.png");
	flamePodTexture = loadTexture("gfx/flamePod.png");
	spaceBeamPodTexture = loadTexture("gfx/spaceBeamPod.png");
	littleBuddyPodTexture = loadTexture("gfx/littleBuddyPod.png");
	healthTexture = loadTexture("gfx/health.png");
	playerBoostTexture1 = loadTexture("gfx/playerFlamegif_0.png");
	playerBoostTexture2 = loadTexture("gfx/playerFlamegif_1.png");
	playerBoostTexture3 = loadTexture("gfx/playerFlamegif_2.png");
	playerBoostTexture4 = loadTexture("gfx/playerFlamegif_3.png");
	littleBuddyTexture = loadTexture("gfx/littleBuddy.png");
	littleBuddyCrosshairTexture = loadTexture("gfx/littleBuddyCrosshair.png");
	currentTexture = playerBoostTexture3;

	memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);
	memset(app.mouse_buttons, 0, sizeof(int) * MAX_MOUSE_BUTTONS);

	resetStage();

	stage.score = 0;

	stage.cameraX = 0;
	stage.cameraY = 0;

	initPlayer();
	initCrossHair();

	enemySpawnTimer = 0;

	stageResetTimer = FPS * 3;

	spaceBeamDuration = 0;
	shotgunDuration = 0;
	laserDuration = 0;
	flameDuration = 0;

	systemsDown = false;
	playerCrashed = 0;

	hyperDriveSoundPlayed = false;
	spaceBeamSoundPlaying = false;

	PLAYER_SPEED = 4;

	prevStartButtonState = 0;
	prevRightShoulderState = 0;

	pauseStartTime = 0;
	totalPauseDuration = 0;
	lastPauseTime = 0;

	flameDamage = 0;

	crosshairDistance = 75;

	autoAim = false;
	playerWantsAutoAim = false;

	playerQuit = false;

	bossMessageShown = false;
	bossMessageDuration = 0;

	initLittleBuddy();
	initLittleBuddyCrossHair();
	littleBuddyEnabled = 1;
}

static void resetStage(void)
{
	Entity *e;
	Beam *b;
	Explosion *ex;
	Debris *d;

	spaceBeamEnabled = 0;
	shotgunEnabled = 0;
	laserEnabled = 0;
	flameEnabled = 0;
	littleBuddyEnabled = 0;

	systemsDown = false;
	playerCrashed = 0;

	hyperDriveSoundPlayed = false;
	spaceBeamSoundPlaying = false;

	prevStartButtonState = 0;
	prevRightShoulderState = 0;

	pauseStartTime = 0;
	totalPauseDuration = 0;
	lastPauseTime = 0;

	flameDamage = 0;

	crosshairDistance = 75;

	autoAim = false;
	playerWantsAutoAim = false;

	playerQuit = false;

	bossMessageShown = false;
	bossMessageDuration = 0;

	while (stage.fighterHead.next)
	{
		e = stage.fighterHead.next;
		stage.fighterHead.next = e->next;
		free(e);
	}

	while (stage.crossHairHead.next)
	{
		e = stage.crossHairHead.next;
		stage.crossHairHead.next = e->next;
		free(e);
	}

	while (stage.bulletHead.next)
	{
		e = stage.bulletHead.next;
		stage.bulletHead.next = e->next;
		free(e);
	}

	while (stage.beamHead.next)
	{
		b = stage.beamHead.next;
		stage.beamHead.next = b->next;
		free(b);
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

	while (stage.spaceBeamPodHead.next)
	{
		e = stage.spaceBeamPodHead.next;
		stage.spaceBeamPodHead.next = e->next;
		free(e);
	}

	while (stage.healthPodHead.next)
	{
		e = stage.healthPodHead.next;
		stage.healthPodHead.next = e->next;
		free(e);
	}

	while (stage.littleBuddyPodHead.next)
	{
		e = stage.littleBuddyPodHead.next;
		stage.littleBuddyPodHead.next = e->next;
		free(e);
	}

	memset(&stage, 0, sizeof(Stage));
	stage.fighterTail = &stage.fighterHead;
	stage.crossHairTail = &stage.crossHairHead;
	stage.bulletTail = &stage.bulletHead;
	stage.beamTail = &stage.beamHead;
	stage.explosionTail = &stage.explosionHead;
	stage.debrisTail = &stage.debrisHead;
	stage.shotgunPodTail = &stage.shotgunPodHead;
	stage.laserPodTail = &stage.laserPodHead;
	stage.flamePodTail = &stage.flamePodHead;
	stage.spaceBeamPodTail = &stage.spaceBeamPodHead;
	stage.healthPodTail = &stage.healthPodHead;
	stage.littleBuddyPodTail = &stage.littleBuddyPodHead;
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

static void initCrossHair()
{
	crossHair = malloc(sizeof(Entity));
	if (crossHair == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for crossHair\n");
		exit(EXIT_FAILURE);
	}
	memset(crossHair, 0, sizeof(Entity));

	crossHair->w = 100;
	crossHair->h = 100;

	stage.crossHairTail->next = crossHair;
	stage.crossHairTail = crossHair;

	crossHair->texture = crossHairTexture;
	SDL_QueryTexture(crossHair->texture, NULL, NULL, &crossHair->w, &crossHair->h);
}

static void initLittleBuddyCrossHair(void)
{
	littleBuddyCrosshair = malloc(sizeof(Entity));
	if (littleBuddyCrosshair == NULL)
	{
		fprintf(stderr, "Failed to allocate memory for crossHair\n");
		exit(EXIT_FAILURE);
	}
	memset(littleBuddyCrosshair, 0, sizeof(Entity));

	littleBuddyCrosshair->w = 100;
	littleBuddyCrosshair->h = 100;

	stage.crossHairTail->next = littleBuddyCrosshair;
	stage.crossHairTail = littleBuddyCrosshair;

	littleBuddyCrosshair->texture = littleBuddyCrosshairTexture;
	SDL_QueryTexture(littleBuddyCrosshair->texture, NULL, NULL, &littleBuddyCrosshair->w, &littleBuddyCrosshair->h);
}

static void initLittleBuddy()
{
	littleBuddy = malloc(sizeof(Entity));
	memset(littleBuddy, 0, sizeof(Entity));
	stage.fighterTail->next = littleBuddy;
	stage.fighterTail = littleBuddy;

	littleBuddy->maxHealth = 1000;
	littleBuddy->health = littleBuddy->maxHealth;
	littleBuddy->texture = littleBuddyTexture;
	SDL_QueryTexture(littleBuddy->texture, NULL, NULL, &littleBuddy->w, &littleBuddy->h);

	littleBuddy->side = SIDE_PLAYER;

	aim_angle = 0.0f;
}

static void logic(void)
{
	updateCamera();

	doStarfield();

	doPlayer();
	if (littleBuddy != NULL && !stage.gamePaused)
	{
		if (littleBuddyEnabled == 1)
		{
			doLittleBuddy();
		}
		if (littleBuddy->health <= 0)
		{
			littleBuddy->health = 0;
			playSound(SND_ALIEN_DIE, CH_ALIEN_DIES);
			addExplosions(littleBuddy->x, littleBuddy->y, 32);
			addDebris(littleBuddy);
		}
	}

	doEnemies();

	doFighters();

	doBullets();
	doBeams();

	doExplosions();

	doDebris();

	doShotgunPods();
	doLaserPods();
	doFlamePods();
	doSpaceBeamPods();
	doHealthPods();
	doLittleBuddyPods();

	if (stage.gamePaused == false)
	{
		// spawnEnemyFighters();
		spawnEnemyMosquitos();
		// spawnEnemyTheCube();
	}

	clipPlayer();
	clipCrosshair();

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
	if (player != NULL)
	{
		if (stage.gamePaused == false)
		{ // Systems down logic
			if (player->systemsTimer > 0)
			{
				player->systemsTimer--;

				if (player->systemsTimer <= 0)
				{
					toggleSystemsDown(false);	   // Deactivate systems when timer expires
					player->systemsCooldown = 120; // Set cooldown to 2 seconds (120 frames)
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
					player->boostCooldown = 180; // Set cooldown to 3 seconds (180 frames)
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
				if ((systemsDown == false) && (!player->boostActive))
				{
					if (SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_LEFTY) < -8000)
					{
						player->dy = -PLAYER_SPEED;
					}

					if (SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_LEFTY) > 8000)
					{
						player->dy = PLAYER_SPEED;
					}

					if (SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_LEFTX) < -8000)
					{
						player->dx = -PLAYER_SPEED;
					}

					if (SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_LEFTX) > 8000)
					{
						player->dx = PLAYER_SPEED;
					}
				}
				// Calculate aiming direction
				aim_x = SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_RIGHTX);
				aim_y = SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_RIGHTY);
				aim_threshold = 8000.0f;

				if (aim_x * aim_x + aim_y * aim_y > aim_threshold * aim_threshold)
				{
					aim_angle = atan2f((float)aim_y, (float)aim_x);
				}

				if (player->boostActive)
				{
					// Move the player in the direction of the right joystick with boosted speed
					player->dx = cosf(aim_angle) * PLAYER_SPEED;
					player->dy = sinf(aim_angle) * PLAYER_SPEED;
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
						if (!spaceBeamSoundPlaying)
						{
							playBeamSound(SND_LASER_BEAM, CH_PLAYER, true);
							spaceBeamSoundPlaying = true; // Flag to indicate space beam sound is playing
						}
					}
					else if (spaceBeamEnabled == 1)
					{
						fireSpaceBeam(aim_angle);
						if (!spaceBeamSoundPlaying)
						{
							playBeamSound(SND_LASER_BEAM, CH_PLAYER, true);
							spaceBeamSoundPlaying = true; // Flag to indicate space beam sound is playing
						}
					}
					else
					{
						fireBullet(aim_angle);
						playSound(SND_PLAYER_FIRE, CH_PLAYER);
					}
				}
				else
				{
					// Stop the space beam sound when trigger is released
					if (spaceBeamSoundPlaying)
					{
						Mix_HaltChannel(CH_PLAYER);
						spaceBeamSoundPlaying = false;
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
				if (!player->boostActive)
				{
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
				}
				if (app.mouse_buttons[SDL_BUTTON_RIGHT])
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

				if (player->boostActive)
				{
					// Move the player in the direction of the mouse with boosted speed
					player->dx = cosf(aim_angle) * PLAYER_SPEED;
					player->dy = sinf(aim_angle) * PLAYER_SPEED;
				}

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
						if (!spaceBeamSoundPlaying)
						{
							playBeamSound(SND_LASER_BEAM, CH_PLAYER, true);
							spaceBeamSoundPlaying = true; // Flag to indicate space beam sound is playing
						}
					}
					else if (spaceBeamEnabled == 1)
					{
						fireSpaceBeam(aim_angle);
						if (!spaceBeamSoundPlaying)
						{
							playBeamSound(SND_LASER_BEAM, CH_PLAYER, true);
							spaceBeamSoundPlaying = true; // Flag to indicate space beam sound is playing
						}
					}
					else
					{
						fireBullet(aim_angle);
						playSound(SND_PLAYER_FIRE, CH_PLAYER);
					}
				}
				else
				{
					// Stop the space beam sound when trigger is released
					if (spaceBeamSoundPlaying)
					{
						Mix_HaltChannel(CH_PLAYER);
						spaceBeamSoundPlaying = false;
					}
				}
			}
			if (app.keyboard[SDL_SCANCODE_ESCAPE])
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

	if (autoAim && crossHair != NULL)
	{
		// Calculate angle towards crosshair
		float dx = crossHair->x + crossHair->w / 2.0f - (player->x + player->w / 2.0f);
		float dy = crossHair->y + crossHair->h / 2.0f - (player->y + player->h / 2.0f);
		angle = atan2(dy, dx);
		aim_angle = angle;
	}

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

		if (autoAim && crossHair != NULL)
		{
			// Calculate angle towards crosshair
			float dx = crossHair->x + crossHair->w / 2.0f - (player->x + player->w / 2.0f);
			float dy = crossHair->y + crossHair->h / 2.0f - (player->y + player->h / 2.0f);
			angle = atan2(dy, dx);
			aim_angle = angle;
		}

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

	laser->texture = laserTexture;
	SDL_QueryTexture(laser->texture, NULL, NULL, &laser->w, &laser->h);

	// Define laser size and duration (burst)
	laser->w = 200;			// Width for a huge space laser
	laser->h = 20;			// Height for a huge space laser
	int laserDuration = 10; // Frames the laser will last

	// Calculate spawn position relative to the player's center
	float spawnDistance = player->w / 3.0f + laser->w / 3.0f; // Distance from player center to laser spawn point

	// Calculate spawn offsets based on firing angle
	float spawnOffsetX = spawnDistance * cosf(angle);
	float spawnOffsetY = spawnDistance * sinf(angle);

	if (autoAim && crossHair != NULL)
	{
		// Calculate angle towards crosshair
		float dx = crossHair->x + crossHair->w / 2.0f - (player->x + player->w / 2.0f);
		float dy = crossHair->y + crossHair->h / 2.0f - (player->y + player->h / 2.0f);
		angle = atan2(dy, dx);
		aim_angle = angle;
	}

	// Adjust laser spawn position based on player's center and spawn offsets
	laser->x = player->x + player->w / 2.0f - laser->w / 2.0f + spawnOffsetX;
	laser->y = player->y + player->h / 2.0f - laser->h / 2.0f + spawnOffsetY;

	// Adjust direction based on angle
	laser->dx = cosf(angle) * LASER_SPEED;
	laser->dy = sinf(angle) * LASER_SPEED;

	laser->health = laserDuration; // Laser lasts for a number of frames
	laser->side = SIDE_PLAYER;
	laser->angle = angle; // Store firing angle

	player->reload = 20; // Adjust reload time for balance, higher for a big burst laser
}

static void fireFlame(float angle)
{
	Beam *spaceBeam;
	int flameDuration = 10; // Frames each beam will last

	// Define the number of beams along the flame
	int numBeams = 10; // Adjust this for the desired density

	// Calculate the distance between each beam
	float distanceBetweenBeams = 20.0f;

	// Calculate spawn position relative to the player's center
	float spawnDistance = player->w / 3.0f;

	// Set the blend mode for the renderer and texture; Making it pretty
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(flameTexture, SDL_BLENDMODE_ADD);

	// Loop to create beams along the flame path
	for (int i = 0; i < numBeams; i++)
	{
		spaceBeam = malloc(sizeof(Beam));
		if (!spaceBeam)
		{
			fprintf(stderr, "Failed to allocate memory for spaceBeam\n");
			exit(1); // Handle memory allocation failure gracefully
		}

		memset(spaceBeam, 0, sizeof(Beam));
		stage.beamTail->next = spaceBeam;
		stage.beamTail = spaceBeam;

		spaceBeam->beamTexture = flameTexture;
		SDL_QueryTexture(spaceBeam->beamTexture, NULL, NULL, &spaceBeam->w, &spaceBeam->h);

		// Define beam size and duration (burst)
		spaceBeam->w = 150; // Width for a huge space beam
		spaceBeam->h = 100; // Height for a huge space beam

		// Calculate spawn offsets based on firing angle and current beam index
		float spawnOffsetX = spawnDistance * cosf(angle) + i * distanceBetweenBeams * cosf(angle);
		float spawnOffsetY = spawnDistance * sinf(angle) + i * distanceBetweenBeams * sinf(angle);

		if (autoAim && crossHair != NULL)
		{
			// Calculate angle towards crosshair
			float dx = crossHair->x + crossHair->w / 2.0f - (player->x + player->w / 2.0f);
			float dy = crossHair->y + crossHair->h / 2.0f - (player->y + player->h / 2.0f);
			angle = atan2(dy, dx);
			aim_angle = angle;
		}

		// Set beam properties
		spaceBeam->x = player->x + player->w / 2.0f - spaceBeam->w / 2.0f + spawnOffsetX;
		spaceBeam->y = player->y + player->h / 2.0f - spaceBeam->h / 2.0f + spawnOffsetY;
		spaceBeam->dx = cosf(angle) * FLAME_SPEED;
		spaceBeam->dy = sinf(angle) * FLAME_SPEED;
		spaceBeam->health = flameDuration; // Each beam lasts for a number of frames
		spaceBeam->side = SIDE_PLAYER;
		spaceBeam->angle = angle; // Store firing angle

		// Color and alpha modulation with randomness for realism
		spaceBeam->r = 200 + (rand() % 56); // Red value between 200 and 255
		spaceBeam->g = 50 + (rand() % 51);	// Green value between 50 and 100
		spaceBeam->b = 0;					// Blue value fixed at 0 for a more fiery look
		spaceBeam->a = 200 + (rand() % 56); // Alpha value between 200 and 255

		// Set the texture color and alpha modulation
		SDL_SetTextureColorMod(spaceBeam->beamTexture, spaceBeam->r, spaceBeam->g, spaceBeam->b);
		SDL_SetTextureAlphaMod(spaceBeam->beamTexture, spaceBeam->a);

		// Update spawnDistance for the next beam
		spawnDistance += distanceBetweenBeams;

		// Draw the beam
		blit(spaceBeam->beamTexture, spaceBeam->x - stage.cameraX, spaceBeam->y - stage.cameraY);
	}

	// Reset the blend mode for the renderer
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);

	player->reload = 1; // Adjust reload time for balance
}

static void fireSpaceBeam(float angle)
{
	Beam *spaceBeam;
	int spaceBeamDuration = 10; // Frames each beam will last

	// Define the number of beams along the spaceBeam
	int numBeams = 10; // Adjust this for the desired density

	// Calculate the distance between each beam
	float distanceBetweenBeams = 20.0f; // Adjust as needed

	// Calculate spawn position relative to the player's center
	float spawnDistance = player->w / 3.0f;

	// Set the blend mode for the renderer and texture
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_ADD);
	SDL_SetTextureBlendMode(spaceBeamTexture, SDL_BLENDMODE_ADD);

	// Loop to create beams along the spaceBeam path
	for (int i = 0; i < numBeams; i++)
	{
		spaceBeam = malloc(sizeof(Beam));
		if (!spaceBeam)
		{
			fprintf(stderr, "Failed to allocate memory for spaceBeam\n");
			exit(1); // Handle memory allocation failure gracefully
		}

		memset(spaceBeam, 0, sizeof(Beam));
		stage.beamTail->next = spaceBeam;
		stage.beamTail = spaceBeam;

		spaceBeam->beamTexture = spaceBeamTexture;
		SDL_QueryTexture(spaceBeam->beamTexture, NULL, NULL, &spaceBeam->w, &spaceBeam->h);

		// Define beam size and duration (burst)
		spaceBeam->w = 200; // Width for a huge space beam
		spaceBeam->h = 100; // Height for a huge space beam

		// Calculate spawn offsets based on firing angle and current beam index
		float spawnOffsetX = spawnDistance * cosf(angle) + i * distanceBetweenBeams * cosf(angle);
		float spawnOffsetY = spawnDistance * sinf(angle) + i * distanceBetweenBeams * sinf(angle);

		if (autoAim && crossHair != NULL)
		{
			// Calculate angle towards crosshair
			float dx = crossHair->x + crossHair->w / 2.0f - (player->x + player->w / 2.0f);
			float dy = crossHair->y + crossHair->h / 2.0f - (player->y + player->h / 2.0f);
			angle = atan2(dy, dx);
			aim_angle = angle;
		}

		// Set beam properties
		spaceBeam->x = player->x + player->w / 2.0f - spaceBeam->w / 2.0f + spawnOffsetX;
		spaceBeam->y = player->y + player->h / 2.0f - spaceBeam->h / 2.0f + spawnOffsetY;
		spaceBeam->dx = cosf(angle) * FLAME_SPEED;
		spaceBeam->dy = sinf(angle) * FLAME_SPEED;
		spaceBeam->health = spaceBeamDuration; // Each beam lasts for a number of frames
		spaceBeam->side = SIDE_PLAYER;
		spaceBeam->angle = angle; // Store firing angle

		// Color and alpha modulation with randomness for realism
		spaceBeam->r = 255;					// Fixed maximum red value for a bright white base
		spaceBeam->g = 255;					// Fixed maximum green value for a bright white base
		spaceBeam->b = 200 + (rand() % 56); // Blue value with a more prominent blue tint (between 220 and 255)
		spaceBeam->a = 200 + (rand() % 56); // Alpha value between 200 and 255 for transparency variation

		// Set the texture color and alpha modulation
		SDL_SetTextureColorMod(spaceBeam->beamTexture, spaceBeam->r, spaceBeam->g, spaceBeam->b);
		SDL_SetTextureAlphaMod(spaceBeam->beamTexture, spaceBeam->a);

		// Update spawnDistance for the next beam
		spawnDistance += distanceBetweenBeams;

		// Draw the beam
		blit(spaceBeam->beamTexture, spaceBeam->x - stage.cameraX, spaceBeam->y - stage.cameraY);
	}

	// Reset the blend mode for the renderer
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);

	player->reload = 1; // Adjust reload time for balance
}

static void doLittleBuddy(void)
{
	const float chaseEnemySpeed = 3.0f; // Speed of chasing the enemy
	const float stopDistance = 100.0f;	// Distance threshold to stop moving towards enemy
	const float backupDistance = 50.0f; // Distance threshold to start backing away from enemy
	const float backupSpeed = 2.0f;		// Speed of backing away from enemy
	static float angle = 0.0f;			// Initialize orbit angle
	const float radius = 100.0f;		// Distance from player
	const float orbitSpeed = 0.05f;		// Speed of orbit

	// Ensure player and little buddy entities are valid
	if (player == NULL || littleBuddy == NULL)
	{
		fprintf(stderr, "Error: player or little buddy entity is not initialized.\n");
		return;
	}

	// Find the closest enemy to the player's aim direction
	Entity *closestEnemy = NULL;
	float closestDistance = FLT_MAX;
	float playerAngle = aim_angle; // Capture the player's current aiming angle

	for (Entity *e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		if (e != player && e != littleBuddy)
		{
			// Calculate vector from player to enemy
			float dx = e->x + e->w / 2 - (player->x + player->w / 2);
			float dy = e->y + e->h / 2 - (player->y + player->h / 2);
			float distance = sqrt(dx * dx + dy * dy);

			// Calculate angle from player to enemy
			float enemyAngle = atan2(dy, dx);

			// Adjust enemy angle to be within [0, 2*PI)
			if (enemyAngle < 0)
			{
				enemyAngle += 2 * M_PI;
			}

			// Calculate angular difference between player's aim direction and enemy
			float angleDiff = fabs(enemyAngle - playerAngle);

			// Normalize angle difference to be within [0, PI)
			if (angleDiff > M_PI)
			{
				angleDiff = 2 * M_PI - angleDiff;
			}

			// Check if this enemy is within the acceptable angular range
			// Adjust the angular range as needed (here, approximately 45 degrees)
			if (angleDiff < M_PI / 4)
			{
				// Check if this enemy is the closest so far
				if (distance < closestDistance)
				{
					closestEnemy = e;
					closestDistance = distance;
				}
			}

			// Check if enemy is within backup distance
			if (distance < backupDistance)
			{
				// Move away from the enemy
				float backDx = littleBuddy->x - e->x;
				float backDy = littleBuddy->y - e->y;
				float backDistance = sqrt(backDx * backDx + backDy * backDy);

				if (backDistance > 0)
				{
					littleBuddy->dx = backDx / backDistance * backupSpeed;
					littleBuddy->dy = backDy / backDistance * backupSpeed;
				}
			}
		}
	}

	if (closestEnemy != NULL)
	{
		// Move towards the closest enemy
		float dx = closestEnemy->x - littleBuddy->x;
		float dy = closestEnemy->y - littleBuddy->y;
		float distance = sqrt(dx * dx + dy * dy);

		// Calculate normalized direction towards the enemy
		float normalizedDx = dx / distance;
		float normalizedDy = dy / distance;

		// Check if little buddy is within stopping distance from the enemy
		if (distance > stopDistance)
		{
			// Move towards the enemy
			littleBuddy->dx = normalizedDx * chaseEnemySpeed;
			littleBuddy->dy = normalizedDy * chaseEnemySpeed;
		}
		else
		{
			// Stop moving towards the enemy
			littleBuddy->dx = 0.0f;
			littleBuddy->dy = 0.0f;
		}
	}
	else
	{
		// Little buddy moves towards player to orbit
		float orbitCenterX = player->x;
		float orbitCenterY = player->y;

		// Calculate orbit position
		float targetX = orbitCenterX + cos(angle) * radius;
		float targetY = orbitCenterY + sin(angle) * radius;

		// Calculate vector from current position to target position
		float dx = targetX - littleBuddy->x;
		float dy = targetY - littleBuddy->y;
		float distance = sqrt(dx * dx + dy * dy);

		// Check if little buddy is within a small distance to target position
		if (distance > 2.0f) // Adjust this threshold as needed
		{
			// Calculate normalized direction towards target position
			float normalizedDx = dx / distance;
			float normalizedDy = dy / distance;

			// Move little buddy towards the target position
			littleBuddy->dx = normalizedDx * chaseEnemySpeed;
			littleBuddy->dy = normalizedDy * chaseEnemySpeed;
		}
		else
		{
			// Stop little buddy if close enough to the target position
			littleBuddy->dx = 0.0f;
			littleBuddy->dy = 0.0f;

			// Set exact orbit position
			littleBuddy->x = targetX;
			littleBuddy->y = targetY;

			// Update angle for next frame
			angle += orbitSpeed;
			if (angle > 2 * M_PI) // Keep angle within bounds
			{
				angle -= 2 * M_PI;
			}
		}
	}

	// Boundary checks to keep little buddy within screen bounds relative to the camera
	float boundaryX = stage.cameraX;
	float boundaryY = stage.cameraY;
	if (littleBuddy->x < boundaryX + 70)
	{
		littleBuddy->x = boundaryX + 70;
	}
	if (littleBuddy->x > boundaryX + SCREEN_WIDTH - littleBuddy->w - 70)
	{
		littleBuddy->x = boundaryX + SCREEN_WIDTH - littleBuddy->w - 70;
	}
	if (littleBuddy->y < boundaryY + 70)
	{
		littleBuddy->y = boundaryY + 70;
	}
	if (littleBuddy->y > boundaryY + SCREEN_HEIGHT - littleBuddy->h - 70)
	{
		littleBuddy->y = boundaryY + SCREEN_HEIGHT - littleBuddy->h - 70;
	}

	// If closestEnemy is valid, fire at it
	if (closestEnemy != NULL && littleBuddy->reload <= 0)
	{
		float dx = closestEnemy->x - littleBuddy->x;
		float dy = closestEnemy->y - littleBuddy->y;
		littleBuddy->angle = atan2(dy, dx) * (180.0f / M_PI); // Convert to degrees

		// Face correct direction when shooting
		littleBuddy->angle += 180.0f;

		fireLittleBuddyBullet(littleBuddy, closestEnemy->x, closestEnemy->y); // Function to fire bullets
		littleBuddy->reload = 7;											  // Reload time
		playSound(SND_PLAYER_FIRE, CH_ANY);									  // Play firing sound
	}

	// Decrease reload time
	if (littleBuddy->reload > 0)
	{
		littleBuddy->reload--;
	}
}

static void fireLittleBuddyBullet(Entity *shooter, float targetX, float targetY)
{
	Entity *bullet = malloc(sizeof(Entity));
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

	// Calculate direction
	float dx = targetX - (shooter->x + shooter->w / 2.0f);
	float dy = targetY - (shooter->y + shooter->h / 2.0f);
	float dist = sqrt(dx * dx + dy * dy);

	if (dist != 0)
	{
		dx /= dist;
		dy /= dist;
	}

	// Calculate firing angle in radians
	bullet->angle = atan2(dy, dx); // Store firing angle in radians

	// Adjust bullet spawn position based on shooter's center
	bullet->x = shooter->x + shooter->w / 2.0f - bullet->w / 2.0f;
	bullet->y = shooter->y + shooter->h / 2.0f - bullet->h / 2.0f;

	// Adjust direction based on angle
	bullet->dx = dx * PLAYER_BULLET_SPEED;
	bullet->dy = dy * PLAYER_BULLET_SPEED;

	bullet->health = 1;
	bullet->side = SIDE_PLAYER;
}

static void doEnemies(void)
{
	Entity *e;
	if (stage.gamePaused == false)
	{
		for (e = stage.fighterHead.next; e != NULL; e = e->next)
		{
			if (e != player && e != littleBuddy) // Ensure the player's position isn't messed with
			{
				float dx, dy;
				float desiredAngle;

				if (player != NULL && player->health > 0)
				{
					// Calculate direction to the player
					dx = player->x - e->x;
					dy = player->y - e->y;

					float distance = sqrt(dx * dx + dy * dy);

					// Normalize direction vector and set enemy speed
					if (distance != 0)
					{
						dx /= distance;
						dy /= distance;
					}

					e->x += dx * ENEMY_SPEED;
					e->y += dy * ENEMY_SPEED;

					// Calculate the desired angle towards the player
					desiredAngle = atan2(dy, dx) + M_PI;
				}
				else
				{
					// Move left when the player's health is 0
					dx = -1.0; // Move left
					dy = 0.0;

					e->x += dx * ENEMY_SPEED;
					e->y += dy * ENEMY_SPEED;

					// Set the desired angle to face left
					desiredAngle = 2 * (M_PI);
				}

				// Gradually rotate towards the desired angle
				float angleDiff = desiredAngle - e->angle;

				// Ensure the angle difference is within the range [-PI, PI]
				if (angleDiff > M_PI)
				{
					angleDiff -= 2 * M_PI;
				}
				if (angleDiff < -M_PI)
				{
					angleDiff += 2 * M_PI;
				}

				// Rotate the enemy towards the desired angle
				if (angleDiff > TURN_SPEED)
				{
					e->angle += TURN_SPEED;
				}
				else if (angleDiff < -TURN_SPEED)
				{
					e->angle -= TURN_SPEED;
				}
				else
				{
					e->angle = desiredAngle;
				}

				// Restrict enemies to within world boundaries
				if (e->y < 70)
				{
					e->y = 70;
				}
				if (player != NULL && player->health > 0)
				{
					if (e->x < 10)
					{
						e->x = 10;
					}
					if (e->x > WORLD_WIDTH - (e->w + 20))
					{
						e->x = WORLD_WIDTH - (e->w + 20);
					}
					if (e->y > WORLD_HEIGHT - (e->h + 20))
					{
						e->y = WORLD_HEIGHT - (e->h + 20);
					}
				}

				// Fire bullets
				if (player != NULL && player->health > 0 && --e->reload <= 0)
				{
					fireAlienBullet(e);						  // Function to fire bullets
					playSound(SND_ALIEN_FIRE, CH_ALIEN_FIRE); // Play firing sound
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
		{ // If enemy goes out of bounds they despawn
			e->health = 0;
		}

		if (e->health == 0)
		{
			if (e == player)
			{
				player = NULL;
				littleBuddy = NULL;
				Mix_Pause(CH_SHIP_DOWN);
				Mix_Pause(CH_HYPER_DRIVE);
				Mix_Pause(CH_PLAYER);
				Mix_Pause(CH_POINTS);
			}

			if (e == littleBuddy)
			{
				littleBuddy = NULL;
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

			// Handle specific actions based on whether player or NPC collided
			if (e1IsPlayer)
			{
				playerCrashed++;
				if (rand() % 30 == 0)
				{ // If the player has crashed 10 times, there's a 30% change systems go down
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
	if (player != NULL && player->health > 0)
	{
		e1->health -= 25;
		e2->health -= 25;
	}

	// Play collision sound
	Mix_Pause(CH_SHIP_HIT);
	playSound(SND_SHIP_HIT, CH_SHIP_HIT);

	// Handle entity death and effects
	if (e2->health <= 0)
	{
		e2->health = 0;
		Mix_Pause(CH_ALIEN_DIES);
		playSound(SND_ALIEN_DIE, CH_ALIEN_DIES);
		addExplosions(e2->x, e2->y, 32);
		addDebris(e2);

		if (rand() % 20 == 0)
		{
			addHealthPods(e2->x + e2->w / 2, e2->y + e2->h / 2);
		}
		if (rand() % 30 == 0)
		{
			addShotgunPods(e2->x + e2->w / 2, e2->y + e2->h / 2);
		}
		if (rand() % 40 == 0 && !littleBuddyEnabled)
		{
			addLittleBuddyPods(e2->x + e2->w / 2, e2->y + e2->h / 2);
		}
		if (rand() % 100 == 0)
		{
			addLaserPods(e2->x + e2->w / 2, e2->y + e2->h / 2);
		}
		if (rand() % 150 == 0)
		{
			addFlamePods(e2->x + e2->w / 2, e2->y + e2->h / 2);
		}
		if (rand() % 200 == 0)
		{
			addSpaceBeamPods(e2->x + e2->w / 2, e2->y + e2->h / 2);
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
		else if ((laserEnabled == 1) && (bulletHitFighter(b)) && (b->side == SIDE_ALIEN))
		{
			if (b == stage.bulletTail)
			{
				stage.bulletTail = prev;
			}

			prev->next = b->next;
			free(b);
			b = prev;
		}
		else
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
			if (b == stage.bulletTail)
			{
				stage.bulletTail = prev;
			}

			prev->next = b->next;
			free(b);
			b = prev;
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

static void doBeams(void)
{
	Beam *b, *prev;

	prev = &stage.beamHead;

	for (b = stage.beamHead.next; b != NULL; b = b->next)
	{
		// Update beam position based on velocity
		if (stage.gamePaused == false)
		{
			b->x += b->dx;
			b->y += b->dy;
		}

		if ((flameEnabled == 1) && (b->x - stage.cameraX > SCREEN_WIDTH || b->y - stage.cameraY > SCREEN_HEIGHT))
		{

			if (b == stage.beamTail)
			{
				stage.beamTail = prev;
			}

			prev->next = b->next;
			free(b);
			b = prev;
		}
		else if ((flameEnabled == 1) && (beamHitFighter(b)))
		{
			// The beam goes through the enemy and does damage
		}

		if ((spaceBeamEnabled == 1) && (b->x - stage.cameraX > SCREEN_WIDTH || b->y - stage.cameraY > SCREEN_HEIGHT))
		{

			if (b == stage.beamTail)
			{
				stage.beamTail = prev;
			}

			prev->next = b->next;
			free(b);
			b = prev;
		}
		else if ((spaceBeamEnabled == 1) && (beamHitFighter(b)))
		{
			// The beam goes through the enemy and does damage
		}

		prev = b;
	}
}

static int beamHitFighter(Beam *b)
{
	Entity *e;

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		if (e->side != b->side && collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h))
		{
			if (flameEnabled == 1)
			{
				b->health = 1;
				if ((e != player) && (e != littleBuddy) && (stage.gamePaused == false))
				{
					// Apply flame damage, but ensure it doesn't go below zero multiple times
					if (e->health > 0)
					{
						flameDamage++;
						if (flameDamage > 20)
						{
							flameDamage = 0;
							e->health -= .000000000001;
						}
						// Apply appropriate flame damage
						if (e->health <= 0)
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
								if (rand() % 20 == 0)
								{
									addHealthPods(e->x + e->w / 2, e->y + e->h / 2);
								}
								if (rand() % 30 == 0)
								{
									addShotgunPods(e->x + e->w / 2, e->y + e->h / 2);
								}
								if (rand() % 40 == 0 && !littleBuddyEnabled)
								{
									addLittleBuddyPods(e->x + e->w / 2, e->y + e->h / 2);
								}
								if (rand() % 100 == 0)
								{
									addLaserPods(e->x + e->w / 2, e->y + e->h / 2);
								}
								if (rand() % 150 == 0)
								{
									addFlamePods(e->x + e->w / 2, e->y + e->h / 2);
								}
								if (rand() % 200 == 0)
								{
									addSpaceBeamPods(e->x + e->w / 2, e->y + e->h / 2);
								}
								stage.score++;
								Mix_Pause(CH_ALIEN_DIES);
								playSound(SND_ALIEN_DIE, CH_ALIEN_DIES);
							}
						}
					}
				}
			}
			else if (spaceBeamEnabled == 1)
			{
				b->health = 1;
				if (e != player && stage.gamePaused == false)
				{
					// Apply beam damage, but ensure it doesn't go below zero multiple times
					if (e->health > 0)
					{
						e->health -= 250;

						// Apply appropriate beam damage
						if (e->health <= 0)
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
								if (rand() % 10 == 0)
								{
									addHealthPods(e->x + e->w / 2, e->y + e->h / 2);
								}
								if (rand() % 20 == 0)
								{
									addShotgunPods(e->x + e->w / 2, e->y + e->h / 2);
								}
								if (rand() % 30 == 0 && !littleBuddyEnabled)
								{
									addLittleBuddyPods(e->x + e->w / 2, e->y + e->h / 2);
								}
								if (rand() % 75 == 0)
								{
									addLaserPods(e->x + e->w / 2, e->y + e->h / 2);
								}
								if (rand() % 100 == 0)
								{
									addFlamePods(e->x + e->w / 2, e->y + e->h / 2);
								}
								if (rand() % 150 == 0)
								{
									addSpaceBeamPods(e->x + e->w / 2, e->y + e->h / 2);
								}
								stage.score++;
								Mix_Pause(CH_ALIEN_DIES);
								playSound(SND_ALIEN_DIE, CH_ALIEN_DIES);
							}
						}
					}
				}
			}
			else if (flameEnabled != 1)
			{
				b->health = 0;
			}

			else if (spaceBeamEnabled != 1)
			{
				b->health = 0;
			}

			if ((e->health > 5) && (flameEnabled != 1) && (spaceBeamEnabled != 1) && (stage.gamePaused == false))
			{
				e->health -= 5;
			}

			return 1;
		}
	}

	return 0;
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
				if (e != player && e != littleBuddy)
				{
					if (stage.gamePaused == false)
					{
						e->health = 0;
					}
				}
				else if ((e == player) && (stage.gamePaused == false))
				{
					e->health -= 5;
				}
			}
			else if (laserEnabled != 1)
			{
				b->health = 0;
			}

			if (flameEnabled == 1)
			{
				b->health = 0;
				if (e != player)
				{
					if (stage.gamePaused == false)
					{
						e->health -= .00000001;
					}
				}
				else if ((e == player) && (stage.gamePaused == false))
				{
					e->health -= 5;
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
				Mix_Pause(CH_PLAYER_DIES);
				playSound(SND_PLAYER_DIE, CH_PLAYER_DIES);
				e->health = 0;
				addExplosions(e->x, e->y, 32);
				addDebris(e);

				if (e != player)
				{
					if (rand() % 10 == 0)
					{
						addHealthPods(e->x + e->w / 2, e->y + e->h / 2);
					}
					if (rand() % 20 == 0)
					{
						addShotgunPods(e->x + e->w / 2, e->y + e->h / 2);
					}
					if (rand() % 30 == 0 && !littleBuddyEnabled)
					{
						addLittleBuddyPods(e->x + e->w / 2, e->y + e->h / 2);
					}
					if (rand() % 75 == 0)
					{
						addLaserPods(e->x + e->w / 2, e->y + e->h / 2);
					}
					if (rand() % 100 == 0)
					{
						addFlamePods(e->x + e->w / 2, e->y + e->h / 2);
					}
					if (rand() % 150 == 0)
					{
						addSpaceBeamPods(e->x + e->w / 2, e->y + e->h / 2);
					}
					stage.score++;
					Mix_Pause(CH_ALIEN_DIES);
					playSound(SND_ALIEN_DIE, CH_ALIEN_DIES);
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

static void clipCrosshair(void)
{
	if (crossHair != NULL)
	{
		if (crossHair->x < 0)
		{
			crossHair->x = 0;
		}
		if (crossHair->y < 0)
		{
			crossHair->y = 0;
		}
		if (crossHair->x > WORLD_WIDTH - crossHair->w)
		{
			crossHair->x = WORLD_WIDTH - crossHair->w;
		}
		if (crossHair->y > WORLD_HEIGHT - crossHair->h)
		{
			crossHair->y = WORLD_HEIGHT - crossHair->h;
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

			// Collision logic that enables shotgun ability
			if (player != NULL && collision(e->x, e->y, e->w, e->h, player->x, player->y, player->w, player->h))
			{
				e->health = 0;

				shotgunEnabled = 1;
				laserEnabled = 0;
				flameEnabled = 0;
				spaceBeamEnabled = 0;
				shotgunStartTime = SDL_GetTicks();
				totalPauseDuration = 0; // Reset total pause duration when ability is enabled
				laserDuration = 0;
				spaceBeamDuration = 0;
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

		// Check if laser ability should be disabled due to time limit
		if (laserEnabled && (currentTime - laserStartTime - totalPauseDuration >= laserDuration * 1000)) // Convert seconds to milliseconds
		{
			laserEnabled = 0;
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

			// Collision logic that enables laser ability
			if (player != NULL && collision(e->x, e->y, e->w, e->h, player->x, player->y, player->w, player->h))
			{
				e->health = 0;

				laserEnabled = 1;
				shotgunEnabled = 0;
				flameEnabled = 0;
				spaceBeamEnabled = 0;
				laserStartTime = SDL_GetTicks();
				totalPauseDuration = 0; // Reset total pause duration when ability is enabled
				shotgunDuration = 0;
				flameDuration = 0;
				spaceBeamDuration = 0;
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

		// Check if flame ability should be disabled due to time limit
		if (flameEnabled && (currentTime - flameStartTime - totalPauseDuration >= flameDuration * 1000)) // Convert seconds to milliseconds
		{
			flameEnabled = 0;
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

			// Collision logic that enables flame ability
			if (player != NULL && collision(e->x, e->y, e->w, e->h, player->x, player->y, player->w, player->h))
			{
				e->health = 0;

				flameEnabled = 1;
				shotgunEnabled = 0;
				laserEnabled = 0;
				spaceBeamEnabled = 0;
				flameStartTime = SDL_GetTicks();
				totalPauseDuration = 0; // Reset total pause duration when ability is enabled
				shotgunDuration = 0;
				laserDuration = 0;
				spaceBeamDuration = 0;
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

static void doSpaceBeamPods(void)
{
	Entity *e, *prev;
	Uint32 currentTime;

	if (stage.gamePaused == false)
	{
		// Get current time in milliseconds
		currentTime = SDL_GetTicks();

		// Check if space beam ability should be disabled due to time limit
		if (spaceBeamEnabled && (currentTime - spaceBeamStartTime - totalPauseDuration >= spaceBeamDuration * 1000)) // Convert seconds to milliseconds
		{
			spaceBeamEnabled = 0;
		}

		prev = &stage.spaceBeamPodHead;

		for (e = stage.spaceBeamPodHead.next; e != NULL; e = e->next)
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

			// Collision logic that enables space beam ability
			if (player != NULL && collision(e->x, e->y, e->w, e->h, player->x, player->y, player->w, player->h))
			{
				e->health = 0;

				spaceBeamEnabled = 1;
				flameEnabled = 0;
				shotgunEnabled = 0;
				laserEnabled = 0;
				spaceBeamStartTime = SDL_GetTicks();
				totalPauseDuration = 0; // Reset total pause duration when ability is enabled
				shotgunDuration = 0;
				laserDuration = 0;
				flameDuration = 0;
				spaceBeamDuration = 10;

				playSound(SND_POINTS, CH_POINTS);
			}

			// Check entity health and remove if necessary
			if (--e->health <= 0)
			{
				if (e == stage.spaceBeamPodTail)
				{
					stage.spaceBeamPodTail = prev;
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

static void doLittleBuddyPods(void)
{
	Entity *e, *prev;

	if (stage.gamePaused == false)
	{
		// Check if littleBuddy ability should be disabled due to him dying
		if (littleBuddy != NULL && littleBuddy->health <= 0)
		{
			littleBuddyEnabled = 0;
		}

		prev = &stage.littleBuddyPodHead;

		for (e = stage.littleBuddyPodHead.next; e != NULL; e = e->next)
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

			// Collision logic that enables SpaceBeam ability
			if (player != NULL && collision(e->x, e->y, e->w, e->h, player->x, player->y, player->w, player->h))
			{
				e->health = 0;

				littleBuddyEnabled = 1;
				totalPauseDuration = 0; // Reset total pause duration when ability is enabled
				if (littleBuddy == NULL)
				{
					initLittleBuddy();
					initLittleBuddyCrossHair();
				}
				playSound(SND_POINTS, CH_POINTS);
			}

			// Check entity health and remove if necessary
			if (--e->health <= 0)
			{
				if (e == stage.littleBuddyPodTail)
				{
					stage.littleBuddyPodTail = prev;
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

static void addSpaceBeamPods(int x, int y)
{
	Entity *e;

	e = malloc(sizeof(Entity));
	memset(e, 0, sizeof(Entity));
	stage.spaceBeamPodTail->next = e;
	stage.spaceBeamPodTail = e;

	e->x = x;
	e->y = y;
	e->dx = -(rand() % 5);
	e->dy = (rand() % 5) - (rand() % 5);
	e->health = FPS * 10;
	e->texture = spaceBeamPodTexture;

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

static void addLittleBuddyPods(int x, int y)
{
	Entity *e;

	e = malloc(sizeof(Entity));
	memset(e, 0, sizeof(Entity));
	stage.littleBuddyPodTail->next = e;
	stage.littleBuddyPodTail = e;

	e->x = x;
	e->y = y;
	e->dx = -(rand() % 5);
	e->dy = (rand() % 5) - (rand() % 5);
	e->health = FPS * 10;
	e->texture = littleBuddyPodTexture;

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
	drawSpaceBeamPods();
	drawHealthPods();
	drawLittleBuddyPods();

	drawFighters();

	drawDebris();

	drawExplosions();

	drawBullets();
	drawBeams();

	drawHud();

	if (stage.gamePaused)
	{
		drawCrosshair();
	}

	if (bossMessageDuration > 0)
	{
		drawEnemyBossMessage(SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 2) - 70, 255, 0, 0, "WHO DARES DESTROY MY ARMY");
	}
	else if (secondBossMessageDuration > 0)
	{
		drawEnemyBossMessage(SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 2) - 70, 255, 0, 0, "LET ME SEE YOU");
	}

	if (stage.currentEnemyCount > 500 && !bossMessageShown)
	{
		bossMessageDuration = 300; // Display for 300 frames
		bossMessageShown = true;
		initiateScreenShake(300, 5); // Shake for 30 frames with magnitude 5
	}
	if (bossMessageShown && bossMessageDuration == 1)
	{
		initiateScreenShake(300, 5);	 // Shake for 30 frames with magnitude 5
		secondBossMessageDuration = 300; // Display for 300 frames
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

static void drawSpaceBeamPods(void)
{
	Entity *e;

	for (e = stage.spaceBeamPodHead.next; e != NULL; e = e->next)
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

static void drawLittleBuddyPods(void)
{
	Entity *e;

	for (e = stage.littleBuddyPodHead.next; e != NULL; e = e->next)
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
			blitRotated(e->texture, e->x - stage.cameraX, e->y - stage.cameraY, e->angle); // Draw aliens with rotation
		}
	}

	if (player != NULL)
	{
		// Calculate rotation angle in degrees (SDL requires degrees)
		float rotationAngle = aim_angle * (180.0f / M_PI);

		// Center of rotation
		SDL_Point rotationCenter = {player->w / 2, player->h / 2};

		// Render the player's texture with rotation
		SDL_Rect playerRect = {player->x - stage.cameraX, player->y - stage.cameraY, player->w, player->h};

		// Draw boost effect if boost is active
		if (player->boostActive)
		{
			autoAim = false;
			if (!hyperDriveSoundPlayed)
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
		{
			// Draw the player normally by default
			SDL_RenderCopyEx(app.renderer, playerTexture, NULL, &playerRect, rotationAngle, &rotationCenter, SDL_FLIP_NONE);
		}

		// Update crosshair position based on input device
		if ((app.controller) && (!stage.gamePaused) && (!autoAim))
		{
			// Handle controller input for crosshair
			int aimX = SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_RIGHTX);
			int aimY = SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_RIGHTY);

			if (aimX * aimX + aimY * aimY > 8000 * 8000) // Check if the player is aiming
			{
				if (crosshairDistance < MAX_CROSSHAIR_DISTANCE)
				{
					crosshairDistance += CROSSHAIR_STEP * 20; // Increase the distance quickly when aiming
				}
			}
			else
			{
				if (crosshairDistance > BASE_CROSSHAIR_DISTANCE)
				{
					crosshairDistance -= CROSSHAIR_STEP; // Decrease the distance gradually when not aiming
				}
			}

			// Calculate crosshair position relative to player and aim direction
			crossHair->x = (player->x + crosshairDistance * cos(aim_angle) + player->w / 2 - crossHair->w / 2);
			crossHair->y = (player->y + crosshairDistance * sin(aim_angle) + player->h / 2 - crossHair->h / 2);

			// Render the crosshair texture at the updated position
			SDL_Rect crossHairRect = {crossHair->x - stage.cameraX, crossHair->y - stage.cameraY, crossHair->w, crossHair->h};
			SDL_RenderCopy(app.renderer, crossHairTexture, NULL, &crossHairRect);
		}
		else if ((app.controller) && (!stage.gamePaused) && (autoAim))
		{
			Entity *closestEnemy = NULL;
			float closestDistance = FLT_MAX;
			float playerAngle = aim_angle; // Capture the player's current aiming angle

			// Find the closest enemy to the player's aim direction
			for (Entity *e = stage.fighterHead.next; e != NULL; e = e->next)
			{
				if (e != player && e != littleBuddy)
				{
					// Calculate vector from player to enemy
					float dx = e->x + e->w / 2 - (player->x + player->w / 2);
					float dy = e->y + e->h / 2 - (player->y + player->h / 2);
					float distance = sqrt(dx * dx + dy * dy);

					// Calculate angle from player to enemy
					float enemyAngle = atan2(dy, dx);

					// Adjust enemy angle to be within [0, 2*PI)
					if (enemyAngle < 0)
					{
						enemyAngle += 2 * M_PI;
					}

					// Calculate angular difference between player's aim direction and enemy
					float angleDiff = fabs(enemyAngle - playerAngle);

					// Normalize angle difference to be within [0, PI)
					if (angleDiff > M_PI)
					{
						angleDiff = 2 * M_PI - angleDiff;
					}

					// Check if this enemy is within the acceptable angular range
					// Adjust the angular range as needed (here, approximately 45 degrees)
					if (angleDiff < M_PI / 4)
					{
						// Check if this enemy is the closest so far
						if (distance < closestDistance)
						{
							closestEnemy = e;
							closestDistance = distance;
						}
					}
				}
			}

			// Adjust the crosshair to target the closest eligible enemy if one is found
			if (closestEnemy != NULL)
			{
				crossHair->x = closestEnemy->x + closestEnemy->w / 2 - crossHair->w / 2;
				crossHair->y = closestEnemy->y + closestEnemy->h / 2 - crossHair->h / 2;
			}
			else
			{
				// If no eligible enemy is found, default to player's aim direction
				crossHair->x = player->x + (int)(100 * cos(playerAngle)) - crossHair->w / 2;
				crossHair->y = player->y + (int)(100 * sin(playerAngle)) - crossHair->h / 2;
			}

			// Render the crosshair texture at the updated position
			SDL_Rect crossHairRect = {crossHair->x - stage.cameraX, crossHair->y - stage.cameraY, crossHair->w, crossHair->h};
			SDL_RenderCopy(app.renderer, crossHairTexture, NULL, &crossHairRect);
		}
		else if (!stage.gamePaused)
		{
			// Handle mouse input for crosshair
			int mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);

			// Calculate crosshair position relative to mouse position
			crossHair->x = mouseX - stage.cameraX - crossHair->w / 2;
			crossHair->y = mouseY - stage.cameraY - crossHair->h / 2;
			// Render the crosshair texture at the updated position
			SDL_Rect crossHairRect = {crossHair->x, crossHair->y, crossHair->w, crossHair->h};
			SDL_RenderCopy(app.renderer, crossHairTexture, NULL, &crossHairRect);
		}

		if (littleBuddy != NULL && littleBuddyEnabled == 1)
		{ // Draw littleBuddy and his crosshair
			if (!autoAim)
			{
				Entity *closestEnemy = NULL;
				float closestDistance = FLT_MAX;
				float playerAngle = aim_angle; // Capture the player's current aiming angle

				// Find the closest enemy to the player's aim direction
				for (Entity *e = stage.fighterHead.next; e != NULL; e = e->next)
				{
					if (e != player && e != littleBuddy)
					{
						// Calculate vector from player to enemy
						float dx = e->x + e->w / 2 - (player->x + player->w / 2);
						float dy = e->y + e->h / 2 - (player->y + player->h / 2);
						float distance = sqrt(dx * dx + dy * dy);

						// Calculate angle from player to enemy
						float enemyAngle = atan2(dy, dx);

						// Adjust enemy angle to be within [0, 2*PI)
						if (enemyAngle < 0)
						{
							enemyAngle += 2 * M_PI;
						}

						// Calculate angular difference between player's aim direction and enemy
						float angleDiff = fabs(enemyAngle - playerAngle);

						// Normalize angle difference to be within [0, PI)
						if (angleDiff > M_PI)
						{
							angleDiff = 2 * M_PI - angleDiff;
						}

						// Check if this enemy is within the acceptable angular range
						// Adjust the angular range as needed (here, approximately 45 degrees)
						if (angleDiff < M_PI / 4)
						{
							// Check if this enemy is the closest so far
							if (distance < closestDistance)
							{
								closestEnemy = e;
								closestDistance = distance;
							}
						}
					}
				}

				// Adjust the crosshair to target the closest eligible enemy if one is found
				if (closestEnemy != NULL)
				{
					littleBuddyCrosshair->x = closestEnemy->x + closestEnemy->w / 2 - littleBuddyCrosshair->w / 2;
					littleBuddyCrosshair->y = closestEnemy->y + closestEnemy->h / 2 - littleBuddyCrosshair->h / 2;
				}
				else
				{
					// If no eligible enemy is found, default to player's aim direction
					littleBuddyCrosshair->x = littleBuddy->x + (int)(100 * cos(playerAngle)) - littleBuddyCrosshair->w / 2;
					littleBuddyCrosshair->y = littleBuddy->y + (int)(100 * sin(playerAngle)) - littleBuddyCrosshair->h / 2;
				}

				// Render the crosshair texture at the updated position
				SDL_Rect littleBuddyCrosshairRect = {littleBuddyCrosshair->x - stage.cameraX, littleBuddyCrosshair->y - stage.cameraY, littleBuddyCrosshair->w, littleBuddyCrosshair->h};
				SDL_RenderCopy(app.renderer, littleBuddyCrosshairTexture, NULL, &littleBuddyCrosshairRect);
			}

			// Center of rotation (if needed)
			SDL_Point littleBuddyRotationCenter = {littleBuddy->w / 2, littleBuddy->h / 2};

			// Render the little buddy's texture with rotation
			SDL_Rect littleBuddyRect = {littleBuddy->x - stage.cameraX, littleBuddy->y - stage.cameraY, littleBuddy->w, littleBuddy->h};

			SDL_RenderCopyEx(app.renderer, littleBuddy->texture, NULL, &littleBuddyRect, littleBuddy->angle, &littleBuddyRotationCenter, SDL_FLIP_NONE);
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

		// Center of rotation
		SDL_Point rotationCenter = {b->w / 2, b->h / 2};

		// Render the bullet's texture with rotation
		SDL_Rect bulletRect = {(int)b->x - stage.cameraX, (int)b->y - stage.cameraY, b->w, b->h};
		SDL_RenderCopyEx(app.renderer, b->texture, NULL, &bulletRect, rotationAngle, &rotationCenter, SDL_FLIP_NONE);
	}
}

static void drawBeams(void)
{
	Beam *b;

	for (b = stage.beamHead.next; b != NULL; b = b->next)
	{
		// Calculate rotation angle in degrees (SDL requires degrees)
		float rotationAngle = b->angle * (180.0f / M_PI);

		// Center of rotation
		SDL_Point rotationCenter = {b->w / 2, b->h / 2};

		// Render the beam's texture with rotation
		SDL_Rect beamRect = {(int)b->x - stage.cameraX, (int)b->y - stage.cameraY, b->w, b->h};
		SDL_RenderCopyEx(app.renderer, b->beamTexture, NULL, &beamRect, rotationAngle, &rotationCenter, SDL_FLIP_NONE);
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
	if (stage.gamePaused)
	{
		drawPauseMenu();
	}
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
			if (!stage.gamePaused && enemy != player) // Only draw the health bar for enemies
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

			// Draw the timer bar for laser ability
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

			// Draw the timer bar for flame ability
			int timerBarWidth = (int)(ABILITY_BAR_WIDTH * remainingTimePercentage);
			SDL_Rect timerBarRect = {ABILITY_BAR_X, ABILITY_BAR_Y, timerBarWidth, ABILITY_BAR_HEIGHT};
			SDL_SetRenderDrawColor(app.renderer, 0, 0, 255, 255); // Blue color for timer bar
			SDL_RenderFillRect(app.renderer, &timerBarRect);
		}
		else if (spaceBeamEnabled == 1)
		{
			Uint32 currentTime;
			Uint32 elapsedSpaceBeamTime;
			float remainingTimePercentage;

			// Get current time in milliseconds
			currentTime = SDL_GetTicks();

			if (!stage.gamePaused)
			{
				// Calculate elapsed time only when game is not paused
				elapsedSpaceBeamTime = currentTime - spaceBeamStartTime - totalPauseDuration;
			}
			else
			{
				// When the game is paused, use the last calculated elapsed time
				elapsedSpaceBeamTime = pauseStartTime - spaceBeamStartTime - totalPauseDuration;
			}

			remainingTimePercentage = 1.0f - (float)elapsedSpaceBeamTime / (spaceBeamDuration * 1000);

			// Clamp the percentage to [0, 1] range
			if (remainingTimePercentage < 0)
				remainingTimePercentage = 0;
			else if (remainingTimePercentage > 1)
				remainingTimePercentage = 1;

			// Draw the timer bar for space beam ability
			int timerBarWidth = (int)(ABILITY_BAR_WIDTH * remainingTimePercentage);
			SDL_Rect timerBarRect = {ABILITY_BAR_X, ABILITY_BAR_Y, timerBarWidth, ABILITY_BAR_HEIGHT};
			SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255); // White color for timer bar
			SDL_RenderFillRect(app.renderer, &timerBarRect);
		}

		else if (littleBuddyEnabled == 1)
		{
			// Draw little buddy health bar
		}

		// Calculate boost availability percentage
		float boostPercentage = (float)player->boostTimer / 120.0f; // The boost lasts 2 seconds (120 frames)

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
			{ // Charging effect for systems coming back online
				cooldownPercentage = 1.0f - ((float)player->boostCooldown / 300.0f);
			} // Cooldown is 5 seconds (300 frames)
			int cooldownBarWidth = (int)(BOOST_BAR_WIDTH * cooldownPercentage);
			SDL_Rect cooldownBarRect = {BOOST_BAR_X, BOOST_BAR_Y, cooldownBarWidth, BOOST_BAR_HEIGHT};
			SDL_SetRenderDrawColor(app.renderer, 128, 128, 128, 255); // Light grey color for cooldown bar
			SDL_RenderFillRect(app.renderer, &cooldownBarRect);
		}
	}
}

static void drawPauseMenu(void)
{
	// Draw a semi-transparent background overlay
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 128); // Semi-transparent black
	SDL_Rect overlayRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
	SDL_RenderFillRect(app.renderer, &overlayRect);

	// Draw a box for the pause menu
	int menuWidth = 500;
	int menuHeight = 600;
	int menuX = (SCREEN_WIDTH - menuWidth) / 2;
	int menuY = (SCREEN_HEIGHT - menuHeight) / 2;
	SDL_Rect menuRect = {menuX, menuY, menuWidth, menuHeight};
	SDL_SetRenderDrawColor(app.renderer, 40, 40, 40, 100); // Dark grey box
	SDL_RenderFillRect(app.renderer, &menuRect);

	// Draw text for the pause menu options
	drawText(menuX + menuWidth / 2, menuY + 20, 255, 255, 255, TEXT_CENTER, "PAUSED");

	// Check if crosshair is over QUIT option
	int quitOptionX = menuX + menuWidth / 2;
	int quitOptionY = menuY + 120;
	int quitOptionWidth = 200;
	int quitOptionHeight = 30;
	SDL_Rect quitOptionRect = {quitOptionX - quitOptionWidth / 2, quitOptionY - quitOptionHeight / 2, quitOptionWidth, quitOptionHeight};
	SDL_Rect crossHairRect = {crossHair->x - stage.cameraX, crossHair->y - stage.cameraY, crossHair->w, crossHair->h};

	// Check if crosshair intersects with QUIT option
	bool isHoveringQuit = SDL_HasIntersection(&crossHairRect, &quitOptionRect);

	// Render QUIT option text with hover effect if hovering
	drawText(menuX + menuWidth / 2, quitOptionY, isHoveringQuit ? 255 : 255, isHoveringQuit ? 0 : 255, isHoveringQuit ? 0 : 255, TEXT_CENTER, "SELECT TO QUIT");

	// Check if crosshair is over RESTART option
	int restartOptionX = menuX + menuWidth / 2;
	int restartOptionY = menuY + 220;
	int restartOptionWidth = 200;
	int restartOptionHeight = 30;
	SDL_Rect restartOptionRect = {restartOptionX - restartOptionWidth / 2, restartOptionY - restartOptionHeight / 2, restartOptionWidth, restartOptionHeight};

	// Check if crosshair intersects with RESTART option
	bool isHoveringRestart = SDL_HasIntersection(&crossHairRect, &restartOptionRect);

	// Render RESTART option text with hover effect if hovering
	drawText(menuX + menuWidth / 2, restartOptionY, isHoveringRestart ? 255 : 255, isHoveringRestart ? 0 : 255, isHoveringRestart ? 0 : 255, TEXT_CENTER, "SELECT TO RESTART");

	// Check if crosshair is over AUTO AIM option
	int autoAimOptionX = menuX + menuWidth / 2;
	int autoAimOptionY = menuY + 320;
	int autoAimOptionWidth = 200;
	int autoAimOptionHeight = 30;
	SDL_Rect autoAimOptionRect = {autoAimOptionX - autoAimOptionWidth / 2, autoAimOptionY - autoAimOptionHeight / 2, autoAimOptionWidth, autoAimOptionHeight};

	// Check if crosshair intersects with AUTO AIM option
	bool isHoveringAutoAim = SDL_HasIntersection(&crossHairRect, &autoAimOptionRect);

	// Render AUTO AIM option text with hover effect if hovering
	if (app.controller)
	{
		if (autoAim)
		{
			drawText(menuX + menuWidth / 2, autoAimOptionY, 255, 0, 0, TEXT_CENTER, "DISABLE AUTO AIM");
		}
		else
		{
			drawText(menuX + menuWidth / 2, autoAimOptionY, isHoveringAutoAim ? 0 : 255, isHoveringAutoAim ? 255 : 255, isHoveringAutoAim ? 0 : 255, TEXT_CENTER, "ENABLE AUTO AIM");
		}
	}
	// Reset render blend mode to default
	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);

	// Handle controller input for quitting the game and restarting
	if (stage.gamePaused)
	{
		// Get current state of the right shoulder button
		int currentRightShoulderState = SDL_GameControllerGetButton(app.controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);

		// Check right shoulder press for quitting the game
		if (currentRightShoulderState && isHoveringQuit && prevRightShoulderState == 0)
		{
			// Play sound and quit game
			quitGame();

			// Update previous state to indicate button is pressed
			prevRightShoulderState = 1;
		}

		// Check right shoulder press for restarting the game
		if (currentRightShoulderState && isHoveringRestart && prevRightShoulderState == 0)
		{
			// Restart game
			restartGame();

			// Update previous state to indicate button is pressed
			prevRightShoulderState = 1;
		}

		// Check right shoulder press for enabling auto aim
		if (currentRightShoulderState && isHoveringAutoAim && prevRightShoulderState == 0)
		{
			// Toggle auto aim
			autoAim = !autoAim;
			playerWantsAutoAim = !playerWantsAutoAim;

			// Update previous state to indicate button is pressed
			prevRightShoulderState = 1;
		}

		// Update previous state if button is released
		if (!currentRightShoulderState)
		{
			prevRightShoulderState = 0;
		}

		// Check for left mouse button click for options
		int mouseX, mouseY;
		Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);

		// Handle left mouse button click for quitting the game
		if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT) && isHoveringQuit)
		{
			// Play sound and quit game
			quitGame();
		}

		// Handle left mouse button click for restarting the game
		if (mouseState & SDL_BUTTON(SDL_BUTTON_LEFT) && isHoveringRestart)
		{
			// Restart game
			restartGame();
		}
	}
	else
	{
		// Reset previous state when not paused
		prevRightShoulderState = 0;
	}
}

// Function to quit the game
static void quitGame()
{
	stage.gamePaused = false;
	Entity *e = player;
	e->health = 5;
	playerQuit = true;
	handleFighterCollision(player, e);
}

// Function to restart the game
static void restartGame()
{
	if (littleBuddy != NULL)
	{
		littleBuddy = NULL;
	}
	initStage();
}

static void toggleBoost(bool activate)
{
	if (activate)
	{
		if (player->boostCooldown <= 0) // Check if boost is not on cooldown
		{
			player->boostTimer = 120;	// Set boost timer to 2 seconds (120 frames)
			PLAYER_SPEED += 7;			// Increment player speed for boost
			player->boostActive = true; // Set boost active flag
		}
	}
	else
	{
		PLAYER_SPEED = 4;			 // Reset player speed to base speed
		player->boostActive = false; // Reset boost active flag
		hyperDriveSoundPlayed = false;
		if (playerWantsAutoAim)
		{
			autoAim = true;
		}
	}
}

static void toggleSystemsDown(bool activate)
{
	if (activate)
	{
		if (player->systemsCooldown <= 0) // Check if boost is not on cooldown
		{
			player->systemsTimer = 300; // Set boost timer to 5 seconds (300 frames)
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

void updateCamera()
{
	if (player != NULL)
	{
		// Center camera on player
		stage.cameraX = player->x - (SCREEN_WIDTH / 2);
		stage.cameraY = player->y - (SCREEN_HEIGHT / 2);

		// Apply screen shake offset
		updateScreenShake();

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

static void updateScreenShake()
{
	if (shakeDuration > 0)
	{
		// Calculate new shake offsets
		int shakeOffsetX = rand() % (2 * shakeMagnitude + 1) - shakeMagnitude;
		int shakeOffsetY = rand() % (2 * shakeMagnitude + 1) - shakeMagnitude;

		// Apply shake offsets to camera position
		int shakenCameraX = stage.cameraX + shakeOffsetX;
		int shakenCameraY = stage.cameraY + shakeOffsetY;

		// Clamp shaken camera position to game world boundaries
		if (shakenCameraX < 0)
		{
			shakenCameraX = 0;
		}
		if (shakenCameraY < 0)
		{
			shakenCameraY = 0;
		}
		if (shakenCameraX > WORLD_WIDTH - SCREEN_WIDTH)
		{
			shakenCameraX = WORLD_WIDTH - SCREEN_WIDTH;
		}
		if (shakenCameraY > WORLD_HEIGHT - SCREEN_HEIGHT)
		{
			shakenCameraY = WORLD_HEIGHT - SCREEN_HEIGHT;
		}

		// Update stage.cameraX and stage.cameraY with the clamped shaken values
		stage.cameraX = shakenCameraX;
		stage.cameraY = shakenCameraY;

		// Decrease shake duration
		shakeDuration--;
	}
}

static void drawCrosshair()
{
	if (player != NULL)
	{
		// Check if a controller is connected and the game is paused
		if (app.controller && stage.gamePaused)
		{
			// Update crosshair position based on controller input
			if (SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_RIGHTY) < -8000)
			{
				crossHair->y -= CROSSHAIR_SPEED; // Move crosshair up
			}

			if (SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_RIGHTY) > 8000)
			{
				crossHair->y += CROSSHAIR_SPEED; // Move crosshair down
			}

			if (SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_RIGHTX) < -8000)
			{
				crossHair->x -= CROSSHAIR_SPEED; // Move crosshair left
			}

			if (SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_RIGHTX) > 8000)
			{
				crossHair->x += CROSSHAIR_SPEED; // Move crosshair right
			}
		}
		else
		{
			// Update crosshair position based on mouse position
			int mouseX, mouseY;
			SDL_GetMouseState(&mouseX, &mouseY);

			crossHair->x = mouseX + stage.cameraX;
			crossHair->y = mouseY + stage.cameraY;
		}

		// Render the crosshair texture at the updated position
		SDL_Rect crossHairRect = {crossHair->x - stage.cameraX, crossHair->y - stage.cameraY, crossHair->w, crossHair->h};
		SDL_RenderCopy(app.renderer, crossHairTexture, NULL, &crossHairRect);
	}
}

static void drawEnemyBossMessage(int x, int y, int r, int g, int b, char *text)
{
	// Draw the message only if bossMessageDuration is greater than 0
	if (bossMessageDuration > 0)
	{
		drawText(x, y, r, g, b, TEXT_CENTER, text);
		bossMessageDuration--; // Decrement the duration each frame
		if (bossMessageShown == true)
		{
			secondBossMessageDuration--;
		}
		if (player != NULL)
		{
			player->x = WORLD_WIDTH / 2;
			player->y = WORLD_HEIGHT / 2;
		}
	}
	else if (secondBossMessageDuration > 0)
	{
		drawText(x, y, r, g, b, TEXT_CENTER, text);
		bossMessageDuration--; // Decrement the duration each frame
		if (bossMessageShown == true)
		{
			secondBossMessageDuration--;
		}
		if (player != NULL)
		{
			player->x = WORLD_WIDTH / 2;
			player->y = WORLD_HEIGHT / 2;
		}
	}
}

static void initiateScreenShake(int duration, int magnitude)
{
	shakeDuration = duration;
	shakeMagnitude = magnitude;
}