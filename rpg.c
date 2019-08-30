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

    printf("%d\n", game->mainCharacter->moveLeft);
    printf("%d\n", game->mainCharacter->moveRight);
    printf("%d\n", game->mainCharacter->moveUp);
    printf("%d\n", game->mainCharacter->moveDown);
    fflush(stdout);

    // Clamp
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

int handlePhysics(Game *game) {
  if (game->status == IS_ACTIVE) {
    float cof = 0.41; // This will be attached to the tile the player is on eventually
    for (int i = 0; i < game->map.characterCount; i++) {
      if (game->map.characters[i].isMoving) {
        game->map.characters[i].frictionalForceX = cof * game->map.characters[i].normalForce;
        game->map.characters[i].frictionalForceY = cof * game->map.characters[i].normalForce;
      } 
      if (game->map.characters[i].moveLeft) {
        game->map.characters[i].directionX = -1;
        game->map.characters[i].thrustX = game->map.characters[i].isRunning ? game->map.characters[i].runThrust : game->map.characters[i].walkThrust;
        if (game->map.characters[i].moveRight) {
          game->map.characters[i].directionX = 0;
        }
      } else if (!game->map.characters[i].moveLeft) {
        game->map.characters[i].directionX = game->map.characters[i].dx < 0 ? 1 : 0;
        if (game->map.characters[i].moveRight) {
          game->map.characters[i].directionX = 1;
        } else {
          game->map.characters[i].thrustX *= -1;
        }
      }
      if (game->map.characters[i].moveRight) {
        game->map.characters[i].directionX = 1;
        game->map.characters[i].thrustX = game->map.characters[i].isRunning ? game->map.characters[i].runThrust : game->map.characters[i].walkThrust;
        if (game->map.characters[i].moveLeft) {
          game->map.characters[i].directionX = 0;
        }
      } else if (!game->map.characters[i].moveRight) {
        game->map.characters[i].directionX = game->map.characters[i].dx > 0 ? -1 : 0;
        game->map.characters[i].directionX = game->map.characters[i].dx < 0 ? 1 : game->map.characters[i].directionX;
        if (game->map.characters[i].moveLeft) {
          game->map.characters[i].directionX = -1;
        } else {
          game->map.characters[i].thrustX *= -1;
        }
      }
      if (game->map.characters[i].moveUp) {
        game->map.characters[i].directionY = -1;
        game->map.characters[i].thrustY = game->map.characters[i].isRunning ? game->map.characters[i].runThrust : game->map.characters[i].walkThrust; 
        if (game->map.characters[i].moveDown) {
          game->map.characters[i].directionY = 0;
        }
      } else if (!game->map.characters[i].moveUp) {
        game->map.characters[i].directionY = game->map.characters[i].dy < 0 ? 1 : 0;
        if (game->map.characters[i].moveDown) {
          game->map.characters[i].directionY = 1;
        } else {
          game->map.characters[i].thrustY *= -1;
        }
      }
      if (game->map.characters[i].moveDown) {
        game->map.characters[i].directionY = 1;
        game->map.characters[i].thrustY = game->map.characters[i].isRunning ? game->map.characters[i].runThrust : game->map.characters[i].walkThrust; 
        if (game->map.characters[i].moveUp) {
          game->map.characters[i].directionY = 0;
        }
      } else if (!game->map.characters[i].moveDown) {
        game->map.characters[i].directionY = game->map.characters[i].dy > 0 ? -1 : 0;
        game->map.characters[i].directionY = game->map.characters[i].dy < 0 ? 1 : game->map.characters[i].directionY;
        if (game->map.characters[i].moveUp) {
          game->map.characters[i].directionY = -1;
        } else {
          game->map.characters[i].thrustY *= -1;
        }
      }

      if (game->map.characters[i].frictionalForceX > game->map.characters[i].thrustX) {
        game->map.characters[i].frictionalForceX = game->map.characters[i].thrustX;
      }

      if (game->map.characters[i].frictionalForceY > game->map.characters[i].thrustY) {
        game->map.characters[i].frictionalForceY = game->map.characters[i].thrustY;
      }

      game->map.characters[i].ax = (((game->map.characters[i].directionX * game->map.characters[i].thrustX) - (game->map.characters[i].directionX * game->map.characters[i].frictionalForceX)) / game->map.characters[i].mass);
      game->map.characters[i].ay = (((game->map.characters[i].directionY * game->map.characters[i].thrustY) - (game->map.characters[i].directionY * game->map.characters[i].frictionalForceY)) / game->map.characters[i].mass);
      game->map.characters[i].dx = accelerate(game->map.characters[i].dx, game->map.characters[i].ax, game->dt); 
      game->map.characters[i].dy = accelerate(game->map.characters[i].dy, game->map.characters[i].ay, game->dt); 

      printf("dx %f\n", game->map.characters[0].dx);
      printf("dy %f\n", game->map.characters[0].dy);
      printf("ax %f\n", game->map.characters[0].ax);
      printf("ay %f\n", game->map.characters[0].ay);
      printf("ff %d\n", game->map.characters[0].frictionalForceX);
      printf("ffY %d\n", game->map.characters[0].frictionalForceY);

      // Clamp
      if (!game->map.characters[i].isMoving) {
        if (fabs(game->map.characters[i].dx) < 2) {
          game->map.characters[i].dx = 0;
          game->map.characters[i].ax = 0;
          game->map.characters[i].thrustX = 0;
          game->map.characters[i].directionX = 0;
        }
        if (fabs(game->map.characters[i].dy) < 2) {
          game->map.characters[i].dy = 0;
          game->map.characters[i].ay = 0;
          game->map.characters[i].thrustY = 0;
          game->map.characters[i].directionY = 0;
        }
      }
    }
  }
 
  return 0;
}


