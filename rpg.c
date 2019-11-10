#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include "rpg.h"
#include "physics.h"

void togglePauseState(Game *game) {
  if (game->status == IS_PAUSED) {
    game->status = IS_ACTIVE;
  } else {
    game->status = IS_PAUSED;
  }
}

int handleEvents(Game *game) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        return 1;
        break;
      case SDL_KEYDOWN:
        switch (event.key.keysym.scancode) {
          case SDL_SCANCODE_ESCAPE:
            togglePauseState(game);
            break;
          case SDL_SCANCODE_A:
            if (game->mainCharacter->has_object) { 
              triggerDrop(game);
            } else if (game->status != IS_CUTSCENE && game->status != IS_DIALOGUE) {
              game->mainCharacter->isLifting = 1;
              triggerDialog(game);
              game->dismissDialog = 0;
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
      if (state[SDL_SCANCODE_A]) {
        game->mainCharacter->isPushing = 1;
      } else {
        game->mainCharacter->isPushing = 0;
      }
    } 
    if (state[SDL_SCANCODE_A]) {
      if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_DOWN]) {
        game->mainCharacter->isPushing = 1;
      } else {
        game->mainCharacter->isPushing = 0;
      }
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

    if (!state[SDL_SCANCODE_UP] && !state[SDL_SCANCODE_DOWN]) {
      game->mainCharacter->moveUp = 0;
      game->mainCharacter->moveDown = 0;
    }

    if (!state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT]) {
      game->mainCharacter->moveLeft = 0;
      game->mainCharacter->moveRight = 0;
    }

    if (!state[SDL_SCANCODE_UP] && !state[SDL_SCANCODE_DOWN] && !state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT]) {
      game->mainCharacter->isMoving = 0;
      game->mainCharacter->isPushing = 0;
      game->mainCharacter->isRunning = 0;
      game->mainCharacter->moveLeft = 0;
      game->mainCharacter->moveRight = 0;
      game->mainCharacter->moveUp = 0;
      game->mainCharacter->moveDown = 0;
    }
  }
 
  return 0;
}

