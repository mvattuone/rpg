#include <time.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include "map.h"
#include "renderers.h"
#include "utils.h"
#include "menu.h"
#include "item.h"
#include "quest.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define IS_ACTIVE 1
#define IS_PAUSED 2
#define IS_DIALOGUE 3
#define IS_CUTSCENE 4
#define IS_MENU 5

typedef struct {
  int capacity;
  int size; 
  int* items;
} DynamicArray;

typedef struct {
  int w;
  int h;
  float x;
  float y;
} Target;

typedef struct {
  SDL_Window *window;
  DynamicObject *mainCharacter;
  Map map;
  TTF_Font *font;
  float scrollX;
  float scrollY;
  SDL_Renderer *renderer;
  Quest *quests;
  Item *items;
  int items_count;
  int quest_count;
  int status;
  int dismissDialog;
  int time;
  DynamicArray inventory;
  time_t startTime;
  float dt;
  SDL_Texture *terrainTexture;
  Menu *inventory_menu;
} Game;

int handleEvents(Game *game);
void doRender(Game *game);
void loadGame(Game *game);
void process(Game *game);
void handleInteraction(Game *game);
void triggerDrop(Game *game);
void handleObjectCollisions(Game *game, DynamicObject *dynamic_object);
