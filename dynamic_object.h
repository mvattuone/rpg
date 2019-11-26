#include <SDL2/SDL_image.h>
#include "physics.h"
#include "utils.h"

#define MAX_TASK_SIZE 250
#define MAX_TASKS 10
#define MAX_INTERACTIONS 10

typedef int (*generic_function)(void*, void*, void*, void*);

// This in theory will contain every possible "state" of the game
typedef enum {
  DEFAULT,
  SPOKEN,
  SPOKEN_TWICE,
  QUEST_ACTIVE,
  QUEST_ACTIVE_SPOKEN_TWICE,
  QUEST_COMPLETED,
  QUEST_COMPLETED_SPOKEN_TWICE
} State;

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
  MAN,
  CRATE,
  JAR
} ObjectType;

typedef enum {
  IS_IDLE,
  IS_RUNNING
} Status;

typedef enum {
  IDLE,
  WALKING
} Behavior;

typedef struct {
  generic_function action;
  void* arg1;
  void* arg2;
  void* arg3;
} QueueItem;

typedef struct {
  QueueItem *items;
  int is_enqueuing;
  size_t size;
  size_t capacity;
  size_t timer;
  size_t prev_size; 
} Queue;


typedef enum {
  MOVE_LEFT,
  MOVE_RIGHT,
  MOVE_UP,
  MOVE_DOWN,
  SPEAK,
  REMOVE,
  ADD_ITEM
} TaskType;

typedef struct {
  TaskType type; 
  char data[MAX_TASK_SIZE];
} Task;

typedef struct {
  int task_count;
  Task tasks[MAX_TASKS];
} Interaction;

typedef struct {
  int id;
  Interaction interactions[MAX_INTERACTIONS];
  int interactions_count;
  ObjectType type; 
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
  float frictionalForceX;
  float frictionalForceY;
  int has_object;
  int isLiftable;
  int isMoving;
  int isMovable;
  int isLifted;
  int isLifting;
  int isPushing;
  int isRunning;
  int moveLeft;
  int moveRight;
  int moveUp;
  int moveDown;
  State state;
  float totalMovedX;
  float totalMovedY;
  float ax;
  float ay;
  float walkThrust;
  float runThrust;
  int quest;
  int isMain;
  int currentTile;
  int triggerDialog;
  char* currentDialog;
  int startingTile;
  Queue task_queue;
  char *name; // string
  int sprite;
  Behavior default_behavior;
  Direction direction;
  Status status;
  SDL_Texture *idleTexture;
  SDL_Texture *runningTexture;
  SDL_Texture *crateTexture;
  SDL_Texture *jarTexture;
} DynamicObject;

DynamicObject initialize_dynamic_object(SDL_Renderer *renderer, DynamicObject *dynamic_object, int spriteValue, float angle, float mass, float walkThrust, float runThrust, Status status, Direction direction, ObjectType type); 


// Probably this should be split into two functions...
void enqueue(Queue *queue, generic_function action, void* arg1, void* arg2, void* arg3);
int process_queue(DynamicObject *dynamic_object, Queue *queue); 
Queue dequeue(Queue *actions);

int moveLeft(DynamicObject *dynamic_object, int tileDistance, int* tileSize);
int moveRight(DynamicObject *dynamic_object, int tileDistance, int* tileSize);
int moveUp(DynamicObject *dynamic_object, int tileDistance, int* tileSize);
int moveDown(DynamicObject *dynamic_object, int tileDistance, int* tileSize);
int speak(DynamicObject *dynamic_object, char* text, int *dismissDialog, time_t duration);
int removeObject(DynamicObject *dynamic_object);
