#include <SDL2/SDL_image.h>
#include "dynamicObject.h"

Man initializeMan(SDL_Renderer *renderer, Man *man, int spriteValue, float angle, float mass, float walkThrust, float runThrust, Status status, Direction direction) {
  SDL_Surface *manIdleSurface = createSurface("images/man-idle.png");
  SDL_Surface *manRunningSurface = createSurface("images/man-running.png");
  man->idleTexture= SDL_CreateTextureFromSurface(renderer, manIdleSurface);
  man->runningTexture= SDL_CreateTextureFromSurface(renderer, manRunningSurface);
  man->isMain = man->id == 'x' ? 1 : 0;
  man->angle = angle;
  man->w = manIdleSurface->w / 8;
  man->h = manIdleSurface->h / 8;
  man->mass = mass;
  man->normalForce = man->mass * GRAVITY * cos(90*M_PI); 
  man->walkThrust = walkThrust * PIXELS_PER_METER;
  man->runThrust = runThrust * PIXELS_PER_METER;
  man->thrustX = 0;
  man->thrustY = 0;
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
