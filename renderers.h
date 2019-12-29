#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

void renderText(SDL_Renderer *renderer, TTF_Font *font, char* text, SDL_Color color, int x, int y, SDL_Surface *surface); 

void renderMenuText(SDL_Renderer *renderer, TTF_Font *font, char* text, SDL_Color color, int x, int y); 

void renderCursor(SDL_Renderer *renderer, int x, int y, int w, int h); 

void renderDialogBox(SDL_Renderer *renderer, int i);

void renderPauseState(SDL_Renderer *renderer, TTF_Font *font);
