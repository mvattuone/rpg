#include <SDL2/SDL_image.h>
#include "physics.h"
#include "utils.h"

typedef int (*generic_function)(void*, void*, void*);

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

typedef struct {
  char id[3];
  float startingX, startingY;
  float x, y;
  int w, h;
  float angle;
  float dx, dy;
  float mass;
  float forceX;
  float forceY;
  int directionX;
  int directionY;
  float normalForce;
  float thrustX;
  float thrustY;
  int frictionalForceX;
  int frictionalForceY;
  int isMoving;
  int isRunning;
  int moveLeft;
  int moveRight;
  int moveUp;
  int moveDown;
  float totalMovedX;
  float totalMovedY;
  float ax;
  float ay;
  float walkThrust;
  float runThrust;
  int isMain;
  int currentTile;
  char* currentDialog;
  int startCutscene;
  int startingTile;
  size_t actionSize;
  size_t actionCapacity;
  time_t actionTimer;
  char *name; // string
  int sprite;
  Direction direction;
  Status status;
  SDL_Texture *idleTexture;
  SDL_Texture *runningTexture;
  generic_function *actions;
} Man;

Man initializeMan(SDL_Renderer *renderer, Man *man, int spriteValue, float angle, float mass, float walkThrust, float runThrust, Status status, Direction direction); 

void addAction(void * *actions, int index, generic_function action, size_t *size, size_t *capacity);
int* removeAction(void* *actions, int index, size_t *size);

int moveLeft(Man *man, int tileDistance, int* tileSize);
int moveRight(Man *man, int tileDistance, int* tileSize);
int moveUp(Man *man, int tileDistance, int* tileSize);
int moveDown(Man *man, int tileDistance, int* tileSize);
int speak(Man *man, char* text, time_t duration);
