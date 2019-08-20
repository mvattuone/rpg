#include <SDL2/SDL_image.h>
#include "utils.h"

time_t toSeconds(time_t time) {
  return time/1000;
}

SDL_Surface* createSurface(char* filename) {
  SDL_Surface *surface;

  surface = IMG_Load(filename);

  if (surface == NULL) {
    printf("Could not find %s\n\n", filename);
    SDL_Quit();
    exit(1);
  }

  return surface;
}
