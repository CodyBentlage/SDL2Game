#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define STRNCPY(dest, src, n) \
	strncpy(dest, src, n);    \
	dest[n - 1] = '\0'

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 650

#define WORLD_WIDTH 5000
#define WORLD_HEIGHT 5000

#define MAX_SCORE_NAME_LENGTH 16
#define MAX_NAME_LENGTH 32
#define MAX_LINE_LENGTH 1024

#define FPS 60

#define ENEMY_SPEED 3
#define TURN_SPEED 0.05

#define PLAYER_BULLET_SPEED 20
#define LASER_SPEED 40
#define FLAME_SPEED 40
#define ALIEN_BULLET_SPEED 8

#define MAX_KEYBOARD_KEYS 350
#define MAX_MOUSE_BUTTONS 8
#define MAX_JOYSTICK_BUTTONS 16

#define SIDE_PLAYER 0
#define SIDE_ALIEN 1

#define MAX_STARS 500

#define MAX_SND_CHANNELS 8

#define NUM_HIGHSCORES 8

#define GLYPH_HEIGHT 28
#define GLYPH_WIDTH 18

#define M_PI 3.14159265358979323846
#define JOYSTICK_DEAD_ZONE 8000

#define HEALTH_BAR_WIDTH 200 // Health bar
#define HEALTH_BAR_HEIGHT 5
#define HEALTH_BAR_X 10
#define HEALTH_BAR_Y 10

#define BOOST_BAR_X 10 // For the boost bar
#define BOOST_BAR_Y 20
#define BOOST_BAR_WIDTH 170
#define BOOST_BAR_HEIGHT 5

#define ABILITY_BAR_X 10 // For the shotgun ability
#define ABILITY_BAR_Y 30
#define ABILITY_BAR_WIDTH 100
#define ABILITY_BAR_HEIGHT 5

#define HITBOX_SIZE 10.0f
#define COLLISION_RESTITUTION 0.1f

#define HIGHSCORE_FILE "highscores.txt"

enum
{
	CH_ANY = -1,
	CH_PLAYER,
	CH_PLAYER_FIRE_LASER,
	CH_ALIEN_FIRE,
	CH_POINTS,
	CH_HYPER_DRIVE,
	CH_SHIP_DOWN,
	CH_ALIEN_DIES,
	CH_PLAYER_DIES
};

enum
{
	SND_PLAYER_FIRE,
	SND_PLAYER_FIRE_LASER,
	SND_ALIEN_FIRE,
	SND_PLAYER_DIE,
	SND_SHIP_HIT,
	SND_ALIEN_DIE,
	SND_POINTS,
	SND_HYPER_DRIVE,
	SND_SHIP_DOWN,
	SND_MAX
};

enum
{
	TEXT_LEFT,
	TEXT_CENTER,
	TEXT_RIGHT
};