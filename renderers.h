#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void renderText(SDL_Renderer *renderer, TTF_Font *font, char* text, SDL_Color color, int x, int y, int w, int h); 

void renderDialogBox(SDL_Renderer *renderer);

void renderPauseState(SDL_Renderer *renderer, TTF_Font *font);
