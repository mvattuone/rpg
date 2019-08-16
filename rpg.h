#include <time.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>

#define PIXELS_PER_METER 1000
#define GAME_OVER 0
#define IS_ACTIVE 1
#define IS_PAUSED 2
#define IS_RESTING 3

typedef enum {
  UP,
  UPRIGHT,
  RIGHT,
  DOWNRIGHT,
  DOWN,
  DOWNLEFT,
  LEFT,
  UPLEFT
} Direction;

typedef enum {
  IS_IDLE,
  IS_RUNNING
} Status;

typedef enum {
  MAN_UP = 0,
  MAN_LEFT = 0,
  MAN_DOWN = 0,
  MAN_RIGHT = 0
} sprites;

typedef struct {
  float x, y;
  int w, h;
  float angle;
  float dx, dy;
  float mass;
  float force;
  float thrustX;
  float thrustY;
  float ax;
  float ay;
  float maxThrust;
  short health;
  char *name; // string
  int sprite;
  int isOnFloor;
  Direction direction;
  Status status;
  int isFacingX;
  int isFacingY;
  SDL_Texture *idleTexture;
  SDL_Texture *runningTexture;
} Man;

typedef struct {
  int x,y,w,h;
  char* tileId;
} Tile;

typedef struct {
  SDL_Texture *texture;
  TTF_Font *font;
} Text;

typedef struct {
  int rowL;
  int columnL;
  int tileSize;
  Tile tiles[96];
} Map;

typedef struct {
  SDL_Window *window;
  Man man;
  Map map;
  Text text;
  float scrollX;
  float scrollY;
  SDL_Renderer *renderer;
  int status;
  int time;
  int life;
  time_t startTime;
  time_t dt;
  float gravity;
  SDL_Texture *terrainTexture;
} Game;

int handleEvents(Game *game);
void doRender(Game *game);
void loadGame(Game *game);
void collisionDetect(Game *game);
void process(Game *game);
