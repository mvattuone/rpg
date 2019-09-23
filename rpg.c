#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include "rpg.h"
#include "physics.h"

int handleEvents(Game *game) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        return 1;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.scancode) {
          case SDL_SCANCODE_A:
            if (game->status != IS_CUTSCENE &&  game->status != IS_DIALOGUE) {
              triggerDialog(game);
            } else {
              game->dismissDialog = 1;
            }
            break;
          default:
            break;
        }
      case SDL_KEYUP:
        switch (event.key.keysym.scancode) { 
          case SDL_SCANCODE_SPACE:
            game->mainCharacter->isRunning = 0;
            break;
          default:
            break;
        }
      default:
        break;
    }
  }
 
  if (game->status == IS_ACTIVE) {
    SDL_PumpEvents();
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_DOWN]) {
      game->mainCharacter->isMoving = 1;
    } 
    if (state[SDL_SCANCODE_SPACE]) {
      game->mainCharacter->isRunning = 1;
    }
    if (state[SDL_SCANCODE_LEFT]) {
      game->mainCharacter->moveLeft = 1;
      if (state[SDL_SCANCODE_RIGHT]) {
        game->mainCharacter->moveLeft  = 0;
      }
    } else if (!state[SDL_SCANCODE_LEFT]) {
      game->mainCharacter->moveLeft = 0;
      if (state[SDL_SCANCODE_RIGHT]) {
        game->mainCharacter->moveRight = 1;
      } else {
        game->mainCharacter->moveRight = 0;
      }
    }
    if (state[SDL_SCANCODE_RIGHT]) {
      game->mainCharacter->moveRight = 1;
      if (state[SDL_SCANCODE_LEFT]) {
        game->mainCharacter->moveRight = 0;
      }
    } else if (!state[SDL_SCANCODE_RIGHT]) {
      game->mainCharacter->moveRight = 0;
      if (state[SDL_SCANCODE_LEFT]) {
        game->mainCharacter->moveLeft = 1;
      } else {
        game->mainCharacter->moveLeft = 0;
      }
    }
    if (state[SDL_SCANCODE_UP]) {
      game->mainCharacter->moveUp = 1;
      if (state[SDL_SCANCODE_DOWN]) {
        game->mainCharacter->moveUp = 0;
      }
    } else if (!state[SDL_SCANCODE_UP]) {
      game->mainCharacter->moveUp = 0;
      if (state[SDL_SCANCODE_DOWN]) {
        game->mainCharacter->moveDown = 1;
      } else {
        game->mainCharacter->moveDown = 0;
      }
    }
    if (state[SDL_SCANCODE_DOWN]) {
      game->mainCharacter->moveDown = 1;
      if (state[SDL_SCANCODE_UP]) {
        game->mainCharacter->moveDown = 0;
      }
    } else if (!state[SDL_SCANCODE_DOWN]) {
      game->mainCharacter->moveDown = 0;
      if (state[SDL_SCANCODE_UP]) {
        game->mainCharacter->moveUp = 1;
      } else {
        game->mainCharacter->moveUp = 0;
        game->mainCharacter->moveDown = 0;
      }
    }

    if (!state[SDL_SCANCODE_UP] && !state[SDL_SCANCODE_DOWN] && !state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT]) {
      game->mainCharacter->isMoving = 0;
      game->mainCharacter->isRunning = 0;
      game->mainCharacter->moveLeft = 0;
      game->mainCharacter->moveRight = 0;
      game->mainCharacter->moveUp = 0;
      game->mainCharacter->moveDown = 0;
    }
  }
 
  return 0;
}

