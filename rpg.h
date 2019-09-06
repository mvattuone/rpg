#include <time.h>
#include <stdio.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include "map.h"
#include "renderers.h"
#include "utils.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define IS_ACTIVE 1
#define IS_PAUSED 2

typedef struct {
  SDL_Window *window;
  Man *mainCharacter;
  Map map;
  TTF_Font *font;
  float scrollX;
  float scrollY;
  SDL_Renderer *renderer;
  int status;
  int dismissDialog;
  int time;
  time_t startTime;
  float dt;
  float gravity;
  SDL_Texture *terrainTexture;
} Game;

int handleEvents(Game *game);
void doRender(Game *game);
void loadGame(Game *game);
void collisionDetect(Game *game);
void process(Game *game);