int handlePhysics(DynamicObject *dynamic_object, Tile *currentTile, float *dt, Game *game) {
  if (dynamic_object->dx || dynamic_object->dy) {
    dynamic_object->frictionalForceX = currentTile->cof * dynamic_object->normalForce;
    dynamic_object->frictionalForceY = currentTile->cof * dynamic_object->normalForce;
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
      dynamic_object->thrustX = 0;
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
      dynamic_object->thrustX = 0;
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
      dynamic_object->thrustY = 0;
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
      dynamic_object->thrustY = 0;
    }
  }

  float prevDx = dynamic_object->dx;
  float prevDy = dynamic_object->dy;
  if (dynamic_object->moveLeft || dynamic_object->moveRight) {
    dynamic_object->ax = ((dynamic_object->directionX * dynamic_object->thrustX) - (dynamic_object->directionX * dynamic_object->frictionalForceX)) / dynamic_object->mass;
  } else {
    dynamic_object->ax = ((dynamic_object->directionX * dynamic_object->thrustX) + (dynamic_object->directionX * dynamic_object->frictionalForceX)) / dynamic_object->mass;
  }
  if (dynamic_object->moveUp || dynamic_object->moveDown) {
    dynamic_object->ay = ((dynamic_object->directionY * dynamic_object->thrustY) - (dynamic_object->directionY * dynamic_object->frictionalForceY)) / dynamic_object->mass;
  } else {
    dynamic_object->ay = ((dynamic_object->directionY * dynamic_object->thrustY) + (dynamic_object->directionY * dynamic_object->frictionalForceY)) / dynamic_object->mass;
  }
  dynamic_object->dx = accelerate(dynamic_object->dx, dynamic_object->ax, *dt); 
  dynamic_object->dy = accelerate(dynamic_object->dy, dynamic_object->ay, *dt); 

  
  if (dynamic_object->isMoving) {
    if (!dynamic_object->isRunning && dynamic_object->dx >= currentTile->maxSpeed) {
      dynamic_object->dx = currentTile->maxSpeed; 
    }
    if (dynamic_object->isRunning && dynamic_object->dx >= currentTile->maxRunningSpeed) {
      dynamic_object->dx = currentTile->maxRunningSpeed;
    }
    if (!dynamic_object->isRunning && dynamic_object->dx <= -currentTile->maxSpeed) {
      dynamic_object->dx = -currentTile->maxSpeed; 
    }
    if (dynamic_object->isRunning && dynamic_object->dx <= -currentTile->maxRunningSpeed) {
      dynamic_object->dx = -currentTile->maxRunningSpeed;
    }

    if (!dynamic_object->isRunning && dynamic_object->dy >= currentTile->maxSpeed) {
      dynamic_object->dy = currentTile->maxSpeed; 
    }
    if (dynamic_object->isRunning && dynamic_object->dy >=currentTile->maxRunningSpeed) {
      dynamic_object->dy = currentTile->maxRunningSpeed;
    }
    if (!dynamic_object->isRunning && dynamic_object->dy <= -currentTile->maxSpeed) {
      dynamic_object->dy = -currentTile->maxSpeed; 
    }
    if (dynamic_object->isRunning && dynamic_object->dy <= -currentTile->maxRunningSpeed) {
      dynamic_object->dy = -currentTile->maxRunningSpeed;
    }
  }

  /* if (dynamic_object->isMain) { */
  /*   printf("what is ax %f \n", dynamic_object->ax); */
  /*   printf("what is ay %f \n", dynamic_object->ay); */
  /*   printf("what is thrustX %f \n", dynamic_object->thrustX); */
  /*   printf("what is thrustY %f \n", dynamic_object->thrustY); */
  /*   printf("what is frictionalX %f \n", dynamic_object->frictionalForceX); */
  /*   printf("what is frictionalY %f \n", dynamic_object->frictionalForceY); */
  /*   printf("what is dx %f \n", dynamic_object->dx); */
  /*   printf("what is dy %f \n", dynamic_object->dy); */
  /*   printf("what is x %f \n", dynamic_object->x); */
  /*   printf("what is y %f \n", dynamic_object->y); */
  /*   fflush(stdout); */
  /* } */

  // Clamp
  if (!dynamic_object->isMoving) {
    if ((dynamic_object->dx < prevDx && prevDx > 0 && dynamic_object->dx < 0) || (dynamic_object->dx > prevDx && prevDx < 0 && dynamic_object->dx > 0)) {
      dynamic_object->dx = 0;
      dynamic_object->ax = 0;
      dynamic_object->directionX = 0;
      dynamic_object->thrustX = 0;
      dynamic_object->frictionalForceX = 0;
    }
    if ((dynamic_object->dy < prevDy && prevDy > 0 && dynamic_object->dy < 0) || (dynamic_object->dy > prevDy && prevDy < 0 && dynamic_object->dy > 0)) {
      dynamic_object->dy = 0;
      dynamic_object->ay = 0;
      dynamic_object->directionY = 0;
      dynamic_object->thrustY = 0;
      dynamic_object->frictionalForceY = 0;
    }
  }

  dynamic_object->x += dynamic_object->dx * *dt * PIXELS_PER_METER; 
  dynamic_object->y += dynamic_object->dy * *dt * PIXELS_PER_METER; 
 
  return 0;
}


void renderCrate(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect srcRect = { dynamic_object->sprite * dynamic_object->w, dynamic_object->h * dynamic_object->direction, dynamic_object->w, dynamic_object->h};
  SDL_Rect destRect = {x, y, dynamic_object->w, dynamic_object->h};
  SDL_RenderCopy(renderer, dynamic_object->crateTexture, &srcRect, &destRect);
}

void renderJar(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect srcRect = { dynamic_object->sprite * dynamic_object->w, dynamic_object->h * dynamic_object->direction, dynamic_object->w, dynamic_object->h};
  SDL_Rect destRect = {x, y, dynamic_object->w, dynamic_object->h};
  SDL_RenderCopy(renderer, dynamic_object->jarTexture, &srcRect, &destRect);
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
  } else if (tileId == '*') {
    tileRow = 1;
    tileColumn = 13;
  } else if (tileId == '#') {
    tileRow = 5;
    tileColumn = 6;
  } else {
    tileRow = 12;
    tileColumn = 7;
  }
  SDL_Rect srcRect = {tileRow * 16, tileColumn * 16, 16, 16};
  SDL_Rect tileRect = {x + game->scrollX, y + game->scrollY, game->map.tileSize, game->map.tileSize};
  SDL_RenderCopy(game->renderer, game->terrainTexture, &srcRect, &tileRect);
};