int handlePhysics(DynamicObject *dynamic_object, float *dt) {
  float cof = 0.41; // This will be attached to the tile the player is on eventually
  if (dynamic_object->isMoving) {
    dynamic_object->frictionalForceX = cof * dynamic_object->normalForce;
    dynamic_object->frictionalForceY = cof * dynamic_object->normalForce;
  }
  if (dynamic_object->moveLeft) {
    dynamic_object->directionX = -1;
    dynamic_object->thrustX = dynamic_object->isRunning ? dynamic_object->runThrust : dynamic_object->walkThrust;
    if (dynamic_object->moveRight) {
      dynamic_object->directionX = 0;
    }
  } else if (!dynamic_object->moveLeft) {
    dynamic_object->directionX = dynamic_object->dx < 0 ? 1 : 0;
    if (dynamic_object->moveRight) {
      dynamic_object->directionX = 1;
    } else {
      dynamic_object->thrustX *= -1;
    }
  }
  if (dynamic_object->moveRight) {
    dynamic_object->directionX = 1;
    dynamic_object->thrustX = dynamic_object->isRunning ? dynamic_object->runThrust : dynamic_object->walkThrust;
    if (dynamic_object->moveLeft) {
      dynamic_object->directionX = 0;
    }
  } else if (!dynamic_object->moveRight) {
    dynamic_object->directionX = dynamic_object->dx > 0 ? -1 : 0;
    dynamic_object->directionX = dynamic_object->dx < 0 ? 1 : dynamic_object->directionX;
    if (dynamic_object->moveLeft) {
      dynamic_object->directionX = -1;
    } else {
      dynamic_object->thrustX *= -1;
    }
  }
  if (dynamic_object->moveUp) {
    dynamic_object->directionY = -1;
    dynamic_object->thrustY = dynamic_object->isRunning ? dynamic_object->runThrust : dynamic_object->walkThrust; 
    if (dynamic_object->moveDown) {
      dynamic_object->directionY = 0;
    }
  } else if (!dynamic_object->moveUp) {
    dynamic_object->directionY = dynamic_object->dy < 0 ? 1 : 0;
    if (dynamic_object->moveDown) {
      dynamic_object->directionY = 1;
    } else {
      dynamic_object->thrustY *= -1;
    }
  }
  if (dynamic_object->moveDown) {
    dynamic_object->directionY = 1;
    dynamic_object->thrustY = dynamic_object->isRunning ? dynamic_object->runThrust : dynamic_object->walkThrust; 
    if (dynamic_object->moveUp) {
      dynamic_object->directionY = 0;
    }
  } else if (!dynamic_object->moveDown) {
    dynamic_object->directionY = dynamic_object->dy > 0 ? -1 : 0;
    dynamic_object->directionY = dynamic_object->dy < 0 ? 1 : dynamic_object->directionY;
    if (dynamic_object->moveUp) {
      dynamic_object->directionY = -1;
    } else {
      dynamic_object->thrustY *= -1;
    }
  }

  dynamic_object->ax = (((dynamic_object->directionX * dynamic_object->thrustX) - (dynamic_object->directionX * dynamic_object->frictionalForceX)) / dynamic_object->mass);
  dynamic_object->ay = (((dynamic_object->directionY * dynamic_object->thrustY) - (dynamic_object->directionY * dynamic_object->frictionalForceY)) / dynamic_object->mass);
  dynamic_object->dx = accelerate(dynamic_object->dx, dynamic_object->ax, *dt); 
  dynamic_object->dy = accelerate(dynamic_object->dy, dynamic_object->ay, *dt); 
  
  float maxSpeed = 60.0f;
  float maxRunningSpeed = 120.0f;
  
  if (dynamic_object->isMoving) {
    if (!dynamic_object->isRunning && dynamic_object->dx >= maxSpeed) {
      dynamic_object->dx = maxSpeed; 
    }
    if (dynamic_object->isRunning && dynamic_object->dx >= maxRunningSpeed) {
      dynamic_object->dx = maxRunningSpeed;
    }
    if (!dynamic_object->isRunning && dynamic_object->dx <= -maxSpeed) {
      dynamic_object->dx = -maxSpeed; 
    }
    if (dynamic_object->isRunning && dynamic_object->dx <= -maxRunningSpeed) {
      dynamic_object->dx = -maxRunningSpeed;
    }

    if (!dynamic_object->isRunning && dynamic_object->dy >= maxSpeed) {
      dynamic_object->dy = maxSpeed; 
    }
    if (dynamic_object->isRunning && dynamic_object->dy >= maxRunningSpeed) {
      dynamic_object->dy = maxRunningSpeed;
    }
    if (!dynamic_object->isRunning && dynamic_object->dy <= -maxSpeed) {
      dynamic_object->dy = -maxSpeed; 
    }
    if (dynamic_object->isRunning && dynamic_object->dy <= -maxRunningSpeed) {
      dynamic_object->dy = -maxRunningSpeed;
    }

  }

  // Clamp
  if (!dynamic_object->isMoving) {
    if (fabs(dynamic_object->dx) < 5) {
      dynamic_object->dx = 0;
      dynamic_object->ax = 0;
      dynamic_object->thrustX = 0;
      dynamic_object->directionX = 0;
      dynamic_object->frictionalForceX = 0;
    }
    if (fabs(dynamic_object->dy) < 5) {
      dynamic_object->dy = 0;
      dynamic_object->ay = 0;
      dynamic_object->thrustY = 0;
      dynamic_object->directionY = 0;
      dynamic_object->frictionalForceY = 0;
    }
  }

  dynamic_object->x += dynamic_object->dx * *dt; 
  dynamic_object->y += dynamic_object->dy * *dt; 
 
  return 0;
}


