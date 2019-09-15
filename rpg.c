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
          case SDL_SCANCODE_RETURN:
            game->dismissDialog = 1;
            break;
          case SDL_SCANCODE_A:
            if (!game->mainCharacter->triggerDialog) {
              game->mainCharacter->triggerDialog = 1;
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

int handlePhysics(Man *character, float *dt) {
  float cof = 0.41; // This will be attached to the tile the player is on eventually
  if (character->isMoving) {
    character->frictionalForceX = cof * character->normalForce;
    character->frictionalForceY = cof * character->normalForce;
  }
  if (character->moveLeft) {
    character->directionX = -1;
    character->thrustX = character->isRunning ? character->runThrust : character->walkThrust;
    if (character->moveRight) {
      character->directionX = 0;
    }
  } else if (!character->moveLeft) {
    character->directionX = character->dx < 0 ? 1 : 0;
    if (character->moveRight) {
      character->directionX = 1;
    } else {
      character->thrustX *= -1;
    }
  }
  if (character->moveRight) {
    character->directionX = 1;
    character->thrustX = character->isRunning ? character->runThrust : character->walkThrust;
    if (character->moveLeft) {
      character->directionX = 0;
    }
  } else if (!character->moveRight) {
    character->directionX = character->dx > 0 ? -1 : 0;
    character->directionX = character->dx < 0 ? 1 : character->directionX;
    if (character->moveLeft) {
      character->directionX = -1;
    } else {
      character->thrustX *= -1;
    }
  }
  if (character->moveUp) {
    character->directionY = -1;
    character->thrustY = character->isRunning ? character->runThrust : character->walkThrust; 
    if (character->moveDown) {
      character->directionY = 0;
    }
  } else if (!character->moveUp) {
    character->directionY = character->dy < 0 ? 1 : 0;
    if (character->moveDown) {
      character->directionY = 1;
    } else {
      character->thrustY *= -1;
    }
  }
  if (character->moveDown) {
    character->directionY = 1;
    character->thrustY = character->isRunning ? character->runThrust : character->walkThrust; 
    if (character->moveUp) {
      character->directionY = 0;
    }
  } else if (!character->moveDown) {
    character->directionY = character->dy > 0 ? -1 : 0;
    character->directionY = character->dy < 0 ? 1 : character->directionY;
    if (character->moveUp) {
      character->directionY = -1;
    } else {
      character->thrustY *= -1;
    }
  }

  character->ax = (((character->directionX * character->thrustX) - (character->directionX * character->frictionalForceX)) / character->mass);
  character->ay = (((character->directionY * character->thrustY) - (character->directionY * character->frictionalForceY)) / character->mass);
  character->dx = accelerate(character->dx, character->ax, *dt); 
  character->dy = accelerate(character->dy, character->ay, *dt); 
  
  float maxSpeed = 60.0f;
  float maxRunningSpeed = 120.0f;
  
  if (character->isMoving) {
    if (!character->isRunning && character->dx >= maxSpeed) {
      character->dx = maxSpeed; 
    }
    if (character->isRunning && character->dx >= maxRunningSpeed) {
      character->dx = maxRunningSpeed;
    }
    if (!character->isRunning && character->dx <= -maxSpeed) {
      character->dx = -maxSpeed; 
    }
    if (character->isRunning && character->dx <= -maxRunningSpeed) {
      character->dx = -maxRunningSpeed;
    }

    if (!character->isRunning && character->dy >= maxSpeed) {
      character->dy = maxSpeed; 
    }
    if (character->isRunning && character->dy >= maxRunningSpeed) {
      character->dy = maxRunningSpeed;
    }
    if (!character->isRunning && character->dy <= -maxSpeed) {
      character->dy = -maxSpeed; 
    }
    if (character->isRunning && character->dy <= -maxRunningSpeed) {
      character->dy = -maxRunningSpeed;
    }

  }

  // Clamp
  if (!character->isMoving) {
    if (fabs(character->dx) < 5) {
      character->dx = 0;
      character->ax = 0;
      character->thrustX = 0;
      character->directionX = 0;
      character->frictionalForceX = 0;
    }
    if (fabs(character->dy) < 5) {
      character->dy = 0;
      character->ay = 0;
      character->thrustY = 0;
      character->directionY = 0;
      character->frictionalForceY = 0;
    }
  }

  character->x += character->dx * *dt; 
  character->y += character->dy * *dt; 
 
  return 0;
}


void renderMan(Man *man, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect manSrcRect = { man->sprite * man->w, man->h * man->direction, man->w, man->h};
  SDL_Rect manRect = {x, y, man->w, man->h};
  man->status == IS_RUNNING
    ? SDL_RenderCopy(renderer, man->runningTexture, &manSrcRect, &manRect)
    : SDL_RenderCopy(renderer, man->idleTexture, &manSrcRect, &manRect);
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

    char* currentDialog = game->map.characters[i].currentDialog;
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
  for (int i = 0; i < game->map.characterCount; i++) {
    game->map.characters[i] = initializeMan(game->renderer, &game->map.characters[i], UP, 0, 80, 950, 1300, IS_IDLE, RIGHT);
    if (game->map.characters[i].isMain) {
      game->mainCharacter = &game->map.characters[i];
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

    int reallyNotOccupied = 0;
    for (int i = 0; i < game->map.characterCount; i++) {
      int characterIndexX = (game->map.characters[i].x + game->map.characters[i].w/2)/game->map.tileSize;
      int characterIndexY = (game->map.characters[i].y + game->map.characters[i].h/2)/game->map.tileSize;
      game->map.characters[i].currentTile = characterIndexX + characterIndexY * game->map.width;
      game->map.tiles[game->map.characters[i].currentTile].characterId = game->map.characters[i].id;

      if (!reallyNotOccupied && game->map.tiles[tileIndex].characterId && game->map.characters[i].currentTile != tileIndex) {
        reallyNotOccupied = 1; 
      } else {
        reallyNotOccupied = 0; 
      }


      if (x >= 0 && x < game->map.width && y>= 0 && y < game->map.height) {
        if (game->map.tiles[tileIndex].tileState == IS_TELEPORT && tileIndex == game->mainCharacter->currentTile) {
            loadMap(game, game->map.tiles[tileIndex].teleportTo);
        }
        if (game->map.tiles[tileIndex].tileState == IS_SOLID || (game->map.characters[i].currentTile == tileIndex && !game->map.characters[i].isMain)) {

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

    if (reallyNotOccupied) { 
      game->map.tiles[tileIndex].characterId = 0;
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

void process(Game *game) {
  game->time++;
  if (!strncmp(game->map.name, "map_03.lvl", 12)) { 

    /* if (townsperson->startCutscene) { */
    /*   townsperson = getCharacterFromMap(&game->map, "001"); */
    /*   addAction(0, townsperson, (void*)&moveLeft, (void*)2, (void*)&game->map.tileSize, NULL); */
    /*   addAction(1, townsperson, (void*)&moveRight, (void*)2, (void*)&game->map.tileSize, NULL); */
    /*   addAction(2, townsperson, (void*)&speak, "I am very proud of you.", (void*)&game->dismissDialog, 0); */
    /*   addAction(3, townsperson, (void*)&speak, "You did it!", (void*)&game->dismissDialog, 0); */
    /*   townsperson->startCutscene = 0; */
    /* } */

    // If man is facing the correct direction and "startDialog" is set, register the action
    if (game->mainCharacter->triggerDialog) {
      if (game->mainCharacter->direction == UP && game->map.tiles[game->mainCharacter->currentTile - game->map.width].characterId) {
        Man* townsperson = getCharacterFromMap(&game->map, game->map.tiles[game->mainCharacter->currentTile - game->map.width].characterId);
        addAction(0, townsperson, (void*)&speak, "...I sort of wish you hadn't, I was enjoying the quiet..", (void*)&game->dismissDialog, 0);
        addAction(1, townsperson, (void*)&speak, "Oh wow, you finally figured out how to talk to me. Very nice!", (void*)&game->dismissDialog, 0);
        game->mainCharacter->triggerDialog = 0; 
      }

      if (game->mainCharacter->direction == LEFT && game->map.tiles[game->mainCharacter->currentTile - 1].characterId) {
        Man *townsperson = getCharacterFromMap(&game->map, game->map.tiles[game->mainCharacter->currentTile - 1].characterId);
        addAction(0, townsperson, (void*)&speak, "...I sort of wish you hadn't, I was enjoying the quiet..", (void*)&game->dismissDialog, 0);
        addAction(1, townsperson, (void*)&speak, "Oh wow, you finally figured out how to talk to me. Very nice!", (void*)&game->dismissDialog, 0);
        game->mainCharacter->triggerDialog = 0; 
      } else if (game->mainCharacter->direction == DOWN && game->map.tiles[game->mainCharacter->currentTile + game->map.width].characterId) {
        Man *townsperson = getCharacterFromMap(&game->map, game->map.tiles[game->mainCharacter->currentTile + game->map.width].characterId);
        addAction(0, townsperson, (void*)&speak, "...I sort of wish you hadn't, I was enjoying the quiet..", (void*)&game->dismissDialog, 0);
        addAction(1, townsperson, (void*)&speak, "Oh wow, you finally figured out how to talk to me. Very nice!", (void*)&game->dismissDialog, 0);
        game->mainCharacter->triggerDialog = 0; 
      } else if (game->mainCharacter->direction == RIGHT && game->map.tiles[game->mainCharacter->currentTile + 1].characterId) {
        Man *townsperson = getCharacterFromMap(&game->map, game->map.tiles[game->mainCharacter->currentTile + 1].characterId);
        addAction(0, townsperson, (void*)&speak, "...I sort of wish you hadn't, I was enjoying the quiet..", (void*)&game->dismissDialog, 0);
        addAction(1, townsperson, (void*)&speak, "Oh wow, you finally figured out how to talk to me. Very nice!", (void*)&game->dismissDialog, 0);
        game->mainCharacter->triggerDialog = 0; 
      } else {
        game->mainCharacter->triggerDialog = 0; 
      }
    }
      
    int running = 0;


    for (int i = 0; i < game->map.characterCount; i++) {
      if (game->map.characters[i].actionSize > 0) {
        running = executeAction(&game->map.characters[i].actions[game->map.characters[i].actionSize-1], &game->map.characters[i]);
      }
      if (!running && game->map.characters[i].actionSize > 0) {
        game->map.characters[i].actions = removeAction((void*)game->map.characters[i].actions, game->map.characters[i].actionSize-1, &game->map.characters[i].actionSize);
        running = 1;
      }
    }
  }

  if (game->status == IS_ACTIVE) {
    for (int i = 0; i < game->map.characterCount; i++) {
      handlePhysics(&game->map.characters[i], &game->dt);
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
  game->mainCharacter->angle = getAngle(game);
  game->mainCharacter->direction = getDirection(game);
  game->mainCharacter->status = game->mainCharacter->dx != 0 || game->mainCharacter->dy != 0 ? IS_RUNNING : IS_IDLE;

  // 60 FPS / 8 animations 
  if (fmod(game->time, 7.5) == 0) {
    for (int i = 0; i < game->map.characterCount; i++) {
      game->map.characters[i].sprite = (game->map.characters[i].sprite + 1) % 8;
    }
  }
   
  detectCollisions(game);
};

void shutdownGame(Game *game) {
  SDL_DestroyTexture(game->terrainTexture);
  for (int i = 0; i < game->map.characterCount; i++) {
    SDL_DestroyTexture(game->map.characters[i].idleTexture);
    SDL_DestroyTexture(game->map.characters[i].runningTexture);
    free(game->map.characters[i].actions);
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
    process(&game);
    doRender(&game);

    SDL_Delay(10);
  }

  shutdownGame(&game);

  return 0;
}