void doRender(Game *game) {
  SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
  SDL_RenderClear(game->renderer);


  for (int y = -game->scrollY/game->map.tileSize; y < (-game->scrollY + WINDOW_HEIGHT)/ game->map.tileSize; y++) {
    for (int x = -game->scrollX/game->map.tileSize; x < (-game->scrollX + WINDOW_WIDTH)/ game->map.tileSize; x++) {
      if (x >= 0 && x < game->map.width && y>= 0 && y < game->map.height) {
        renderTile(game, x * game->map.tileSize, y * game->map.tileSize, game->map.tiles[x + y * game->map.width].tileId);
      }
    }
  }

  for (int i = 0; i < game->map.dynamic_objects_count; i++) {
    if (game->map.dynamic_objects[i].type == MAN) {
      renderMan(&game->map.dynamic_objects[i], game->map.dynamic_objects[i].x+game->scrollX, game->map.dynamic_objects[i].y+game->scrollY, game->renderer);
    } else if (game->map.dynamic_objects[i].type == JAR) {
        if (game->map.dynamic_objects[i].isLifted) {
          game->mainCharacter->isLifting = 0;
          game->map.dynamic_objects[i].x = game->mainCharacter->x;
          game->map.dynamic_objects[i].y = game->mainCharacter->y - game->mainCharacter->h;
        }
        renderJar(&game->map.dynamic_objects[i], game->map.dynamic_objects[i].x+game->scrollX, game->map.dynamic_objects[i].y+game->scrollY, game->renderer);
    } else if (game->map.dynamic_objects[i].type == CRATE) {
      renderCrate(&game->map.dynamic_objects[i], game->map.dynamic_objects[i].x+game->scrollX, game->map.dynamic_objects[i].y+game->scrollY, game->renderer);
    }
  }

  for (int i = 0; i < game->map.dynamic_objects_count; i++) {
    char* currentDialog = game->map.dynamic_objects[i].currentDialog;
    if (currentDialog != NULL) {
      renderDialogBox(game->renderer);
      SDL_Color color = {255, 255, 255};
      renderText(game->renderer, game->font, currentDialog, color, 25, WINDOW_HEIGHT - 180, WINDOW_WIDTH - 45, 20);
    }
  }

  if (game->status == IS_PAUSED) {
    renderPauseState(game->renderer, game->font);
  }

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
    if (game->map.dynamic_objects[i].type == MAN) {
      game->map.dynamic_objects[i] = initialize_dynamic_object(game->renderer, &game->map.dynamic_objects[i], DOWN, 0, 70, 700, 800, IS_IDLE, RIGHT, MAN);
    } else if (game->map.dynamic_objects[i].type == CRATE) {
      game->map.dynamic_objects[i] = initialize_dynamic_object(game->renderer, &game->map.dynamic_objects[i], UP, 0, 80, 700, 600, IS_IDLE, UP, CRATE);
    } else if (game->map.dynamic_objects[i].type == JAR) {
      game->map.dynamic_objects[i] = initialize_dynamic_object(game->renderer, &game->map.dynamic_objects[i], UP, 0, 80, 700, 600, IS_IDLE, UPRIGHT, JAR);
    }
    if (game->map.dynamic_objects[i].isMain) {
      game->mainCharacter = &game->map.dynamic_objects[i];
    }
  }
}

void loadGame(Game *game) {
  game->dt = 1.0f/60.0f;
  game->font = initializeFont("fonts/slkscr.ttf", 24);
  game->scrollX = 0;
  game->scrollY = 0;
  game->dismissDialog = 0;
  game->terrainTexture = initializeTerrain(game->renderer);
  game->status = IS_ACTIVE;
  loadMap(game, "map_01.lvl");
};

