#include <SDL2/SDL_image.h>
#include "rpg.h"

int moveLeft(DynamicObject *dynamic_object, int tileDistance, int *tileSize) {
  if (fabs(dynamic_object->totalMovedX) >= (tileDistance * *(int*)tileSize)/2) {
    dynamic_object->isMoving = 0;
    dynamic_object->moveLeft = 0;
    dynamic_object->totalMovedX = 0;
    dynamic_object->startingX = dynamic_object->x;
    return 0;
  } else { 
    dynamic_object->direction = LEFT;
    dynamic_object->isMoving = 1;
    dynamic_object->moveLeft = 1;
    dynamic_object->totalMovedX = dynamic_object->x - dynamic_object->startingX;
    return 1;
  }
}

int moveRight(DynamicObject *dynamic_object, int tileDistance, int *tileSize) {
  if (fabs(dynamic_object->totalMovedX) >= (tileDistance * *(int*)tileSize)/2) {
    dynamic_object->isMoving = 0;
    dynamic_object->moveRight = 0;
    dynamic_object->totalMovedX = 0;
    dynamic_object->startingX = dynamic_object->x;
    return 0;
  } else { 
    dynamic_object->direction = RIGHT;
    dynamic_object->isMoving = 1;
    dynamic_object->moveRight = 1;
    dynamic_object->totalMovedX = dynamic_object->x - dynamic_object->startingX;
    return 1;
  }
}

int moveUp(DynamicObject *dynamic_object, int tileDistance, int *tileSize) {
  if (fabs(dynamic_object->totalMovedY) >= (tileDistance * *(int*)tileSize)/2) {
    dynamic_object->isMoving = 0;
    dynamic_object->moveUp = 0;
    dynamic_object->totalMovedY = 0;
    dynamic_object->startingY = dynamic_object->y;
    return 0;
  } else { 
    dynamic_object->direction = UP;
    dynamic_object->isMoving = 1;
    dynamic_object->moveUp = 1;
    dynamic_object->totalMovedY = dynamic_object->y - dynamic_object->startingY;
    return 1;
  }
}

int moveDown(DynamicObject *dynamic_object, int tileDistance, int *tileSize) {
  if (fabs(dynamic_object->totalMovedY) >= (tileDistance * *(int*)tileSize)/2) {
    dynamic_object->isMoving = 0;
    dynamic_object->moveDown = 0;
    dynamic_object->totalMovedY = 0;
    dynamic_object->startingY = dynamic_object->y;
    return 0;
  } else { 
    dynamic_object->direction = DOWN;
    dynamic_object->isMoving = 1;
    dynamic_object->moveDown = 1;
    dynamic_object->totalMovedY = dynamic_object->y - dynamic_object->startingY;
    return 1;
  }
}

int speak(DynamicObject *dynamic_object, char* text, int *dismissDialog, time_t duration) {
  time_t timer = SDL_GetTicks() / 1000;
  time_t passedDuration = duration && timer - dynamic_object->actionTimer;
  if (*dismissDialog || passedDuration) {
    dynamic_object->actionTimer = 0;
    dynamic_object->currentDialog = NULL;
    *dismissDialog = 0;
    return 0;
  } else {
    dynamic_object->currentDialog = text;
    *dismissDialog = 0;
    return 1;
  }
}

int executeAction(Action *action, DynamicObject *dynamic_object) {
  if (!dynamic_object->actionTimer) {
    dynamic_object->actionTimer = SDL_GetTicks() / 1000;
  }
  int running = action->action(dynamic_object, action->arg1, action->arg2, action->arg3);
  return running;
}

void addAction(int index, DynamicObject *dynamic_object, generic_function action, void* arg1, void* arg2, void* arg3){
   dynamic_object->actions = realloc(dynamic_object->actions, sizeof(dynamic_object->actions) * 2);
   if (dynamic_object->actionSize >= dynamic_object->actionCapacity) {
     dynamic_object->actionCapacity = dynamic_object->actionSize * 2;
   }
   dynamic_object->actionSize = dynamic_object->actionSize + 1;
   dynamic_object->actions[index].action = action;
   dynamic_object->actions[index].arg1= arg1;
   dynamic_object->actions[index].arg2= arg2;
   dynamic_object->actions[index].arg3= arg3;
}

Action* removeAction(void* *actions, int index, size_t *size) 
{
    Action *updatedActions = malloc((*size - 1) * sizeof(Action)); // allocate an array with a size 1 less than the current one

    if (index != 0) {
        memcpy(updatedActions, actions, index * sizeof(Action)); // copy everything BEFORE the index
    }

    if (index != (*size - 1)) {
        memcpy(updatedActions + index, actions + index + 1, (*size - index - 1) * sizeof(Action)); // copy everything AFTER the index
    }

    *size = *size - 1;

    free(actions);
    return updatedActions;
}

DynamicObject initializeMan(SDL_Renderer *renderer, DynamicObject *dynamic_object, int spriteValue, float angle, float mass, float walkThrust, float runThrust, Status status, Direction direction) {
  SDL_Surface *manIdleSurface = createSurface("images/man-idle.png");
  SDL_Surface *manRunningSurface = createSurface("images/man-running.png");
  dynamic_object->actions = malloc(1 * sizeof(generic_function));
  dynamic_object->prevActionSize = 0;
  dynamic_object->actionSize = 0;
  dynamic_object->actionCapacity = 1;
  dynamic_object->actionTimer = 0;
  dynamic_object->idleTexture= SDL_CreateTextureFromSurface(renderer, manIdleSurface);
  dynamic_object->runningTexture= SDL_CreateTextureFromSurface(renderer, manRunningSurface);
  dynamic_object->angle = angle;
  dynamic_object->w = manIdleSurface->w / 8;
  dynamic_object->h = manIdleSurface->h / 8;
  dynamic_object->mass = mass;
  dynamic_object->normalForce = dynamic_object->mass * GRAVITY * cos(90*M_PI); 
  dynamic_object->walkThrust = walkThrust * PIXELS_PER_METER;
  dynamic_object->runThrust = runThrust * PIXELS_PER_METER;
  dynamic_object->thrustX = 0;
  dynamic_object->thrustY = 0;
  dynamic_object->directionX = 0;
  dynamic_object->directionY = 0;
  dynamic_object->frictionalForceX = 0;
  dynamic_object->frictionalForceY = 0;
  dynamic_object->isMoving = 0;
  dynamic_object->isRunning = 0;
  dynamic_object->moveLeft = 0;
  dynamic_object->moveRight = 0;
  dynamic_object->moveUp = 0;
  dynamic_object->moveDown = 0;
  dynamic_object->totalMovedX = 0;
  dynamic_object->totalMovedY = 0;
  dynamic_object->ax= 0;
  dynamic_object->ay= 0;
  dynamic_object->dx = 0;
  dynamic_object->dy = 0;
  dynamic_object->triggerDialog = 0;
  dynamic_object->status = status;
  dynamic_object->sprite = spriteValue;
  dynamic_object->direction = direction;
  dynamic_object->currentDialog = NULL;
  SDL_FreeSurface(manRunningSurface);
  SDL_FreeSurface(manIdleSurface);

  return *dynamic_object;
}
