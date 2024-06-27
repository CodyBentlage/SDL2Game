#include <stdbool.h>

typedef struct Entity Entity;
typedef struct Explosion Explosion;
typedef struct Debris Debris;
typedef struct Texture Texture;

typedef struct
{
	void (*logic)(void);
	void (*draw)(void);
} Delegate;

struct Texture
{
	char name[MAX_NAME_LENGTH];
	SDL_Texture *texture;
	Texture *next;
};

typedef struct
{
	SDL_Renderer *renderer;
	SDL_Window *window;
	SDL_GameController *controller; // Controller initialization
	Delegate delegate;
	int keyboard[MAX_KEYBOARD_KEYS];
	int mouse_buttons[MAX_MOUSE_BUTTONS];
	int joystickButtons[MAX_JOYSTICK_BUTTONS];
	Texture textureHead, *textureTail;
	char inputText[MAX_LINE_LENGTH];
    int mouse_x; // Mouse x position
    int mouse_y; // Mouse y position
} App;

struct Entity
{
	float x;
	float y;
	int w;
	int h;
	float dx;
	float dy;
	int health;
	int reload;
	int boostTimer;
	int boostCooldown;
	int side;
	SDL_Texture *texture;
	Entity *next;
	float angle;
	float maxHealth;
	bool boostActive;
};

struct Explosion
{
	float x;
	float y;
	float dx;
	float dy;
	int r, g, b, a;
	Explosion *next;
};

struct Debris
{
	float x;
	float y;
	float dx;
	float dy;
	SDL_Rect rect;
	SDL_Texture *texture;
	int life;
	Debris *next;
};

typedef struct
{
	Entity fighterHead, *fighterTail;
	Entity bulletHead, *bulletTail;
	Entity shotgunPodHead, *shotgunPodTail;
	Entity healthPodHead, *healthPodTail;
	Explosion explosionHead, *explosionTail;
	Debris debrisHead, *debrisTail;
	int score;
} Stage;

typedef struct
{
	int x;
	int y;
	int speed;
} Star;

typedef struct
{
	char name[MAX_SCORE_NAME_LENGTH];
	int recent;
	int score;
} Highscore;

typedef struct
{
	Highscore highscore[NUM_HIGHSCORES];
} Highscores;