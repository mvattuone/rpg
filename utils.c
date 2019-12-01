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

SDL_Texture* createTexture(SDL_Renderer *renderer, char* filename) {
  SDL_Surface *surface = createSurface(filename);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  return texture;
}

// Detect if two objects in space have a collision
int hasCollision(float x1, float y1, float x2, float y2, float w1, float h1, float w2, float h2)
{
  return (!((x1 > (x2+w2)) || (x2 > (x1+w1)) || (y1 > (y2+h2)) || (y2 > (y1+h1))));
}
