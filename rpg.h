#include <time.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include "map.h"

#define PIXELS_PER_METER 16
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
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
  float forceX;
  float forceY;
  float normalForce;
  float thrustX;
  float thrustY;
  float ax;
  float ay;
  float walkThrust;
  float runThrust;
  char *name; // string
  int sprite;
  Direction direction;
  Status status;
  SDL_Texture *idleTexture;
  SDL_Texture *runningTexture;
} Man;

typedef struct {
  SDL_Texture *texture;
  TTF_Font *font;
} Text;

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
  time_t startTime;
  float dt;
  float gravity;
  SDL_Texture *terrainTexture;
} Game;

int handleEvents(Game *game);
void doRender(Game *game);
void loadGame(Game *game);
void collisionDetect(Game *game);
void process(Game *game);
