#include <time.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include "game.h"
#include "map.h"
#include "renderers.h"
#include "utils.h"
#include "menu.h"
#include "item.h"
#include "quest.h"
#include "inventory.h"

#define OK 1
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
typedef struct {
  int w;
  int h;
  float x;
  float y;
} Target;

int handleEvents(Game *game);
void doRender(Game *game);
void process(Game *game);
void handleInteraction(Game *game);
void triggerEvent(Game *game, DynamicObject *dynamic_object);
void triggerDrop(Game *game);
void handleObjectCollisions(Game *game, DynamicObject *dynamic_object);