void detectCollision(Game *game, DynamicObject *active_dynamic_object, Target *target) {
  float mainX = active_dynamic_object->x;
  float mainY = active_dynamic_object->y;
  float mainDx = active_dynamic_object->dx;
  float mainDy = active_dynamic_object->dy;
  int mainW = active_dynamic_object->w;
  int mainH = active_dynamic_object->h;

  if (mainX + mainW / 2 > target->x && mainX + mainW/ 2 < target->x+target->w) {
    if (mainY < target->h+target->y && mainY > target->y && mainDy < 0) {
      active_dynamic_object->y = target->y+target->h;
      active_dynamic_object->dy = 0;
    } 
  }
  
  if (mainX + mainW > target->x && mainX<target->x+target->w) {
    if (mainY + mainH > target->y && mainY < target->y && mainDy > 0) {
      active_dynamic_object->y = target->y-mainH;
      if (!game->mainCharacter->isPushing) {
        active_dynamic_object->dy = 0;
      }
    }
  }

  if (mainY + mainH/2 > target->y && mainY<target->y+target->h) {
    if (mainX < target->x+target->w && mainX+mainW > target->x+target->w && mainDx < 0) {
      active_dynamic_object->x = target->x + target->w;
      active_dynamic_object->dx = 0;
    } else if (mainX+mainW > target->x && mainX < target->x && mainDx > 0) {
      active_dynamic_object->x = target->x - mainW;
      active_dynamic_object->dx = 0;
    }
  }


}

void detectObjectCollision(Game *game, DynamicObject *active_dynamic_object, DynamicObject *target_object) {
  Target target = { .x=target_object->x, .y=target_object->y, .w=target_object->w, .h=target_object->h};
  detectCollision(game, active_dynamic_object, &target);
}

void detectTileCollision(Game *game, DynamicObject *active_dynamic_object, Tile *tile) {
  Target target = { .x=tile->x, .y=tile->y, .w=tile->w, .h=tile->h};
  detectCollision(game, active_dynamic_object, &target);
}

