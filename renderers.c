#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "rpg.h"
#include "renderers.h"

void renderText(SDL_Renderer *renderer, TTF_Font *font, char* text, SDL_Color color, int x, int y, int w, int h) {
  SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font, text, color, WINDOW_WIDTH - 50);
  SDL_Rect textRect = {x, y, surface->w, surface->h};
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_RenderCopy(renderer, texture, NULL, &textRect);
  SDL_FreeSurface(surface);
}

void renderDialogBox(SDL_Renderer *renderer) {
  SDL_Rect dialogBoxBorderRect = { 0, WINDOW_HEIGHT - 220, WINDOW_WIDTH, 220};
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &dialogBoxBorderRect); 
  SDL_Rect dialogBoxRect = { 20, WINDOW_HEIGHT - 200, WINDOW_WIDTH - 40, 180};
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderFillRect(renderer, &dialogBoxRect); 
}
