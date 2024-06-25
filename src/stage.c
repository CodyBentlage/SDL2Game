#include "common.h"
#include "background.h"
#include "draw.h"
#include "highscores.h"
#include "sound.h"
#include "stage.h"
#include "text.h"
#include "util.h"

extern App app;
extern Highscores highscores;
extern Stage stage;

static void logic(void);
static void draw(void);
static void initPlayer(void);
static void fireBullet(float angle);
static void fireShotgun(float angle);
static void doPlayer(void);
static void doFighters(void);
static void doBullets(void);
static void drawFighters(void);
static void drawBullets(void);
static void spawnEnemies(void);
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
static void drawShotgunPods(void);
static void addShotgunPod(int x, int y);

static Entity *player;
static SDL_Texture *bulletTexture;
static SDL_Texture *enemyTexture;
static SDL_Texture *alienBulletTexture;
static SDL_Texture *playerTexture;
static SDL_Texture *explosionTexture;
static SDL_Texture *pointsTexture;
static int enemySpawnTimer;
static int stageResetTimer;
static int shotgunEnabled = 0;

static float aim_angle = 0.0f;

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

	memset(app.keyboard, 0, sizeof(int) * MAX_KEYBOARD_KEYS);
	memset(app.mouse_buttons, 0, sizeof(int) * MAX_MOUSE_BUTTONS);

	resetStage();

	stage.score = 0;

	initPlayer();

	enemySpawnTimer = 0;

	stageResetTimer = FPS * 3;
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

	while (stage.shotgunHead.next)
	{
		e = stage.shotgunHead.next;
		stage.shotgunHead.next = e->next;
		free(e);
	}

	memset(&stage, 0, sizeof(Stage));
	stage.fighterTail = &stage.fighterHead;
	stage.bulletTail = &stage.bulletHead;
	stage.explosionTail = &stage.explosionHead;
	stage.debrisTail = &stage.debrisHead;
	stage.pointsTail = &stage.shotgunHead;
}

static void initPlayer()
{
	player = malloc(sizeof(Entity));
	memset(player, 0, sizeof(Entity));
	stage.fighterTail->next = player;
	stage.fighterTail = player;

	player->health = 1;
	player->x = 100;
	player->y = 100;
	player->texture = playerTexture;
	SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);

	player->side = SIDE_PLAYER;

	aim_angle = 0.0f;
}

