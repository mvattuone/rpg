#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "renderers.h"

void renderText(SDL_Renderer *renderer, TTF_Font *font, char* text, SDL_Color color, int x, int y, int w, int h) {
  SDL_Rect textRect = {x, y, w, h};
  SDL_Surface *surface = TTF_RenderText_Solid(font, text, color);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_RenderCopy(renderer, texture, NULL, &textRect);
  SDL_FreeSurface(surface);
}