void renderMan(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect srcRect = { dynamic_object->sprite * dynamic_object->w, dynamic_object->h * dynamic_object->direction, dynamic_object->w, dynamic_object->h};
  SDL_Rect destRect = {x, y, dynamic_object->w, dynamic_object->h};
  dynamic_object->status == IS_RUNNING
    ? SDL_RenderCopy(renderer, dynamic_object->runningTexture, &srcRect, &destRect)
    : SDL_RenderCopy(renderer, dynamic_object->idleTexture, &srcRect, &destRect);
}

void renderTile(Game *game, int x, int y, char tileId) {
  int tileRow;
  int tileColumn;
  if (tileId == '@') {
    tileRow = 4;
    tileColumn = 4;
  } else {
    tileRow = 12;
    tileColumn = 7;
  }
  SDL_Rect srcRect = {tileRow * 16, tileColumn * 16, 16, 16};
  SDL_Rect tileRect = {x + game->scrollX, y + game->scrollY, game->map.tileSize, game->map.tileSize};
  SDL_RenderCopy(game->renderer, game->terrainTexture, &srcRect, &tileRect);
};

void doRender(Game *game) {
  SDL_RenderClear(game->renderer);

  for (int y = -game->scrollY/game->map.tileSize; y < (-game->scrollY + WINDOW_HEIGHT)/ game->map.tileSize; y++)
    for (int x = -game->scrollX/game->map.tileSize; x < (-game->scrollX + WINDOW_WIDTH)/ game->map.tileSize; x++) {
      if (x >= 0 && x < game->map.width && y>= 0 && y < game->map.height) {
        renderTile(game, x * game->map.tileSize, y * game->map.tileSize, game->map.tiles[x + y * game->map.width].tileId);
      }
    }

  for (int i = 0; i < game->map.dynamic_objects_count; i++) {
    renderMan(&game->map.dynamic_objects[i], game->map.dynamic_objects[i].x+game->scrollX, game->map.dynamic_objects[i].y+game->scrollY, game->renderer);

    char* currentDialog = game->map.dynamic_objects[i].currentDialog;
    if (currentDialog != NULL) {
      SDL_Color color = {0, 0, 0};
      renderText(game->renderer, game->font, currentDialog, color, 20, 400, WINDOW_WIDTH - 20, 20);
    }
  }

  SDL_SetRenderDrawColor(game->renderer, 25, 100, 155, 255);

  SDL_RenderPresent(game->renderer);
};

SDL_Texture* initializeTerrain(SDL_Renderer *renderer) {
  SDL_Surface *surface = createSurface("images/terrain.png");
  SDL_Texture *terrainTexture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  return terrainTexture;
}

TTF_Font* initializeFont(char* fileName, int fontSize) {
  TTF_Font *font = TTF_OpenFont(fileName, fontSize);
  if (font == NULL) {
    printf("Could not find font");
    SDL_Quit();
    exit(1);
  }

  return font;
}

void loadMap(Game *game, char* fileName) {
  game->map = initializeMap(fileName, 32);
  for (int i = 0; i < game->map.dynamic_objects_count; i++) {
    game->map.dynamic_objects[i] = initializeMan(game->renderer, &game->map.dynamic_objects[i], UP, 0, 80, 950, 1300, IS_IDLE, RIGHT);
    if (game->map.dynamic_objects[i].isMain) {
      game->mainCharacter = &game->map.dynamic_objects[i];
    }
  }
}

void loadGame(Game *game) {
  game->dt = 1.0f/60.0f;
  game->font = initializeFont("fonts/slkscr.ttf", 18);
  game->scrollX = 0;
  game->scrollY = 0;
  game->dismissDialog = 0;
  game->terrainTexture = initializeTerrain(game->renderer);
  game->status = IS_ACTIVE;
  loadMap(game, "map_03.lvl");
};

