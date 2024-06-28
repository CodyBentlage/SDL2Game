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

static void doHealthPods(void);
static void addHealthPods(int x, int y);
static void drawHealthPods(void);

static void toggleBoost(bool activate);

static Entity *player;
static SDL_Texture *bulletTexture;
static SDL_Texture *enemyTexture;
static SDL_Texture *alienBulletTexture;
static SDL_Texture *playerTexture;
static SDL_Texture *explosionTexture;
static SDL_Texture *pointsTexture;
static SDL_Texture *healthTexture;
static SDL_Texture *currentTexture;
static SDL_Texture *playerBoostTexture1;
static SDL_Texture *playerBoostTexture2;
static SDL_Texture *playerBoostTexture3;
static SDL_Texture *playerBoostTexture4;
static int enemySpawnTimer;
static int stageResetTimer;
static int shotgunEnabled = 0;
static int shotgunDuration = 0;
static Uint32 shotgunStartTime = 0;
static float PLAYER_SPEED = 4;

static float aim_angle = 0.0f;

bool isBoostActive = false;
bool hyperDriveSoundPlayed = false;

bool shipCollision = false;
static Uint32 collisionCooldownEndTime = 0;

static int flameGifFrame = 0;

void initStage(void)
{
	app.delegate.logic = logic;
	app.delegate.draw = draw;

	bulletTexture = loadTexture("gfx/playerBullet.png");
	enemyTexture = loadTexture("gfx/enemy.png");
	alienBulletTexture = loadTexture("gfx/alienBullet.png");
	playerTexture = loadTexture("gfx/player.png");
	explosionTexture = loadTexture("gfx/explosion.png");
	pointsTexture = loadTexture("gfx/points.png");
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

	PLAYER_SPEED = 4;
}

