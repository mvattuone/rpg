#include <SDL2/SDL_image.h>
#include "rpg.h"

void addAction(void * *actions, int index, generic_function action, size_t *size, size_t *capacity){
     actions = realloc(actions, sizeof(actions) * 2);
     *capacity = sizeof(actions)/sizeof(actions[0]) * 2;
     *size = *size + 1;
     printf("*size before is %zu\n", *size);
     
     actions[index] = action;
}

int* removeAction(void* *actions, int index, size_t *size) 
{
    int* updatedActions = malloc((*size - 1) * sizeof(generic_function)); // allocate an array with a size 1 less than the current one

    if (index != 0) {
        memcpy(updatedActions, actions, index * sizeof(generic_function)); // copy everything BEFORE the index
    }

    if (index != (*size - 1)) {
        memcpy(updatedActions + index, actions + index + 1, (*size - index - 1) * sizeof(generic_function)); // copy everything AFTER the index
    }

    *size = *size - 1;
    printf("*size after is %zu\n", *size);

    free(actions);
    return updatedActions;
}

Man initializeMan(SDL_Renderer *renderer, Man *man, int spriteValue, float angle, float mass, float walkThrust, float runThrust, Status status, Direction direction) {
  SDL_Surface *manIdleSurface = createSurface("images/man-idle.png");
  SDL_Surface *manRunningSurface = createSurface("images/man-running.png");
  man->actions = malloc(1 * sizeof(generic_function));
  man->actionSize = 0;
  man->actionCapacity = 1;
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
  man->status = status;
  man->sprite = spriteValue;
  man->direction = direction;
  SDL_FreeSurface(manRunningSurface);
  SDL_FreeSurface(manIdleSurface);

  return *man;
}