// Detect if two objects in space have a collision
int hasCollision(float x1, float y1, float x2, float y2, float w1, float h1, float w2, float h2)
{
  return (!((x1 > (x2+w2)) || (x2 > (x1+w1)) || (y1 > (y2+h2)) || (y2 > (y1+h1))));
}

void detectCollisions(Game *game) {
  for (int y = -game->scrollY/game->map.tileSize; y < (-game->scrollY + WINDOW_HEIGHT)/ game->map.tileSize; y++)
    for (int x = -game->scrollX/game->map.tileSize; x < (-game->scrollX + WINDOW_WIDTH)/ game->map.tileSize; x++) {
    int tileIndex = x + y * game->map.width;
    if (tileIndex < 0) continue;
    float* mainX = &game->mainCharacter->x;
    float* mainY = &game->mainCharacter->y;
    float* mainDx = &game->mainCharacter->dx;
    float* mainDy = &game->mainCharacter->dy;
    int* mainW = &game->mainCharacter->w;
    int* mainH = &game->mainCharacter->h;
    
    float tileX = game->map.tiles[tileIndex].x;
    float tileY = game->map.tiles[tileIndex].y;
    int tileW = game->map.tiles[tileIndex].w;
    int tileH = game->map.tiles[tileIndex].h; 

    int reallyNotOccupied = 0;
    for (int i = 0; i < game->map.dynamic_objects_count; i++) {
      int doIndexX = (game->map.dynamic_objects[i].x + game->map.dynamic_objects[i].w/2)/game->map.tileSize;
      int doIndexY = (game->map.dynamic_objects[i].y + game->map.dynamic_objects[i].h/2)/game->map.tileSize;
      game->map.dynamic_objects[i].currentTile = doIndexX + doIndexY * game->map.width;
      game->map.tiles[game->map.dynamic_objects[i].currentTile].dynamic_object_id = game->map.dynamic_objects[i].id;

      if (!reallyNotOccupied && game->map.tiles[tileIndex].dynamic_object_id && game->map.dynamic_objects[i].currentTile != tileIndex) {
        reallyNotOccupied = 1; 
      } else {
        reallyNotOccupied = 0; 
      }


      if (x >= 0 && x < game->map.width && y>= 0 && y < game->map.height) {
        if (game->map.tiles[tileIndex].tileState == IS_TELEPORT && tileIndex == game->mainCharacter->currentTile) {
            loadMap(game, game->map.tiles[tileIndex].teleportTo);
        }
        if (game->map.tiles[tileIndex].tileState == IS_SOLID || (game->map.dynamic_objects[i].currentTile == tileIndex && !game->map.dynamic_objects[i].isMain)) {

          if (*mainX + *mainW / 2 > tileX && *mainX + *mainW/ 2 < tileX+tileW) {
            if (*mainY < tileH+tileY && *mainY > tileY && *mainDy < 0) {
              *mainY = tileY+tileH;
              *mainDy = 0;
            } 
          }
          
          if (*mainX + *mainW > tileX && *mainX<tileX+tileW) {
            if (*mainY + *mainH > tileY && *mainY < tileY && *mainDy > 0) {
              *mainY = tileY-*mainH;
              *mainDy = 0;
            }
          }

          if (*mainY + *mainH/2 > tileY && *mainY<tileY+tileH) {
            if (*mainX < tileX+tileW && *mainX+*mainW > tileX+tileW && *mainDx < 0) {
              *mainX = tileX + tileW;
              *mainDx = 0;
            } else if (*mainX+*mainW > tileX && *mainX < tileX && *mainDx > 0) {
              *mainX = tileX - *mainW;
              *mainDx = 0;
            }
          }
        }
      }
    }

    if (reallyNotOccupied) { 
      game->map.tiles[tileIndex].dynamic_object_id = 0;
    }
  }
};

float getAngle(Game *game) {
  float angle = atan2(game->mainCharacter->dy, game->mainCharacter->dx) * (180 / M_PI); 
  return angle > 0 ? angle : angle + 360;
};

int getDirection(Game *game) { 
  return (int)fabs(floor((game->mainCharacter->angle < 270 ? game->mainCharacter->angle + 90 : game->mainCharacter->angle - 270)/45));
};