static void resetStage(void)
{
	Entity *e;
	Explosion *ex;
	Debris *d;

	shotgunEnabled = 0;
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

	doHealthPods();

	spawnEnemyFighters();
	spawnEnemyMosquitos();
	spawnEnemyTheCube();

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
	if (shipCollision && SDL_GetTicks() >= collisionCooldownEndTime)
	{
		// Reset shipCollision to false
		shipCollision = false;
	}

	if (player != NULL)
	{
		if (shipCollision == false)
		{
			player->dx = player->dy = 0;
		}

		// Update player reload
		if (player->reload > 0)
		{
			player->reload--;
		}

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

		if (app.controller)
		{
			// Handle game controller input
			if (shipCollision == false)
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

			// Calculate aiming direction
			int aim_x = SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_RIGHTX);
			int aim_y = SDL_GameControllerGetAxis(app.controller, SDL_CONTROLLER_AXIS_RIGHTY);
			float aim_threshold = 8000.0f;

			if (aim_x * aim_x + aim_y * aim_y > aim_threshold * aim_threshold)
			{
				aim_angle = atan2f((float)aim_y, (float)aim_x);
			}
			if (SDL_GameControllerGetButton(app.controller, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER) && player->reload <= 0)
			{
				playSound(SND_PLAYER_FIRE, CH_PLAYER);

				if (shotgunEnabled == 1)
				{
					fireShotgun(aim_angle);
				}
				else
				{
					fireBullet(aim_angle);
				}
			}
			if (SDL_GameControllerGetButton(app.controller, SDL_CONTROLLER_BUTTON_LEFTSHOULDER))
			{
				if (player->boostTimer <= 0 && player->boostCooldown <= 0)
				{
					playSound(SND_PLAYER_FIRE, CH_PLAYER);
					toggleBoost(true); // Activate boost
				}
			}
		}
		else
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

			// Track mouse position
			float aim_x = (float)(app.mouse_x - player->x);
			float aim_y = (float)(app.mouse_y - player->y);

			// Calculate aim_angle
			aim_angle = atan2f(aim_y, aim_x);

			if (app.mouse_buttons[SDL_BUTTON_LEFT] && player->reload <= 0)
			{
				playSound(SND_PLAYER_FIRE, CH_PLAYER);

				if (shotgunEnabled == 1)
				{
					fireShotgun(aim_angle);
				}
				else
				{
					fireBullet(aim_angle);
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

static void doEnemies(void)
{
	Entity *e;
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

	// Calculate angle in radians and adjust it by π to flip the direction
	e->angle = atan2(bullet->dy, bullet->dx) + M_PI;

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
		e->x += e->dx;
		e->y += e->dy;

		if ((e != player && e->x - (stage.cameraX - 500) < -e->w) || (e != player && e->y - (stage.cameraY - 500) < -e->h))
		{
			e->health = 0;
		}

		if (e->health == 0)
		{
			if (e == player)
			{
				player = NULL;
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
			if (rand() % 40 == 0)
			{
				playSound(SND_SHIP_DOWN, CH_SHIP_DOWN);
				collisionCooldownEndTime = SDL_GetTicks() + COLLISION_COOLDOWN_MS;
				shipCollision = true; // Player collision
				player->boostCooldown = 300;
			}
		}

		return true; // Collision detected
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
		stage.currentEnemyCount--;
		playSound(SND_ALIEN_DIE, CH_ANY);
		addExplosions(e2->x, e2->y, 32);
		addDebris(e2);
		if (rand() % 20 == 0)
		{
			addShotgunPods(e2->x + e2->w / 2, e2->y + e2->h / 2);
		}
		if (rand() % 20 == 0)
		{
			addHealthPods(e2->x + e2->w / 2, e2->y + e2->h / 2);
		}
	}
	if (e1->health <= 0)
	{
		e1->health = 0;
		playSound(SND_PLAYER_DIE, CH_ANY);
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
		b->x += b->dx;
		b->y += b->dy;

		// Check if the bullet hits an entity or goes out of bounds
		if (bulletHitFighter(b) || b->x - stage.cameraX < -b->w || b->y - stage.cameraY < -b->h || b->x - stage.cameraX > SCREEN_WIDTH || b->y - stage.cameraY > SCREEN_HEIGHT)
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
			b->health = 0;
			if (e->health > 5)
			{
				e->health -= 5;
			}
			else
			{
				e->health = 0;
				addExplosions(e->x, e->y, 32);
				addDebris(e);
				if (e == player)
				{
					playSound(SND_PLAYER_DIE, CH_PLAYER);
				}
				else
				{
					if (rand() % 10 == 0)
					{
						addShotgunPods(e->x + e->w / 2, e->y + e->h / 2);
					}
					if (rand() % 10 == 0)
					{
						addHealthPods(e->x + e->w / 2, e->y + e->h / 2);
					}
					stage.score++;
					playSound(SND_ALIEN_DIE, CH_ANY);
					stage.currentEnemyCount--;
				}
			}

			if (e == player)
			{
				playSound(SND_SHIP_HIT, CH_PLAYER);
			}
			else if (e != player)
			{
				playSound(SND_SHIP_HIT, CH_PLAYER);
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

static void doDebris(void)
{
	Debris *d, *prev;

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

static void doShotgunPods(void)
{
	Entity *e, *prev;
	Uint32 currentTime;

	// Get current time in milliseconds
	currentTime = SDL_GetTicks();

	// Check if shotgun ability should be disabled due to time limit
	if (shotgunEnabled && (currentTime - shotgunStartTime >= shotgunDuration * 1000)) // Convert seconds to milliseconds
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

		if (e->y < 0)
		{
			e->y = 0;
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
			shotgunStartTime = SDL_GetTicks();
			shotgunDuration += 10;

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

static void doHealthPods(void)
{
	Entity *e, *prev;

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

		if (e->y < 0)
		{
			e->y = 0;
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

		// Calculate remaining time percentage for shotgun ability (if needed)
		Uint32 currentTime = SDL_GetTicks();
		Uint32 elapsedTime = currentTime - shotgunStartTime;
		float remainingTimePercentage = 1.0f - (float)elapsedTime / (shotgunDuration * 1000); // Convert to seconds

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
			// Boost is active, draw yellow bar
			SDL_Rect boostBarRect = {BOOST_BAR_X, BOOST_BAR_Y, BOOST_BAR_WIDTH, BOOST_BAR_HEIGHT};
			SDL_SetRenderDrawColor(app.renderer, 255, 255, 0, 255); // Yellow color for boost bar
			SDL_RenderFillRect(app.renderer, &boostBarRect);
		}
		else if (player->boostTimer > 0)
		{
			// Boost is active, draw yellow bar
			int boostBarWidth = (int)(BOOST_BAR_WIDTH * boostPercentage);
			SDL_Rect boostBarRect = {BOOST_BAR_X, BOOST_BAR_Y, boostBarWidth, BOOST_BAR_HEIGHT};
			SDL_SetRenderDrawColor(app.renderer, 255, 255, 0, 255); // Yellow color for boost bar
			SDL_RenderFillRect(app.renderer, &boostBarRect);
		}
		else if (player->boostCooldown > 0)
		{
			// Boost is on cooldown, draw grey bar with charging effect
			float cooldownPercentage = 1.0f - ((float)player->boostCooldown / 180.0f); // Cooldown is 3 seconds (180 frames)
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