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
  SDL_DestroyTexture(texture);
}

void renderCursor(SDL_Renderer *renderer, int x, int y, int w, int h) {
  SDL_Rect cursorRect = { x, y, w, h };
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &cursorRect); 
}

void renderDialogBox(SDL_Renderer *renderer, int i) {
  SDL_Rect dialogBoxBorderRect = { 0, WINDOW_HEIGHT - (220 * i), WINDOW_WIDTH, 220};
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &dialogBoxBorderRect); 
  SDL_Rect dialogBoxRect = { 20, WINDOW_HEIGHT - (210 * i), WINDOW_WIDTH - 40, 180};
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderFillRect(renderer, &dialogBoxRect); 
}

void renderPauseState(SDL_Renderer *renderer, TTF_Font *font) {
  SDL_Rect pauseOverlayRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 50);
  SDL_RenderFillRect(renderer, &pauseOverlayRect); 
  SDL_Color color = {255, 255, 255};
  renderText(renderer, font, "Paused", color, WINDOW_WIDTH/2 - 40, WINDOW_HEIGHT/2, 40, 20);
}