void renderMan(Man *man, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect manSrcRect = { man->sprite * man->w, man->h * man->direction, man->w, man->h};
  SDL_Rect manRect = {x, y, man->w, man->h};
  man->status == IS_RUNNING
    ? SDL_RenderCopy(renderer, man->runningTexture, &manSrcRect, &manRect)
    : SDL_RenderCopy(renderer, man->idleTexture, &manSrcRect, &manRect);
}

void renderText(Game *game, char* text, SDL_Color color, int x, int y, int w, int h) { 
  SDL_Rect textRect = {x, y, w, h};
  SDL_Surface *surface = TTF_RenderText_Solid(game->text.font, text, color);
  game->text.texture = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_RenderCopy(game->renderer, game->text.texture, NULL, &textRect);
  SDL_FreeSurface(surface);
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

  for (int i = 0; i < game->map.characterCount; i++) {
    renderMan(&game->map.characters[i], game->map.characters[i].x+game->scrollX, game->map.characters[i].y+game->scrollY, game->renderer);
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
  for (int i = 0; i < game->map.characterCount; i++) {
    game->map.characters[i] = initializeMan(game->renderer, &game->map.characters[i], UP, 0, 80, 950, 2400, IS_IDLE, RIGHT);
    if (game->map.characters[i].isMain) {
      game->mainCharacter = &game->map.characters[i];
    }
  }
}

void loadGame(Game *game) {
  game->dt = 1.0f/60.0f;
  game->text.font = initializeFont("fonts/slkscr.ttf", 48);
  game->scrollX = 0;
  game->scrollY = 0;
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
    float* manX = &game->mainCharacter->x;
    float* manY = &game->mainCharacter->y;
    float* manDx = &game->mainCharacter->dx;
    float* manDy = &game->mainCharacter->dy;
    int* manW = &game->mainCharacter->w;
    int* manH = &game->mainCharacter->h;
    
    float floorX = game->map.tiles[tileIndex].x;
    float floorY = game->map.tiles[tileIndex].y;
    int floorW = game->map.tiles[tileIndex].w;
    int floorH = game->map.tiles[tileIndex].h; 

    for (int i = 0; i < game->map.characterCount; i++) {
      int characterIndexX = (game->map.characters[i].x + game->map.characters[i].w/2)/game->map.tileSize;
      int characterIndexY = (game->map.characters[i].y + game->map.characters[i].h/2)/game->map.tileSize;
      game->map.characters[i].currentTile = characterIndexX + characterIndexY * game->map.width;
      if (!game->map.characters[i].isMain) {
        if (game->map.tiles[tileIndex].isOccupied && tileIndex != game->map.characters[i].currentTile) {
          printf("tileIndex %d", tileIndex);
          fflush(stdout);
          game->map.tiles[tileIndex].isOccupied = 0;
        }
        if (tileIndex == game->map.characters[i].currentTile) {
          game->map.tiles[tileIndex].isOccupied = 1;
        }
      } 
    }


    if (x >= 0 && x < game->map.width && y>= 0 && y < game->map.height) {
      if (game->map.tiles[tileIndex].tileState == IS_TELEPORT) {
        if (tileIndex == game->mainCharacter->currentTile) {
          loadMap(game, game->map.tiles[tileIndex].teleportTo);
        }
      }
      if (game->map.tiles[tileIndex].tileState == IS_SOLID || game->map.tiles[tileIndex].isOccupied == 1) {

        if (*manX + *manW / 2 > floorX && *manX + *manW/ 2 < floorX+floorW) {
          if (*manY < floorH+floorY && *manY > floorY && *manDy < 0) {
            *manY = floorY+floorH;
            *manDy = 0;
          } 
        }
        
        if (*manX + *manW > floorX && *manX<floorX+floorW) {
          if (*manY + *manH > floorY && *manY < floorY && *manDy > 0) {
            *manY = floorY-*manH;
            *manDy = 0;
          }
        }

        if (*manY + *manH/2 > floorY && *manY<floorY+floorH) {
          if (*manX < floorX+floorW && *manX+*manW > floorX+floorW && *manDx < 0) {
            *manX = floorX + floorW;
            *manDx = 0;
          } else if (*manX+*manW > floorX && *manX < floorX && *manDx > 0) {
            *manX = floorX - *manW;
            *manDx = 0;
          }
        }
      }
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

Man* getCharacter(Game *game, char* id) {
  for (int i = 0; i < game->map.characterCount; i++) {
    if (!strncmp(game->map.characters[i].id, id, sizeof game->map.characters[i].id)) {
      return &game->map.characters[i];
    }
  }

  printf("Could not find character with id %s", id);
  SDL_Quit();
  exit(1);
}

void moveLeft(Game *game, Man *man, int tileDistance) {
  if (man->totalMovedX >= -(tileDistance * game->map.tileSize)) {
    man->isMoving = 1;
    man->moveLeft = 1;
    man->totalMovedX += man->dx * game->dt;
    printf("left left %d\n", abs(man->currentTile - man->startingTile));
    if (abs(man->currentTile - man->startingTile) >= tileDistance) {
      man->isMoving = 0;
      man->moveLeft = 0;
      man->totalMovedX = 0;
    }
  }
}

void moveRight(Game *game, Man *man, int tileDistance) {
  printf("totalmovedX in moveRight is %f\n", man->totalMovedX);
  if (man->totalMovedX <= tileDistance * game->map.tileSize) {
    man->isMoving = 1;
    man->moveRight = 1;
    man->totalMovedX += man->dx * game->dt;
    printf(" right right %d\n", abs(man->currentTile - man->startingTile));
    if (abs(man->currentTile - man->startingTile) >= tileDistance) {
      man->isMoving = 0;
      man->moveRight = 0;
      man->totalMovedX = 0;
    }
  } 
}

void process(Game *game) {
  game->time++;

  if (!strncmp(game->map.name, "map_003.lvl", sizeof *game->map.name)) { 
    Man* townsperson = getCharacter(game, "001");
    printf("current %d\n", townsperson->currentTile);
    printf("start %d\n", townsperson->startingTile);
    fflush(stdout);
    moveLeft(game, townsperson, 2);
  }

  for (int i = 0; i < game->map.characterCount; i++) {
    game->map.characters[i].x += game->map.characters[i].dx * game->dt; 
    game->map.characters[i].y += game->map.characters[i].dy * game->dt;
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
  game->mainCharacter->angle = getAngle(game);
  game->mainCharacter->direction = getDirection(game);
  game->mainCharacter->status = game->mainCharacter->dx != 0 || game->mainCharacter->dy != 0 ? IS_RUNNING : IS_IDLE;

  // 60 FPS / 8 animations 
  if (fmod(game->time, 7.5) == 0) {
    game->mainCharacter->sprite = (game->mainCharacter->sprite + 1) % 8;
  }
   
  detectCollisions(game);
};

void shutdownGame(Game *game) {
  SDL_DestroyTexture(game->terrainTexture);
  for (int i = 0; i < game->map.characterCount; i++) {
    SDL_DestroyTexture(game->map.characters[i].idleTexture);
    SDL_DestroyTexture(game->map.characters[i].runningTexture);
  }
  TTF_CloseFont(game->text.font);
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
      "Man Jump",
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
    handlePhysics(&game);
    process(&game);
    doRender(&game);

    SDL_Delay(10);
  }

  shutdownGame(&game);

  return 0;
}
