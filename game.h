#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include "font.h"
#include "map.h"
#include "menu.h"
#include "item.h"
#include "quest.h"
#define IS_ACTIVE 1
#define IS_LOADING 3
#define IS_CUTSCENE 4
#define IS_MENU 5


typedef struct {
  SDL_Window *window;
  DynamicObject *mainCharacter;
  Map maps[2];
  Map* current_map;
  TTF_Font *font;
  float scrollX;
  float scrollY;
  SDL_Renderer *renderer;
  Quest *quests;
  ActiveQuests active_quests;
  Item *items;
  int is_paused;
  int items_count;
  int quests_count;
  int active_quest_count;
  int status;
  int dismissDialog;
  int time;
  DynamicArray inventory;
  time_t startTime;
  float dt;
  SDL_Texture *indoorTexture;
  SDL_Texture *terrainTexture;
  Menu *inventory_menu;
} Game;

void loadGame(Game *game); 
void loadMap(Game *game, char* fileName, int map_id, int startingTile, DynamicObject *mainCharacter); 
void shutdownGame(Game *game);

#endif