void handleObjectInteractions(Game *game, DynamicObject *active_dynamic_object) {
  for (int y = -game->scrollY/game->map.tileSize; y < (-game->scrollY + WINDOW_HEIGHT)/ game->map.tileSize; y++)
    for (int x = -game->scrollX/game->map.tileSize; x < (-game->scrollX + WINDOW_WIDTH)/ game->map.tileSize; x++) {
    int tileIndex = x + y * game->map.width;
    if (tileIndex < 0) continue;

    for (int i = 0; i < game->map.dynamic_objects_count; i++) {
      float objectX = game->map.dynamic_objects[i].x;
      float objectY = game->map.dynamic_objects[i].y;
      float objectW = game->map.dynamic_objects[i].w;
      float objectH = game->map.dynamic_objects[i].h;
      int doIndexX = (objectX + objectW/2)/game->map.tileSize;
      int doIndexY = (objectY + objectH/2)/game->map.tileSize;
      int previousTile = game->map.dynamic_objects[i].currentTile;
      game->map.dynamic_objects[i].currentTile = doIndexX + doIndexY * game->map.width;

      if (previousTile != game->map.dynamic_objects[i].currentTile) {
        game->map.tiles[game->map.dynamic_objects[i].currentTile].dynamic_object_id = game->map.dynamic_objects[i].id;
        game->map.tiles[previousTile].dynamic_object_id = 0;
      } 

      if (x >= 0 && x < game->map.width && y>= 0 && y < game->map.height) {
        if (game->map.tiles[tileIndex].tileState == IS_TELEPORT && tileIndex == game->mainCharacter->currentTile) {
            loadMap(game, game->map.tiles[tileIndex].teleportTo);
        }
        int tileIsSolid = game->map.tiles[tileIndex].tileState == IS_SOLID;
        int tileHasObject = game->map.tiles[tileIndex].dynamic_object_id;
        int isNotSelf = game->map.tiles[tileIndex].dynamic_object_id != active_dynamic_object->id;

        if (game->map.dynamic_objects[i].isMovable && !game->mainCharacter->isPushing)  {
          game->map.dynamic_objects[i].isMoving = 0;
          game->map.dynamic_objects[i].moveUp = 0;
          game->map.dynamic_objects[i].moveLeft = 0;
          game->map.dynamic_objects[i].moveRight = 0;
          game->map.dynamic_objects[i].moveDown = 0;
        }

        if (tileIsSolid) {
          detectTileCollision(game, active_dynamic_object, &game->map.tiles[tileIndex]);
        }

        detectObjectCollision(game, active_dynamic_object, &game->map.dynamic_objects[i]);

        if (tileHasObject && isNotSelf) {

          int tileIsAboveObject = game->map.dynamic_objects[i].currentTile == active_dynamic_object->currentTile - game->map.width && active_dynamic_object->direction == UP; 
          int tileIsBelowObject = game->map.dynamic_objects[i].currentTile == active_dynamic_object->currentTile + game->map.width && active_dynamic_object->direction == DOWN;
          int tileIsToLeftOfObject = game->map.dynamic_objects[i].currentTile == active_dynamic_object->currentTile - 1 && active_dynamic_object->direction == LEFT;
          int tileIsToRightOfObject = game->map.dynamic_objects[i].currentTile == active_dynamic_object->currentTile + 1 && active_dynamic_object->direction == RIGHT;

          if (game->map.dynamic_objects[i].isLiftable && active_dynamic_object->isLifting)  {
            if (tileIsBelowObject) {
              game->map.dynamic_objects[i].isLifted = 1;
              active_dynamic_object->has_object = 1;
            }
            if (tileIsAboveObject) {
              game->map.dynamic_objects[i].isLifted = 1;
              active_dynamic_object->has_object = 1;
            } 
            if (tileIsToLeftOfObject) {
              game->map.dynamic_objects[i].isLifted = 1;
              active_dynamic_object->has_object = 1;
            }
            if (tileIsToRightOfObject) {
              game->map.dynamic_objects[i].isLifted = 1;
              active_dynamic_object->has_object = 1;
            } 
          }

          if (game->map.dynamic_objects[i].isMovable && active_dynamic_object->isPushing)  {
            game->map.dynamic_objects[i].isMoving = active_dynamic_object->isPushing ? 1 : 0;
            game->map.dynamic_objects[i].moveLeft = tileIsToLeftOfObject ? 1 : 0;
            game->map.dynamic_objects[i].moveUp = tileIsAboveObject ? 1 : 0;
            game->map.dynamic_objects[i].moveRight = tileIsToRightOfObject ? 1 : 0;
            game->map.dynamic_objects[i].moveDown = tileIsBelowObject ? 1 : 0;
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

// Eventually we want to have certain things that we drop and things that 
// we can throw. Or something like that. For now we just drop the object in
// the adjacent tile. 
void triggerDrop(Game *game) {
  for (int i = 0; i < game->map.dynamic_objects_count; i++) {
    if (game->map.dynamic_objects[i].isLifted) {
      game->map.dynamic_objects[i].isLifted = 0;
      game->mainCharacter->has_object = 0;
      if (game->mainCharacter->direction == UP) {
        game->map.dynamic_objects[i].x = game->mainCharacter->x;
        game->map.dynamic_objects[i].y = game->mainCharacter->y - game->map.tiles[0].h;
      }
      if (game->mainCharacter->direction == DOWN) {
        game->map.dynamic_objects[i].x = game->mainCharacter->x;
        game->map.dynamic_objects[i].y = game->mainCharacter->y + game->map.tiles[0].h;
      }
      if (game->mainCharacter->direction == RIGHT) {
        game->map.dynamic_objects[i].x = game->mainCharacter->x + game->map.tiles[0].w;
        game->map.dynamic_objects[i].y = game->mainCharacter->y;
      }
      if (game->mainCharacter->direction == LEFT) {
        game->map.dynamic_objects[i].x = game->mainCharacter->x - game->map.tiles[0].w;
        game->map.dynamic_objects[i].y = game->mainCharacter->y; 
      }
    }
  }

  return;
}

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
  } else {
    game->status = IS_ACTIVE;
    return;
  }

  if (townsperson->id &&townsperson->dialogues[townsperson->state].line_count) {
    for (int i = 0; i < townsperson->dialogues[townsperson->state].line_count; i++) {
      enqueue(&townsperson->dialogue_queue, (void*)&speak, townsperson->dialogues[townsperson->state].lines[i], (void*)&game->dismissDialog, 0);
    }
  }
  for (int i = 0; i < game->map.dynamic_objects_count; i++) {
    if (townsperson->id == game->map.dynamic_objects[i].id && townsperson->dialogues[townsperson->state].line_count) {
      game->status = IS_DIALOGUE;
      if (game->map.dynamic_objects[i].state == SPOKEN && townsperson->dialogues[SPOKEN_TWICE].line_count) {
        game->map.dynamic_objects[i].state = SPOKEN_TWICE;
      } else if (townsperson->state == DEFAULT && townsperson->dialogues[SPOKEN].line_count){
        game->map.dynamic_objects[i].state = SPOKEN;
      }
    }
  }
}

void process(Game *game) {
  game->time++;
  if (!strncmp(game->map.name, "map_01.lvl", 12)) { 
    if (game->mainCharacter->currentTile == 150 && game->status != IS_CUTSCENE) {
      // How do I make a switch that applies globally. 
      // I want to have dialogue or behavior only happen when an event is triggered
      // It should take precedence over normal dialogue behavior
      // It _can_ persist, or not
      // Maybe this is a "Quest" state?
      // Do quest states have subgroups that need to be arranged accordingly
      // Maybe quest state comes from the main character!
      // So we still load via another level of nesting, but we 
      // set it on the mainCharacter (or just the game?)
      // dialogues[QUEST_STATE][DIALOGUE_STATE].lines
      // but also "dialogues" state is like... local state. stuff that only persists
      // while you are on the map but resets once you load a new map.
      // But this could be true for cutscenes. Like a cutscene sets
      // local state, but it could set quest state. I think as long
      // as we set quest state on the game or the main character
      // and local state on NPCs, we'll be pretty flexible.
      // At any rate, this needs to be generalized and possible
      // to add to the map, rather than hardcoded.
      // It probably would make sense to pause all actions aside
      // from default behavior, too. Maybe just flush dialogue_queues.
      game->status = IS_CUTSCENE;
      DynamicObject *townsperson = NULL;
      townsperson = getDynamicObjectFromMap(&game->map, 2);
      enqueue(&game->mainCharacter->action_queue, (void*)&moveDown, (void*)1, (void*)&game->map.tileSize, NULL);
      enqueue(&game->mainCharacter->dialogue_queue, (void*)&speak, "You probably shouldn't try to step here again.", (void*)&game->dismissDialog, 0);
      enqueue(&game->mainCharacter->dialogue_queue, (void*)&speak, "Yikes, you just stepped in some rotten milk!", (void*)&game->dismissDialog, 0);
      enqueue(&townsperson->dialogue_queue, (void*)&speak, "Egads!", (void*)&game->dismissDialog, (void*)10000);
      townsperson->default_behavior = WALKING;
    }
  }
      
  int action_running = 0;
  int dialogue_running = 0;

  for (int i = 0; i < game->map.dynamic_objects_count; i++) {
    if (game->map.dynamic_objects[i].default_behavior == WALKING && game->map.dynamic_objects[i].action_queue.size == 0 && !game->map.dynamic_objects[i].isMain && game->status == IS_ACTIVE) {
      if (fmod(game->time, 240) == 0) {
        int randomNumber = rand() % 4;

        if (randomNumber == 0) {
          enqueue(&game->map.dynamic_objects[i].action_queue, (void*)&moveUp, (void*)1, (void*)&game->map.tileSize, NULL);
        } else if (randomNumber == 1) {
          enqueue(&game->map.dynamic_objects[i].action_queue, (void*)&moveRight, (void*)1, (void*)&game->map.tileSize, NULL);
        } else if (randomNumber == 2) {
          enqueue(&game->map.dynamic_objects[i].action_queue, (void*)&moveDown, (void*)1, (void*)&game->map.tileSize, NULL);
        } else if (randomNumber == 4) {
          enqueue(&game->map.dynamic_objects[i].action_queue, (void*)&moveLeft, (void*)1, (void*)&game->map.tileSize, NULL);
        }
      }
    }

    if (game->map.dynamic_objects[i].dialogue_queue.size > 0) {
      game->map.dynamic_objects[i].dialogue_queue.prev_size = game->map.dynamic_objects[i].dialogue_queue.size;
      dialogue_running = process_queue(&game->map.dynamic_objects[i], &game->map.dynamic_objects[i].dialogue_queue);
;
    }

    if (game->map.dynamic_objects[i].action_queue.size > 0) {
      game->map.dynamic_objects[i].action_queue.prev_size = game->map.dynamic_objects[i].action_queue.size;
      action_running = process_queue(&game->map.dynamic_objects[i], &game->map.dynamic_objects[i].action_queue);
    }

    if (!dialogue_running && !game->map.dynamic_objects[i].dialogue_queue.is_enqueuing && game->map.dynamic_objects[i].dialogue_queue.size > 0) {
      game->map.dynamic_objects[i].dialogue_queue = dequeue(&game->map.dynamic_objects[i].dialogue_queue);
      dialogue_running = 1;
    }

    if (!action_running && game->map.dynamic_objects[i].action_queue.size > 0 && !game->map.dynamic_objects[i].action_queue.is_enqueuing) {
      game->map.dynamic_objects[i].action_queue = dequeue(&game->map.dynamic_objects[i].action_queue);
      action_running = 1;
    }

    if (!dialogue_running && game->map.dynamic_objects[i].dialogue_queue.prev_size == (game->map.dynamic_objects[i].dialogue_queue.size + 1) && game->map.dynamic_objects[i].dialogue_queue.size == 0) {
      if (game->status == IS_DIALOGUE || game->status == IS_CUTSCENE) { 
        game->status = IS_ACTIVE;
        game->map.dynamic_objects[i].dialogue_queue.prev_size = 0;
      }
    }
  }

  for (int i = 0; i < game->map.dynamic_objects_count; i++) {
    handlePhysics(&game->map.dynamic_objects[i], &game->map.tiles[game->map.dynamic_objects[i].currentTile], &game->dt, game);
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
  // 60 FPS / 8 animations 
    for (int i = 0; i < game->map.dynamic_objects_count; i++) {
      if (game->map.dynamic_objects[i].type == MAN) {
        if (fmod(game->time, 7.5) == 0) {
        game->map.dynamic_objects[i].sprite = (game->map.dynamic_objects[i].sprite + 1) % 8;
        if (game->map.dynamic_objects[i].dx != 0 || game->map.dynamic_objects[i].dy != 0) { 
          game->map.dynamic_objects[i].angle = getAngle(game);
          game->map.dynamic_objects[i].direction = getDirection(game);
        }
        game->map.dynamic_objects[i].status = game->map.dynamic_objects[i].dx != 0 || game->map.dynamic_objects[i].dy != 0 ? IS_RUNNING : IS_IDLE;
        }
      } 
      handleObjectInteractions(game, &game->map.dynamic_objects[i]);
    }
};

void shutdownGame(Game *game) {
  SDL_DestroyTexture(game->terrainTexture);
  for (int i = 0; i < game->map.dynamic_objects_count; i++) {
    SDL_DestroyTexture(game->map.dynamic_objects[i].idleTexture);
    SDL_DestroyTexture(game->map.dynamic_objects[i].runningTexture);
    SDL_DestroyTexture(game->map.dynamic_objects[i].crateTexture);
    SDL_DestroyTexture(game->map.dynamic_objects[i].jarTexture);
    free(game->map.dynamic_objects[i].action_queue.items);
    free(game->map.dynamic_objects[i].dialogue_queue.items);
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

  if (game.window == NULL) {
    fprintf(stderr, "could not create window: %s\n", SDL_GetError());
    return 1;
  }

  game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED);
  if (game.renderer == NULL) {
    fprintf(stderr, "could not create renderer: %s\n", SDL_GetError());
    return 1;
  }

  SDL_SetRenderDrawBlendMode(game.renderer, SDL_BLENDMODE_BLEND);

  loadGame(&game);

  int done = 0;
  while (!done) {
    done = handleEvents(&game);

    if (game.status != IS_PAUSED) {
      process(&game);
    }

    doRender(&game);

    SDL_Delay(10);
  }

  shutdownGame(&game);

  return 0;
}
