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
            stage.currentEnemyCount++;

            enemy = malloc(sizeof(Entity));
            memset(enemy, 0, sizeof(Entity));
            stage.fighterTail->next = enemy;
            stage.fighterTail = enemy;

            // Determine spawn coordinates outside the camera view
            int spawnEdge = rand() % 4; // 0 = top, 1 = bottom, 2 = left, 3 = right
            int bufferDistance = 100;   // Distance outside camera view to spawn enemies

            switch (spawnEdge)
            {
            case 0: // Top edge
                enemy->x = stage.cameraX + rand() % SCREEN_WIDTH;
                enemy->y = stage.cameraY - bufferDistance - enemy->h;
                break;
            case 1: // Bottom edge
                enemy->x = stage.cameraX + rand() % SCREEN_WIDTH;
                enemy->y = stage.cameraY + SCREEN_HEIGHT + bufferDistance;
                break;
            case 2: // Left edge
                enemy->x = stage.cameraX - bufferDistance - enemy->w;
                enemy->y = stage.cameraY + rand() % SCREEN_HEIGHT;
                break;
            case 3: // Right edge
                enemy->x = stage.cameraX + SCREEN_WIDTH + bufferDistance;
                enemy->y = stage.cameraY + rand() % SCREEN_HEIGHT;
                break;
            default:
                break;
            }

            // Clamp spawn coordinates to game world boundaries
            if (enemy->x < 0)
                enemy->x = 0;
            if (enemy->y < 0)
                enemy->y = 0;
            if (enemy->x > WORLD_WIDTH - enemy->w)
                enemy->x = WORLD_WIDTH - enemy->w;
            if (enemy->y > WORLD_HEIGHT - enemy->h)
                enemy->y = WORLD_HEIGHT - enemy->h;

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
            stage.currentEnemyCount++;

            enemy = malloc(sizeof(Entity));
            memset(enemy, 0, sizeof(Entity));
            stage.fighterTail->next = enemy;
            stage.fighterTail = enemy;

            // Determine spawn coordinates outside the camera view
            int spawnEdge = rand() % 4; // 0 = top, 1 = bottom, 2 = left, 3 = right
            int bufferDistance = 100;   // Distance outside camera view to spawn enemies

            switch (spawnEdge)
            {
            case 0: // Top edge
                enemy->x = stage.cameraX + rand() % SCREEN_WIDTH;
                enemy->y = stage.cameraY - bufferDistance - enemy->h;
                break;
            case 1: // Bottom edge
                enemy->x = stage.cameraX + rand() % SCREEN_WIDTH;
                enemy->y = stage.cameraY + SCREEN_HEIGHT + bufferDistance;
                break;
            case 2: // Left edge
                enemy->x = stage.cameraX - bufferDistance - enemy->w;
                enemy->y = stage.cameraY + rand() % SCREEN_HEIGHT;
                break;
            case 3: // Right edge
                enemy->x = stage.cameraX + SCREEN_WIDTH + bufferDistance;
                enemy->y = stage.cameraY + rand() % SCREEN_HEIGHT;
                break;
            default:
                break;
            }

            // Clamp spawn coordinates to game world boundaries
            if (enemy->x < 0)
                enemy->x = 0;
            if (enemy->y < 0)
                enemy->y = 0;
            if (enemy->x > WORLD_WIDTH - enemy->w)
                enemy->x = WORLD_WIDTH - enemy->w;
            if (enemy->y > WORLD_HEIGHT - enemy->h)
                enemy->y = WORLD_HEIGHT - enemy->h;

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
            stage.currentEnemyCount++;

            enemy = malloc(sizeof(Entity));
            memset(enemy, 0, sizeof(Entity));
            stage.fighterTail->next = enemy;
            stage.fighterTail = enemy;

            // Determine spawn coordinates outside the camera view
            int spawnEdge = rand() % 4; // 0 = top, 1 = bottom, 2 = left, 3 = right
            int bufferDistance = 100;   // Distance outside camera view to spawn enemies

            switch (spawnEdge)
            {
            case 0: // Top edge
                enemy->x = stage.cameraX + rand() % SCREEN_WIDTH;
                enemy->y = stage.cameraY - bufferDistance - enemy->h;
                break;
            case 1: // Bottom edge
                enemy->x = stage.cameraX + rand() % SCREEN_WIDTH;
                enemy->y = stage.cameraY + SCREEN_HEIGHT + bufferDistance;
                break;
            case 2: // Left edge
                enemy->x = stage.cameraX - bufferDistance - enemy->w;
                enemy->y = stage.cameraY + rand() % SCREEN_HEIGHT;
                break;
            case 3: // Right edge
                enemy->x = stage.cameraX + SCREEN_WIDTH + bufferDistance;
                enemy->y = stage.cameraY + rand() % SCREEN_HEIGHT;
                break;
            default:
                break;
            }

            // Clamp spawn coordinates to game world boundaries
            if (enemy->x < 0)
                enemy->x = 0;
            if (enemy->y < 0)
                enemy->y = 0;
            if (enemy->x > WORLD_WIDTH - enemy->w)
                enemy->x = WORLD_WIDTH - enemy->w;
            if (enemy->y > WORLD_HEIGHT - enemy->h)
                enemy->y = WORLD_HEIGHT - enemy->h;

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