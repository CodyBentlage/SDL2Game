#include <stdbool.h>

typedef struct Entity Entity;
typedef struct SpaceBeam Beam;
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
	int mouse_x;
	int mouse_y;
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
	int systemsTimer;
	int systemsCooldown;
	int side;
	float angle;
	float maxHealth;
	bool boostActive;
	int invincibilityFrames;
	SDL_Texture *texture;
	Entity *next;
};

struct SpaceBeam
{
	float x;
	float y;
	int w;
	int h;
	float dx;
	float dy;
	int health;
	int reload;
	int side;
	float angle;
	float maxHealth;
	bool boostActive;
	int r, g, b, a;
	SDL_Texture *beamTexture;
	Beam *next;
};

struct Explosion
{
	float x;
	float y;
	float dx;
	float dy;
	int r, g, b, a;
	Uint32 creationTime;
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
	Entity crossHairHead, *crossHairTail;
	Entity bulletHead, *bulletTail;
	Beam beamHead, *beamTail;
	Entity shotgunPodHead, *shotgunPodTail;
	Entity laserPodHead, *laserPodTail;
	Entity flamePodHead, *flamePodTail;
	Entity spaceBeamPodHead, *spaceBeamPodTail;
	Entity healthPodHead, *healthPodTail;
	Entity littleBuddyPodHead, *littleBuddyPodTail;
	Explosion explosionHead, *explosionTail;
	Debris debrisHead, *debrisTail;
	int score;
	int currentEnemyCount;
	int cameraX;
	int cameraY;
	bool gamePaused;
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

typedef struct
{
	SDL_Texture *texture;
	int x, y;
	int dx, dy;
} BackgroundElement;