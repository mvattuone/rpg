#include <time.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>

#define PIXELS_PER_METER 1000
#define IS_ACTIVE 1
#define IS_PAUSED 2
#define IS_RESTING 3

typedef enum {
  RIGHT,
  LEFT,
  UP,
  DOWN
} direction;

typedef enum {
  CAT_WALKING_RIGHT_LEFT_FOOT = 0,
  CAT_STANDING_RIGHT = 1,
  CAT_WALKING_RIGHT_RIGHT_FOOT = 2,
  CAT_RESTING_RIGHT = 3,
  CAT_WALKING_UP_RIGHT_FOOT = 0,
  CAT_STANDING_UP = 1,
  CAT_WALKING_UP_LEFT_FOOT = 2,
  CAT_WALKING_DOWN_RIGHT_FOOT = 0,
  CAT_STANDING_DOWN = 1,
  CAT_WALKING_DOWN_LEFT_FOOT = 2,
  CAT_RESTING_UP = 4,
  CAT_WALKING_LEFT_RIGHT_FOOT = 0,
  CAT_STANDING_LEFT = 1,
  CAT_WALKING_LEFT_LEFT_FOOT = 2,
  CAT_RESTING_LEFT = 3,
} sprites;

typedef struct {
  float x, y;
  int w, h;
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
  int direction;
  int isFacingX;
  int isFacingY;
  SDL_Texture *texture;
} Cat;

typedef struct {
  int sprite;
  int speed;
  int x,y,w,h;
  SDL_Texture *texture;
} Dog;

typedef struct {
  int x,y,w,h;
} Floor;

typedef struct {
  SDL_Texture *texture;
  TTF_Font *font;
} Text;

typedef struct {
  SDL_Window *window;
  Cat cat;
  Dog dogs[100];
  Floor floors[100];
  Text text;
  float scrollX;
  float scrollY;
  SDL_Renderer *renderer;
  int status;
  int time;
  int life;
  int row;
  time_t startTime;
  time_t dt;
  float gravity;
} Game;

int handleEvents(Game *game);
void doRender(Game *game);
void loadGame(Game *game);
void collisionDetect(Game *game);
void process(Game *game);
