#include <SDL2/SDL_image.h>
#include "rpg.h"

int moveLeft(Man *man, int tileDistance, int *tileSize) {
  if (fabs(man->totalMovedX) >= (tileDistance * *(int*)tileSize)/2) {
    man->isMoving = 0;
    man->moveLeft = 0;
    man->totalMovedX = 0;
    man->startingX = man->x;
    return 0;
  } else { 
    man->direction = LEFT;
    man->isMoving = 1;
    man->moveLeft = 1;
    man->totalMovedX = man->x - man->startingX;
    return 1;
  }
}

int moveRight(Man *man, int tileDistance, int *tileSize) {
  if (fabs(man->totalMovedX) >= (tileDistance * *(int*)tileSize)/2) {
    man->isMoving = 0;
    man->moveRight = 0;
    man->totalMovedX = 0;
    man->startingX = man->x;
    return 0;
  } else { 
    man->direction = RIGHT;
    man->isMoving = 1;
    man->moveRight = 1;
    man->totalMovedX = man->x - man->startingX;
    return 1;
  }
}

int moveUp(Man *man, int tileDistance, int *tileSize) {
  if (fabs(man->totalMovedY) >= (tileDistance * *(int*)tileSize)/2) {
    man->isMoving = 0;
    man->moveUp = 0;
    man->totalMovedY = 0;
    man->startingY = man->y;
    return 0;
  } else { 
    man->direction = UP;
    man->isMoving = 1;
    man->moveUp = 1;
    man->totalMovedY = man->y - man->startingY;
    return 1;
  }
}

int moveDown(Man *man, int tileDistance, int *tileSize) {
  if (fabs(man->totalMovedY) >= (tileDistance * *(int*)tileSize)/2) {
    man->isMoving = 0;
    man->moveDown = 0;
    man->totalMovedY = 0;
    man->startingY = man->y;
    return 0;
  } else { 
    man->direction = DOWN;
    man->isMoving = 1;
    man->moveDown = 1;
    man->totalMovedY = man->y - man->startingY;
    return 1;
  }
}

int speak(Man *man, char* text, int *dismissDialog, time_t duration) {
  time_t timer = SDL_GetTicks() / 1000;
  time_t passedDuration = duration && timer - man->actionTimer;
  if (*dismissDialog || passedDuration) {
    man->actionTimer = 0;
    man->currentDialog = NULL;
    *dismissDialog = 0;
    return 0;
  } else {
    man->currentDialog = text;
    *dismissDialog = 0;
    return 1;
  }
}

int executeAction(Action *action, Man *dynamic_object) {
  if (!dynamic_object->actionTimer) {
    dynamic_object->actionTimer = SDL_GetTicks() / 1000;
  }
  int running = action->action(dynamic_object, action->arg1, action->arg2, action->arg3);
  return running;
}

void addAction(int index, Man *dynamic_object, generic_function action, void* arg1, void* arg2, void* arg3){
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

Man initializeMan(SDL_Renderer *renderer, Man *man, int spriteValue, float angle, float mass, float walkThrust, float runThrust, Status status, Direction direction) {
  SDL_Surface *manIdleSurface = createSurface("images/man-idle.png");
  SDL_Surface *manRunningSurface = createSurface("images/man-running.png");
  man->actions = malloc(1 * sizeof(generic_function));
  man->actionSize = 0;
  man->actionCapacity = 1;
  man->actionTimer = 0;
  man->idleTexture= SDL_CreateTextureFromSurface(renderer, manIdleSurface);
  man->runningTexture= SDL_CreateTextureFromSurface(renderer, manRunningSurface);
  man->startCutscene = 1;
  man->angle = angle;
  man->w = manIdleSurface->w / 8;
  man->h = manIdleSurface->h / 8;
  man->mass = mass;
  man->normalForce = man->mass * GRAVITY * cos(90*M_PI); 
  man->walkThrust = walkThrust * PIXELS_PER_METER;
  man->runThrust = runThrust * PIXELS_PER_METER;
  man->thrustX = 0;
  man->thrustY = 0;
  man->directionX = 0;
  man->directionY = 0;
  man->frictionalForceX = 0;
  man->frictionalForceY = 0;
  man->isMoving = 0;
  man->isRunning = 0;
  man->moveLeft = 0;
  man->moveRight = 0;
  man->moveUp = 0;
  man->moveDown = 0;
  man->totalMovedX = 0;
  man->totalMovedY = 0;
  man->ax= 0;
  man->ay= 0;
  man->dx = 0;
  man->dy = 0;
  man->triggerDialog = 0;
  man->status = status;
  man->sprite = spriteValue;
  man->direction = direction;
  man->currentDialog = NULL;
  SDL_FreeSurface(manRunningSurface);
  SDL_FreeSurface(manIdleSurface);

  return *man;
}
