#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "dynamic_object.h"
#include "camera.h"
#include "item.h"
#include "menu.h"
#include "utils.h"

void renderText(SDL_Renderer *renderer, TTF_Font *font, char* text, SDL_Color color, int x, int y, SDL_Surface *surface); 

void renderMenuText(SDL_Renderer *renderer, TTF_Font *font, char* text, SDL_Color color, int x, int y); 

void renderCursor(SDL_Renderer *renderer, int x, int y, int w, int h); 

void renderDialogBox(SDL_Renderer *renderer, int i);

void renderPauseState(SDL_Renderer *renderer, TTF_Font *font);

void renderCrate(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer); 

void renderBed(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer); 

void renderDoor(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer); 

void renderJar(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer); 

void renderMan(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer); 

void renderMenu(Menu *inventory_menu, Item *items, DynamicArray inventory, TTF_Font *font, SDL_Renderer *renderer, int items_count);

void renderTile(int x, int y, Camera camera, int tileSize, char tileId, SDL_Texture *texture, SDL_Renderer *renderer);
