#include "common.h"
#include "draw.h"
#include "enemies.h"

extern Stage stage;

static SDL_Texture *enemyFighterTexture;
static SDL_Texture *enemyMosquitoTexture;
static SDL_Texture *enemyTheCube;

static int enemySpawnTimer;

void spawnEnemyTheCube(void)
{
    Entity *enemy;

    enemyTheCube = loadTexture("gfx/enemyTheCube.png");

    if (--enemySpawnTimer <= 0)
    {
        enemy = malloc(sizeof(Entity));
        memset(enemy, 0, sizeof(Entity));
        stage.fighterTail->next = enemy;
        stage.fighterTail = enemy;

        enemy->x = SCREEN_WIDTH;
        enemy->y = rand() % (SCREEN_HEIGHT - 70); // Ensure enemy spawns below the HUD area
        enemy->texture = enemyTheCube;
        SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

        enemy->dx = -(2 + (rand() % 4));
        enemy->dy = -100 + (rand() % 200);
        enemy->dy /= 100;

        enemy->side = SIDE_ALIEN;
        enemy->maxHealth = 100;
        enemy->health = 100;

        enemy->reload = FPS * (1 + (rand() % 3));

        enemySpawnTimer = 1 + (rand() % FPS);
    }
}

void spawnEnemyFighters(void)
{
    Entity *enemy;

    enemyFighterTexture = loadTexture("gfx/enemyFighter.png");

    if (--enemySpawnTimer <= 0)
    {
        enemy = malloc(sizeof(Entity));
        memset(enemy, 0, sizeof(Entity));
        stage.fighterTail->next = enemy;
        stage.fighterTail = enemy;

        enemy->x = SCREEN_WIDTH;
        enemy->y = rand() % (SCREEN_HEIGHT - 70); // Ensure enemy spawns below the HUD area
        enemy->texture = enemyFighterTexture;
        SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

        enemy->dx = -(2 + (rand() % 4));
        enemy->dy = -100 + (rand() % 200);
        enemy->dy /= 100;

        enemy->side = SIDE_ALIEN;
        enemy->maxHealth = 50;
        enemy->health = 50;

        enemy->reload = FPS * (1 + (rand() % 3));

        enemySpawnTimer = 20 + (rand() % FPS);
    }
}

void spawnEnemyMosquitos(void)
{
    Entity *enemy;

    enemyMosquitoTexture = loadTexture("gfx/enemyMosquito.png");

    if (--enemySpawnTimer <= 0)
    {
        enemy = malloc(sizeof(Entity));
        memset(enemy, 0, sizeof(Entity));
        stage.fighterTail->next = enemy;
        stage.fighterTail = enemy;

        enemy->x = SCREEN_WIDTH;
        enemy->y = rand() % (SCREEN_HEIGHT - 70); // Ensure enemy spawns below the HUD area
        enemy->texture = enemyMosquitoTexture;
        SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

        enemy->dx = -(2 + (rand() % 4));
        enemy->dy = -100 + (rand() % 200);
        enemy->dy /= 100;

        enemy->side = SIDE_ALIEN;
        enemy->maxHealth = 20;
        enemy->health = 20;

        enemy->reload = FPS * (1 + (rand() % 3));

        enemySpawnTimer = 70 + (rand() % FPS);
    }
}