void triggerDialog(Game *game) { 
  DynamicObject *townsperson = NULL;
  if (game->mainCharacter->direction == UP && game->map.tiles[game->mainCharacter->currentTile - game->map.width].dynamic_object_id) {
    townsperson = getDynamicObjectFromMap(&game->map, game->map.tiles[game->mainCharacter->currentTile - game->map.width].dynamic_object_id);
  } else if (game->mainCharacter->direction == LEFT && game->map.tiles[game->mainCharacter->currentTile - 1].dynamic_object_id) {
    townsperson = getDynamicObjectFromMap(&game->map, game->map.tiles[game->mainCharacter->currentTile - 1].dynamic_object_id);
  } else if (game->mainCharacter->direction == DOWN && game->map.tiles[game->mainCharacter->currentTile + game->map.width].dynamic_object_id) {
    townsperson = getDynamicObjectFromMap(&game->map, game->map.tiles[game->mainCharacter->currentTile + game->map.width].dynamic_object_id);
  } else if (game->mainCharacter->direction == RIGHT && game->map.tiles[game->mainCharacter->currentTile + 1].dynamic_object_id) {
    townsperson = getDynamicObjectFromMap(&game->map, game->map.tiles[game->mainCharacter->currentTile + 1].dynamic_object_id);
  }

  if (townsperson != NULL) {
    game->status = IS_DIALOGUE;
    addAction(0, townsperson, (void*)&speak, "...I sort of wish you hadn't, I was enjoying the quiet..", (void*)&game->dismissDialog, 0);
    addAction(1, townsperson, (void*)&speak, "Oh wow, you finally figured out how to talk to me. Very nice!", (void*)&game->dismissDialog, 0);
  }
}