static void logic(void)
{
	doBackground();

	doStarfield();

	doPlayer();

	doEnemies();

	doFighters();

	doBullets();

	doExplosions();

	doDebris();

	doShotgunPods();

	spawnEnemies();

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
	if (player != NULL)
	{
		player->dx = player->dy = 0;

		if (player->reload > 0)
		{
			player->reload--;
		}

		if (app.controller)
		{
			// Handle game controller input
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

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		if (e != player)
		{
			e->y = MIN(MAX(e->y, 0), SCREEN_HEIGHT - e->h);

			if (player != NULL && --e->reload <= 0)
			{
				fireAlienBullet(e);

				playSound(SND_ALIEN_FIRE, CH_ALIEN_FIRE);
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

static void doFighters(void)
{
	Entity *e, *prev;

	prev = &stage.fighterHead;

	for (e = stage.fighterHead.next; e != NULL; e = e->next)
	{
		e->x += e->dx;
		e->y += e->dy;

		if (e != player && e->x < -e->w)
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

		else if (player != NULL && e != player && collision(player->x, player->y, player->w, player->h, e->x, e->y, e->w, e->h))
        {
            // Player and enemy collided, both should die
            e->health = 0;
            player->health = 0;

            addExplosions(player->x, player->y, 32); // Create explosion at player's position

            addExplosions(e->x, e->y, 32); // Create explosion at enemy's position

            addDebris(player); // Add debris for player

            addDebris(e); // Add debris for enemy

            playSound(SND_PLAYER_DIE, CH_PLAYER); // Play player death sound
        }

		prev = e;
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
		if (bulletHitFighter(b) || b->x < -b->w || b->y < -b->h || b->x > SCREEN_WIDTH || b->y > SCREEN_HEIGHT)
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
			e->health = 0;

			addExplosions(e->x, e->y, 32);

			addDebris(e);

			if (e == player)
			{
				playSound(SND_PLAYER_DIE, CH_PLAYER);
			}
			else
			{
				if (rand() % 20 == 0)
				{
					addShotgunPod(e->x + e->w / 2, e->y + e->h / 2);
				}
				stage.score++;
				playSound(SND_ALIEN_DIE, CH_ANY);
			}

			return 1;
		}
	}

	return 0;
}

static void spawnEnemies(void)
{
	Entity *enemy;

	if (--enemySpawnTimer <= 0)
	{
		enemy = malloc(sizeof(Entity));
		memset(enemy, 0, sizeof(Entity));
		stage.fighterTail->next = enemy;
		stage.fighterTail = enemy;

		enemy->x = SCREEN_WIDTH;
		enemy->y = rand() % SCREEN_HEIGHT;
		enemy->texture = enemyTexture;
		SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

		enemy->dx = -(2 + (rand() % 4));
		enemy->dy = -100 + (rand() % 200);
		enemy->dy /= 100;

		enemy->side = SIDE_ALIEN;
		enemy->health = 1;

		enemy->reload = FPS * (1 + (rand() % 3));

		enemySpawnTimer = 30 + (rand() % FPS);
	}
}

static void clipPlayer(void)
{
	if (player != NULL)
	{
		if (player->x < 0)
		{
			player->x = 0;
		}

		if (player->y < 0)
		{
			player->y = 0;
		}

		if (player->x > SCREEN_WIDTH - player->w)
		{
			player->x = SCREEN_WIDTH - player->w;
		}

		if (player->y > SCREEN_HEIGHT - player->h)
		{
			player->y = SCREEN_HEIGHT - player->h;
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

	prev = &stage.shotgunHead;

	for (e = stage.shotgunHead.next; e != NULL; e = e->next)
	{
		if (e->x < 0)
		{
			e->x = 0;
			e->dx = -e->dx;
		}

		if (e->x + e->w > SCREEN_WIDTH)
		{
			e->x = SCREEN_WIDTH - e->w;
			e->dx = -e->dx;
		}

		if (e->y < 0)
		{
			e->y = 0;
			e->dy = -e->dy;
		}

		if (e->y + e->h > SCREEN_HEIGHT)
		{
			e->y = SCREEN_HEIGHT - e->h;
			e->dy = -e->dy;
		}

		e->x += e->dx;
		e->y += e->dy;

		if (player != NULL && collision(e->x, e->y, e->w, e->h, player->x, player->y, player->w, player->h))
		{
			e->health = 0;

			shotgunEnabled = 1;

			playSound(SND_POINTS, CH_POINTS);
		}

		if (--e->health <= 0)
		{
			if (e == stage.pointsTail)
			{
				stage.pointsTail = prev;
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

static void addShotgunPod(int x, int y)
{
	Entity *e;

	e = malloc(sizeof(Entity));
	memset(e, 0, sizeof(Entity));
	stage.pointsTail->next = e;
	stage.pointsTail = e;

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

static void draw(void)
{
	drawBackground();

	drawStarfield();

	drawShotgunPods();

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
		SDL_Rect playerRect = {player->x, player->y, player->w, player->h};
		SDL_RenderCopyEx(app.renderer, playerTexture, NULL, &playerRect, rotationAngle, &rotationCenter, SDL_FLIP_NONE);
	}
}

static void drawShotgunPods(void)
{
	Entity *e;

	for (e = stage.shotgunHead.next; e != NULL; e = e->next)
	{
		if (e->health > (FPS * 2) || e->health % 12 < 6)
		{
			blit(e->texture, e->x, e->y);
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
			blit(e->texture, (int)e->x, (int)e->y); // Cast float x and y to int
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
		SDL_Rect bulletRect = {(int)b->x, (int)b->y, b->w, b->h};
		SDL_RenderCopyEx(app.renderer, b->texture, NULL, &bulletRect, rotationAngle, &rotationCenter, SDL_FLIP_NONE);
	}
}

static void drawDebris(void)
{
	Debris *d;

	for (d = stage.debrisHead.next; d != NULL; d = d->next)
	{
		blitRect(d->texture, &d->rect, d->x, d->y);
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

		blit(explosionTexture, e->x, e->y);
	}

	SDL_SetRenderDrawBlendMode(app.renderer, SDL_BLENDMODE_NONE);
}

static void drawHud(void)
{
	drawText(10, 10, 255, 255, 255, TEXT_LEFT, "SCORE: %03d", stage.score);

	if (stage.score < highscores.highscore[0].score)
	{
		drawText(SCREEN_WIDTH - 10, 10, 255, 255, 255, TEXT_RIGHT, "HIGHSCORE: %03d", highscores.highscore[0].score);
	}
	else
	{
		drawText(SCREEN_WIDTH - 10, 10, 0, 255, 0, TEXT_RIGHT, "HIGHSCORE: %03d", stage.score);
	}
}