void process(Game *game) {
  game->time++;
  if (!strncmp(game->map.name, "map_03.lvl", 12)) { 
    if (game->mainCharacter->currentTile == 20 && game->status != IS_CUTSCENE) {
      game->status = IS_CUTSCENE;
      addAction(0, game->mainCharacter, (void*)&moveDown, (void*)1, (void*)&game->map.tileSize, NULL);
      addAction(1, game->mainCharacter, (void*)&speak, "You probably shouldn't try to step here again.", (void*)&game->dismissDialog, 0);
      addAction(2, game->mainCharacter, (void*)&speak, "Yikes, you just stepped in some rotten milk!", (void*)&game->dismissDialog, 0);
    }
  }
      
  int running = 0;

  for (int i = 0; i < game->map.dynamic_objects_count; i++) {
    if (game->map.dynamic_objects[i].default_behavior == WALKING && game->map.dynamic_objects[i].actionSize == 0 && !game->map.dynamic_objects[i].isMain && game->status == IS_ACTIVE) {
      if (fmod(game->time, 240) == 0) {
        printf("hello\n");
        fflush(stdout);
        int randomNumber = rand() % 4;

        if (randomNumber == 0) {
          addAction(0, &game->map.dynamic_objects[i], (void*)&moveUp, (void*)1, (void*)&game->map.tileSize, NULL);
        } else if (randomNumber == 1) {
          addAction(0, &game->map.dynamic_objects[i], (void*)&moveRight, (void*)1, (void*)&game->map.tileSize, NULL);
        } else if (randomNumber == 2) {
          addAction(0, &game->map.dynamic_objects[i], (void*)&moveDown, (void*)1, (void*)&game->map.tileSize, NULL);
        } else if (randomNumber == 3) {
          addAction(0, &game->map.dynamic_objects[i], (void*)&moveLeft, (void*)1, (void*)&game->map.tileSize, NULL);
        }
      }
    }

    if (game->map.dynamic_objects[i].actionSize > 0) {
      game->map.dynamic_objects[i].prevActionSize = game->map.dynamic_objects[i].actionSize;
      running = executeAction(&game->map.dynamic_objects[i].actions[game->map.dynamic_objects[i].actionSize-1], &game->map.dynamic_objects[i]);
    }
    if (!running && game->map.dynamic_objects[i].actionSize > 0) {
      game->map.dynamic_objects[i].actions = removeAction((void*)game->map.dynamic_objects[i].actions, game->map.dynamic_objects[i].actionSize-1, &game->map.dynamic_objects[i].actionSize);
      running = 1;
    } 

    if (!running && game->map.dynamic_objects[i].prevActionSize > game->map.dynamic_objects[i].actionSize && game->map.dynamic_objects[i].actionSize == 0) {
      if (game->status == IS_DIALOGUE || game->status == IS_CUTSCENE) { 
        game->status = IS_ACTIVE;
        game->map.dynamic_objects[i].prevActionSize = 0;
      }
    }
  }

  if (game->status == IS_ACTIVE || game->status == IS_DIALOGUE) {
    for (int i = 0; i < game->map.dynamic_objects_count; i++) {
      handlePhysics(&game->map.dynamic_objects[i], &game->dt);
    }
  }

  game->scrollX = -game->mainCharacter->x+WINDOW_WIDTH/2;
  game->scrollY = -game->mainCharacter->y+WINDOW_HEIGHT/2;

  if (game->mainCharacter->x < 0) {
    game->mainCharacter->x = 0;
  }

  if (game->mainCharacter->x > game->map.width * game->map.tileSize - game->mainCharacter->w) {
    game->mainCharacter->x = game->map.width * game->map.tileSize - game->mainCharacter->w;
  }

  if (game->mainCharacter->y < 0) {
    game->mainCharacter->y = 0;
  }

  if (game->mainCharacter->y > game->map.height * game->map.tileSize - game->mainCharacter->h) {
    game->mainCharacter->y = game->map.height * game->map.tileSize - game->mainCharacter->h;
  }

  if(game->scrollX > 0) {
    game->scrollX = 0;
  }
  if(game->scrollX < -game->map.width * game->map.tileSize+WINDOW_WIDTH) {
    game->scrollX = -game->map.width * game->map.tileSize+WINDOW_WIDTH;
  }

  if(game->scrollY > 0) {
    game->scrollY = 0;
  }

  if(game->scrollY < -game->map.height * game->map.tileSize+WINDOW_HEIGHT) {
    game->scrollY = -game->map.height * game->map.tileSize+WINDOW_HEIGHT;
  }

  // handle animation
  if (game->mainCharacter->dx != 0 || game->mainCharacter->dy != 0) { 
    game->mainCharacter->angle = getAngle(game);
    game->mainCharacter->direction = getDirection(game);
  }
  game->mainCharacter->status = game->mainCharacter->dx != 0 || game->mainCharacter->dy != 0 ? IS_RUNNING : IS_IDLE;

  // 60 FPS / 8 animations 
  if (fmod(game->time, 7.5) == 0) {
    for (int i = 0; i < game->map.dynamic_objects_count; i++) {
      game->map.dynamic_objects[i].sprite = (game->map.dynamic_objects[i].sprite + 1) % 8;
    }
  }
   
  detectCollisions(game);
};

void shutdownGame(Game *game) {
  SDL_DestroyTexture(game->terrainTexture);
  for (int i = 0; i < game->map.dynamic_objects_count; i++) {
    SDL_DestroyTexture(game->map.dynamic_objects[i].idleTexture);
    SDL_DestroyTexture(game->map.dynamic_objects[i].runningTexture);
    free(game->map.dynamic_objects[i].actions);
  }
  TTF_CloseFont(game->font);
  SDL_DestroyWindow(game->window); 

  TTF_Quit();
  SDL_Quit(); 
};

int main(int argc, char *argv[]) {
  srand(time(NULL));

  TTF_Init();

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
    return 1;
  }

  Game game;
  // Create an application window with the following settings:
  game.window = SDL_CreateWindow( 
      "RPG",
      SDL_WINDOWPOS_UNDEFINED,           
      SDL_WINDOWPOS_UNDEFINED,           
      WINDOW_WIDTH,                               
      WINDOW_HEIGHT,                               
      SDL_WINDOW_SHOWN
      );

  game.gravity = 9.8 * PIXELS_PER_METER;

  if (game.window == NULL) {
    fprintf(stderr, "could not create window: %s\n", SDL_GetError());
    return 1;
  }

  game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED);
  if (game.renderer == NULL) {
    fprintf(stderr, "could not create renderer: %s\n", SDL_GetError());
    return 1;
  }

  loadGame(&game);

  int done = 0;
  while (!done) {
    done = handleEvents(&game);
    process(&game);
    doRender(&game);

    SDL_Delay(10);
  }

  shutdownGame(&game);

  return 0;
}
