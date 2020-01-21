#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include "rpg.h"
#include "physics.h"

int addToInventory(DynamicObject *dynamic_object, int inventory_id, DynamicArray *inventory) {
  if (&inventory->size > &inventory->capacity) {
    inventory->items = realloc(inventory->items, (sizeof(inventory->items) * sizeof(Item)) + sizeof(Item));
    inventory->capacity = sizeof(inventory->items) + sizeof(Item);
  }

  inventory->items[inventory->size] = inventory_id;
  inventory->size++; 
  return 0;
}

DynamicArray removeFromInventory(DynamicObject *dynamic_object, Game *game, int inventory_id) {
  DynamicArray new_inventory; 
  new_inventory.items = malloc((sizeof(game->inventory.items) * sizeof(Item)) - sizeof(Item));
  new_inventory.capacity = (sizeof(new_inventory.items) * sizeof(Item)) - sizeof(Item);
  new_inventory.size = game->inventory.size - 1;
  for (int i = 0; i < game->inventory.size; i++) {
    if (game->inventory.items[i] != inventory_id) {
      new_inventory.items[i] = game->inventory.items[i];
    }
  }

  free(game->inventory.items);
  return new_inventory;
}


void loadMap(Game *game, char* fileName, int startingTile, int map_id) {
  printf("what is map id %d\n", map_id);
  printf("what is map name %s\n", game->maps[map_id].name);
  int map_loaded = 0;

  if (game->mainCharacter != NULL) {
    removeObject(game->mainCharacter);
  }

  if (strcmp(game->maps[map_id].name, "No Name")) {
    printf("map %d exists, setting \n", map_id);
    fflush(stdout);
    game->current_map = &game->maps[map_id];
    map_loaded = 1;
  }

  printf("Map %d was not loaded, initializing...\n", map_id);
  fflush(stdout);
  if (!map_loaded) {
    game->maps[map_id] = initializeMap(fileName, 32, startingTile);
    game->current_map = &game->maps[map_id];
  }
  for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
    if (!map_loaded) {
      ObjectType type = game->current_map->dynamic_objects[i].type;
      // @TODO It would be cool if we reset _certain_ parameters regardless
      // of whether we have loaded the map or not. We could do that here.
      // And move the other state that should not reset ever elsewhere.
      if (type == MAN) {
        HatType hat = game->current_map->dynamic_objects[i].equipment.hat;
        game->current_map->dynamic_objects[i] = initialize_dynamic_object(game->renderer, &game->current_map->dynamic_objects[i], DOWN, 0, 70, 700, 800, IS_IDLE, RIGHT, MAN, hat);
      } else if (type == CRATE) {
        game->current_map->dynamic_objects[i] = initialize_dynamic_object(game->renderer, &game->current_map->dynamic_objects[i], UP, 0, 80, 700, 600, IS_IDLE, UP, CRATE, -1);
      } else if (type == JAR) {
        game->current_map->dynamic_objects[i] = initialize_dynamic_object(game->renderer, &game->current_map->dynamic_objects[i], UP, 0, 80, 700, 600, IS_IDLE, UPRIGHT, JAR, -1);
      } else if (type == BED) {
        game->current_map->dynamic_objects[i] = initialize_dynamic_object(game->renderer, &game->current_map->dynamic_objects[i], UP, 0, 80, 700, 600, IS_IDLE, UP, BED, -1);
      } else if (type == EVENT) {
        game->current_map->dynamic_objects[i] = initialize_dynamic_object(game->renderer, &game->current_map->dynamic_objects[i], UP, 0, 80, 700, 600, IS_IDLE, UP, EVENT, -1);
      } else if (type == DOOR) {
        game->current_map->dynamic_objects[i] = initialize_dynamic_object(game->renderer, &game->current_map->dynamic_objects[i], UP, 0, 80, 700, 600, IS_IDLE, UP, DOOR, -1);
      }
    }
    if (game->current_map->dynamic_objects[i].isMain) {
      game->mainCharacter = &game->current_map->dynamic_objects[i];
      // @TODO Make a function like setDynamicObjectPosition
      // I guess it can be void?
      game->current_map->dynamic_objects[i].x = (startingTile % game->current_map->width) * game->current_map->tileSize;
      game->current_map->dynamic_objects[i].y = floor(startingTile/game->current_map->width) * game->current_map->tileSize; 
      game->current_map->dynamic_objects[i].startingTile = startingTile;
      game->current_map->dynamic_objects[i].currentTile = game->mainCharacter->startingTile;
    }
  }
}

// @TODO Need to make paused a separate setting 
// Because we want it override "game states"
void togglePauseState(Game *game) {
  if (game->status == IS_PAUSED) {
    game->status = IS_ACTIVE;
  } else {
    game->status = IS_PAUSED;
  }
}

void toggleMenu(Game *game) {
  if (game->status == IS_MENU) {
    game->status = IS_ACTIVE;
  } else {
    game->status = IS_MENU;
  }
}

int handleEvents(Game *game) {
  SDL_Event event;

  // 
  // TODO: It would be helpful to have references to the main character's 
  // tile, as well as the objects tiles adjacent and their states.
  //

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
          case SDL_SCANCODE_P:
            loadMap(game, "map_01.lvl", -1, 0);
            break;
          case SDL_SCANCODE_O:
            loadMap(game, "map_02.lvl", -1, 1);
            break;
          case SDL_SCANCODE_S:
            if (game->status == IS_ACTIVE || game->status == IS_MENU) {
              toggleMenu(game);
            }
            break;
          case SDL_SCANCODE_X:
            if (game->status == IS_MENU) {
              game->inventory_menu->show_description = game->inventory_menu->show_description ? 0 : 1;
            }
            break;
          case SDL_SCANCODE_A:
            if (game->status == IS_MENU) {
              MenuState menu_state = game->inventory_menu->state;
              if (menu_state == DEFAULT_MENU) {
                game->inventory_menu->selected_item_index = game->inventory_menu->active_item_index;
                game->inventory_menu->state = ITEM_SELECTED;
              } else if (menu_state == ITEM_SELECTED) {
                swap_ints(game->inventory.items, game->inventory_menu->selected_item_index, game->inventory_menu->active_item_index);
                game->inventory_menu->selected_item_index = -1;
                game->inventory_menu->state = DEFAULT_MENU;
              }
            } else if (game->mainCharacter->has_object) { 
              triggerDrop(game);
            } else if (game->status != IS_CUTSCENE) {
              game->mainCharacter->isLifting = 1;
              handleInteraction(game);
              game->dismissDialog = 0;
            } else {
              game->dismissDialog = 1;
            }
            break;
          case SDL_SCANCODE_UP:
            if (game->status == IS_MENU) {
              if (game->inventory_menu->active_item_index) {
                game->inventory_menu->active_item_index--;
              }
            }
            break;
          case SDL_SCANCODE_DOWN:
            if (game->status == IS_MENU) {
              if (game->inventory_menu->active_item_index < game->inventory.size - 1) {
                game->inventory_menu->active_item_index++;
              }
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

void renderBed(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect srcRect = { dynamic_object->sprite * dynamic_object->w, dynamic_object->h * dynamic_object->direction, dynamic_object->w, dynamic_object->h};
  SDL_Rect destRect = {x, y, dynamic_object->w, dynamic_object->h};
  SDL_RenderCopy(renderer, dynamic_object->bedTexture, &srcRect, &destRect);
}

void renderDoor(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect srcRect = { dynamic_object->sprite * dynamic_object->w, dynamic_object->h * dynamic_object->direction, dynamic_object->w, dynamic_object->h};
  SDL_Rect destRect = {x, y, dynamic_object->w, dynamic_object->h};
  SDL_RenderCopy(renderer, dynamic_object->doorTexture, &srcRect, &destRect);
}

void renderJar(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect srcRect = { dynamic_object->sprite * dynamic_object->w, dynamic_object->h * dynamic_object->direction, dynamic_object->w, dynamic_object->h};
  SDL_Rect destRect = {x, y, dynamic_object->w, dynamic_object->h};
  SDL_RenderCopy(renderer, dynamic_object->jarTexture, &srcRect, &destRect);
}

int getHatDirection(Direction direction) {
  switch (direction) {
    case UP:
      return 0;
      break;
    case LEFT:
      return 1;
      break;
    case DOWN:
      return 2;
      break;
    case RIGHT:
      return 3;
      break;
    default:
      break;
  }

  return -1;
};

void renderMan(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect srcRect = { dynamic_object->sprite * dynamic_object->w, dynamic_object->h * dynamic_object->direction, dynamic_object->w, dynamic_object->h};
  SDL_Rect destRect = {x, y, dynamic_object->w, dynamic_object->h};
  dynamic_object->status == IS_RUNNING
    ? SDL_RenderCopy(renderer, dynamic_object->runningTexture, &srcRect, &destRect)
    : SDL_RenderCopy(renderer, dynamic_object->idleTexture, &srcRect, &destRect);
  if (dynamic_object->hatTexture != NULL) {
    int hat_direction = getHatDirection(dynamic_object->direction);
    SDL_Rect srcRect = { hat_direction * 64, 0, 64, 64};
    SDL_Rect destRect = {x - 10, y - 10, dynamic_object->w * 2, dynamic_object->h * 2};
    SDL_RenderCopy(renderer, dynamic_object->hatTexture, &srcRect, &destRect);
  }
}

void renderMenu(Game *game, TTF_Font *font) {
  SDL_Rect MenuRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
  SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 50);
  SDL_RenderFillRect(game->renderer, &MenuRect); 
  SDL_Color text_color = { 255, 255, 255 }; 
  int item_position_index = 0;
  if (game->inventory.size <= 0) {
    renderText(game->renderer, font, "No items", text_color, 80, 40, NULL);
  } else {
    for (int i = 0; i < game->inventory.size; i++) {
      for (int j = 0; j < game->items_count; j++) {
        if (game->inventory.items[i] == game->items[j].id) {
          char *name = game->items[j].name;
          if (game->inventory_menu->state == ITEM_SELECTED && game->inventory_menu->selected_item_index == i) {
            SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font, name, text_color, WINDOW_WIDTH - 50);
            SDL_Rect SelectedItemRect = {80, (i + 1) * 40, surface->w, surface->h};
            SDL_SetRenderDrawColor(game->renderer, 90, 110, 140, 100);
            SDL_RenderFillRect(game->renderer, &SelectedItemRect); 
          }
          renderText(game->renderer, font, name, text_color, 80, (i + 1) * 40, NULL);
          renderCursor(game->renderer, 60, (game->inventory_menu->active_item_index + 1) * 40, 20, 20);
          if (game->inventory_menu->show_description && i == game->inventory_menu->active_item_index) {
            renderText(game->renderer, font, game->items[j].description, text_color, 80, WINDOW_HEIGHT - 100, NULL);
          }
          item_position_index++;
        }
      }
    }
  }
}

 void renderTile(Game *game, int x, int y, char tileId, SDL_Texture *texture) {
  int tileRow;
  int tileColumn;
  tileRow = tileId % 16;
  tileColumn = tileId / 16;
  SDL_Rect srcRect = {tileRow * 16, tileColumn * 16, 16, 16};
  SDL_Rect tileRect = {x + game->scrollX, y + game->scrollY, game->current_map->tileSize, game->current_map->tileSize};
  SDL_RenderCopy(game->renderer, texture, &srcRect, &tileRect);
};

void doRender(Game *game) {
  SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
  SDL_RenderClear(game->renderer);
  int dialogueCount = 0;


  for (int y = -game->scrollY/game->current_map->tileSize; y < (-game->scrollY + WINDOW_HEIGHT)/ game->current_map->tileSize; y++) {
    for (int x = -game->scrollX/game->current_map->tileSize; x < (-game->scrollX + WINDOW_WIDTH)/ game->current_map->tileSize; x++) {
      if (x >= 0 && x < game->current_map->width && y>= 0 && y < game->current_map->height) {
        renderTile(game, x * game->current_map->tileSize, y * game->current_map->tileSize, game->current_map->tiles[x + y * game->current_map->width].tileId, game->indoorTexture);
      }
    }
  }

  for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
    if (game->current_map->dynamic_objects[i].type == MAN) {
      renderMan(&game->current_map->dynamic_objects[i], game->current_map->dynamic_objects[i].x+game->scrollX, game->current_map->dynamic_objects[i].y+game->scrollY, game->renderer);
    } else if (game->current_map->dynamic_objects[i].type == JAR) {
        if (game->current_map->dynamic_objects[i].isLifted) {
          game->current_map->dynamic_objects[i].x = game->mainCharacter->x;
          game->current_map->dynamic_objects[i].y = game->mainCharacter->y - game->mainCharacter->h;
        }
        renderJar(&game->current_map->dynamic_objects[i], game->current_map->dynamic_objects[i].x+game->scrollX, game->current_map->dynamic_objects[i].y+game->scrollY, game->renderer);
    } else if (game->current_map->dynamic_objects[i].type == BED) {
      renderBed(&game->current_map->dynamic_objects[i], game->current_map->dynamic_objects[i].x+game->scrollX, game->current_map->dynamic_objects[i].y+game->scrollY, game->renderer);
    } else if (game->current_map->dynamic_objects[i].type == CRATE) {
      renderCrate(&game->current_map->dynamic_objects[i], game->current_map->dynamic_objects[i].x+game->scrollX, game->current_map->dynamic_objects[i].y+game->scrollY, game->renderer);
    } else if (game->current_map->dynamic_objects[i].type == DOOR) {
      renderDoor(&game->current_map->dynamic_objects[i], game->current_map->dynamic_objects[i].x+game->scrollX, game->current_map->dynamic_objects[i].y+game->scrollY, game->renderer);
    } else if (game->current_map->dynamic_objects[i].type == EVENT) {
      renderMan(&game->current_map->dynamic_objects[i], game->current_map->dynamic_objects[i].x+game->scrollX, game->current_map->dynamic_objects[i].y+game->scrollY, game->renderer);
    }
  }

  for (int y = -game->scrollY/game->current_map->tileSize; y < (-game->scrollY + WINDOW_HEIGHT)/ game->current_map->tileSize; y++) {
    for (int x = -game->scrollX/game->current_map->tileSize; x < (-game->scrollX + WINDOW_WIDTH)/ game->current_map->tileSize; x++) {
      if (x >= 0 && x < game->current_map->width && y>= 0 && y < game->current_map->height && game->current_map->tiles[x+y*game->current_map->width].tileState == IS_ABOVE) {
        renderTile(game, x * game->current_map->tileSize, y * game->current_map->tileSize, game->current_map->tiles[x + y * game->current_map->width].tileId, game->indoorTexture);
      }
    }
  }

  for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
    char* currentDialog = game->current_map->dynamic_objects[i].currentDialog;
    if (currentDialog != NULL) {
      dialogueCount++;
      renderDialogBox(game->renderer, dialogueCount);
      SDL_Color color = {255, 255, 255};
      renderText(game->renderer, game->font, currentDialog, color, 25, WINDOW_HEIGHT - (180 * dialogueCount), NULL);
    }
  }

  if (game->status == IS_MENU) {
    renderMenu(game, game->font);
  }

  if (game->status == IS_PAUSED) {
    renderPauseState(game->renderer, game->font);
  }

  game->mainCharacter->isLifting = 0;
  SDL_RenderPresent(game->renderer);
};


TTF_Font* initializeFont(char* fileName, int fontSize) {
  TTF_Font *font = TTF_OpenFont(fileName, fontSize);
  if (font == NULL) {
    printf("Could not find font");
    SDL_Quit();
    exit(1);
  }

  return font;
}

// @TODO Pass items in this way?
// Might be good if we have different 
// inventories per character...
Menu load_inventory_menu() {
  Menu inventory_menu = {
   0 
  } ;
  inventory_menu.show_description = 0;
  inventory_menu.type = INVENTORY;
  inventory_menu.active_item_index = 0;
  inventory_menu.selected_item_index = -1;
  inventory_menu.cursor = initializeMenuCursor();
  return inventory_menu;
}

void loadGame(Game *game) {
  game->dt = 1.0f/60.0f;
  game->font = initializeFont("fonts/slkscr.ttf", 24);
  game->scrollX = 0;
  game->scrollY = 0;
  game->dismissDialog = 0;
  game->active_quests.size = 0; 
  game->active_quests.capacity = sizeof(Quest); 
  game->active_quests.items = malloc(sizeof(Quest)); 
  // @NOTE - once we have a better sense of our art,
  // this all will likely be condensed into a single tileset.
  // or sets grouped by location type (e.g. snowy, desert)
  game->terrainTexture = createTexture(game->renderer, "images/terrain.png");
  game->indoorTexture = createTexture(game->renderer, "images/indoor.png");
  game->status = IS_ACTIVE;
  game->items = load_items("items.dat", &game->items_count);
  game->inventory.size = 4;
  game->inventory.capacity = sizeof(Item) * game->inventory.size; 
  game->inventory.items = malloc(sizeof(Item) * game->inventory.size); 
  memset(game->inventory.items, -1, sizeof(Item) * game->inventory.size);
  game->inventory.items[0] = 1;
  game->inventory.items[1] = 2;
  *game->inventory_menu = load_inventory_menu();
  game->quests = load_quests("quests.dat", &game->quests_count);
  char bufferPtr[10] = "No Name";
  for (int i = 0; i < 2; i++ ) {
    strcpy(game->maps[i].name, bufferPtr);
  }
  loadMap(game, "map_01.lvl", 186, 0);
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

void handleObjectCollisions(Game *game, DynamicObject *active_dynamic_object) {
  for (int y = -game->scrollY/game->current_map->tileSize; y < (-game->scrollY + WINDOW_HEIGHT)/ game->current_map->tileSize; y++)
    for (int x = -game->scrollX/game->current_map->tileSize; x < (-game->scrollX + WINDOW_WIDTH)/ game->current_map->tileSize; x++) {
    int tileIndex = x + y * game->current_map->width;
    if (tileIndex < 0) continue;


    int *previousMainTile = NULL;
    for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
      float objectX = game->current_map->dynamic_objects[i].x;
      float objectY = game->current_map->dynamic_objects[i].y;
      float objectW = game->current_map->dynamic_objects[i].w;
      float objectH = game->current_map->dynamic_objects[i].h;
      int doIndexX = (objectX + objectW/2)/game->current_map->tileSize;
      int doIndexY = (objectY + objectH/2)/game->current_map->tileSize;
      int previousTile = game->current_map->dynamic_objects[i].currentTile;
      if (game->current_map->dynamic_objects[i].isMain) {
        previousMainTile = &previousTile;
        /* printf("previousMainTile  %d\n", *previousMainTile); */
      }
      game->current_map->dynamic_objects[i].currentTile = doIndexX + doIndexY * game->current_map->width;
      if (game->current_map->dynamic_objects[i].isMain) {
        /* printf("game current tile %d\n", game->current_map->dynamic_objects[i].currentTile); */
      }
      fflush(stdout);

      // Handle resetting dynamic object ids when movement occurs
      // Need to avoid this when an object is static i.e. door, event
      if (previousTile != game->current_map->dynamic_objects[i].currentTile) {
        if (game->current_map->dynamic_objects[i].isMain) {
          /* printf("game current tile %d\n", game->current_map->dynamic_objects[i].currentTile); */
          /* printf("game starting tile %d\n", game->current_map->dynamic_objects[i].startingTile); */
          /* printf("previousMainTile later on  %d\n", *previousMainTile); */
        }
        if (game->current_map->tiles[game->current_map->dynamic_objects[i].currentTile].dynamic_object_type != DOOR && game->current_map->tiles[game->current_map->dynamic_objects[i].currentTile].dynamic_object_type != EVENT) {
          game->current_map->tiles[game->current_map->dynamic_objects[i].currentTile].dynamic_object_id = game->current_map->dynamic_objects[i].id;
        }
        if (game->current_map->tiles[previousTile].dynamic_object_type != DOOR && game->current_map->tiles[previousTile].dynamic_object_type != EVENT) {
          game->current_map->tiles[previousTile].dynamic_object_id = 0;
        } 
      } 

      if (x >= 0 && x < game->current_map->width && y>= 0 && y < game->current_map->height) {
        int tileIsSolid = game->current_map->tiles[tileIndex].tileState == IS_SOLID;
        int tileHasObject = game->current_map->tiles[tileIndex].dynamic_object_id;
        int tileHasEvent = game->current_map->tiles[tileIndex].dynamic_object_type == EVENT;
        fflush(stdout);
        if (tileHasEvent) {
          /* printf("current tile %d\n", game->mainCharacter->currentTile); */
          /* printf("previousMainTile  %d\n", *previousMainTile); */
          /* printf("tileIndex %d\n", tileIndex); */
        }
        if (game->status != IS_CUTSCENE && tileHasEvent && tileIndex == game->mainCharacter->currentTile && previousMainTile > 0 && *previousMainTile != game->mainCharacter->currentTile) { 
          game->mainCharacter->currentTile = 0;
          printf("my dude %d\n", game->mainCharacter->id);
          printf("current tile %d\n", game->mainCharacter->currentTile);
          printf("previousMainTile  %d\n", *previousMainTile);
          printf("tileIndex %d\n", tileIndex);
          DynamicObject *event = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[tileIndex].dynamic_object_id);
          printf("Sup %d \n", event->id);
          fflush(stdout);
          triggerEvent(game, event);
          return;
        }

        int isNotSelf = game->current_map->tiles[tileIndex].dynamic_object_id != active_dynamic_object->id;

        if (game->current_map->dynamic_objects[i].isMovable && !game->mainCharacter->isPushing)  {
          game->current_map->dynamic_objects[i].isMoving = 0;
          game->current_map->dynamic_objects[i].moveUp = 0;
          game->current_map->dynamic_objects[i].moveLeft = 0;
          game->current_map->dynamic_objects[i].moveRight = 0;
          game->current_map->dynamic_objects[i].moveDown = 0;
        }

        if (tileIsSolid) {
          detectTileCollision(game, active_dynamic_object, &game->current_map->tiles[tileIndex]);
        }

        if (game->status == IS_ACTIVE && (game->current_map->dynamic_objects[i].isPassable == 0)) {
          detectObjectCollision(game, active_dynamic_object, &game->current_map->dynamic_objects[i]);
        }

        if (tileHasObject && isNotSelf) {

          int tileIsAboveObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile - game->current_map->width && active_dynamic_object->direction == UP; 
          int tileIsBelowObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile + game->current_map->width && active_dynamic_object->direction == DOWN;
          int tileIsToLeftOfObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile - 1 && active_dynamic_object->direction == LEFT;
          int tileIsToRightOfObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile + 1 && active_dynamic_object->direction == RIGHT;

          if (game->current_map->dynamic_objects[i].isLiftable && active_dynamic_object->isLifting)  {
            if (tileIsBelowObject) {
              game->current_map->dynamic_objects[i].isLifted = 1;
              active_dynamic_object->has_object = 1;
              game->current_map->dynamic_objects[i].isPassable = 1;
            }
            if (tileIsAboveObject) {
              game->current_map->dynamic_objects[i].isLifted = 1;
              active_dynamic_object->has_object = 1;
              game->current_map->dynamic_objects[i].isPassable = 1;
            } 
            if (tileIsToLeftOfObject) {
              game->current_map->dynamic_objects[i].isLifted = 1;
              active_dynamic_object->has_object = 1;
              game->current_map->dynamic_objects[i].isPassable = 1;
            }
            if (tileIsToRightOfObject) {
              game->current_map->dynamic_objects[i].isLifted = 1;
              active_dynamic_object->has_object = 1;
              game->current_map->dynamic_objects[i].isPassable = 1;
            } 
          }

          if (game->current_map->dynamic_objects[i].isMovable && active_dynamic_object->isPushing)  {
            game->current_map->dynamic_objects[i].isMoving = active_dynamic_object->isPushing ? 1 : 0;
            game->current_map->dynamic_objects[i].moveLeft = tileIsToLeftOfObject ? 1 : 0;
            game->current_map->dynamic_objects[i].moveUp = tileIsAboveObject ? 1 : 0;
            game->current_map->dynamic_objects[i].moveRight = tileIsToRightOfObject ? 1 : 0;
            game->current_map->dynamic_objects[i].moveDown = tileIsBelowObject ? 1 : 0;
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
  for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
    if (game->current_map->dynamic_objects[i].isLifted) {
      game->current_map->dynamic_objects[i].isLifted = 0;
      game->current_map->dynamic_objects[i].isPassable = 0;
      game->mainCharacter->has_object = 0;
      if (game->mainCharacter->direction == UP) {
        game->current_map->dynamic_objects[i].x = game->mainCharacter->x;
        game->current_map->dynamic_objects[i].y = game->mainCharacter->y - game->current_map->tiles[0].h;
      }
      if (game->mainCharacter->direction == DOWN) {
        game->current_map->dynamic_objects[i].x = game->mainCharacter->x;
        game->current_map->dynamic_objects[i].y = game->mainCharacter->y + game->current_map->tiles[0].h;
      }
      if (game->mainCharacter->direction == RIGHT) {
        game->current_map->dynamic_objects[i].x = game->mainCharacter->x + game->current_map->tiles[0].w;
        game->current_map->dynamic_objects[i].y = game->mainCharacter->y;
      }
      if (game->mainCharacter->direction == LEFT) {
        game->current_map->dynamic_objects[i].x = game->mainCharacter->x - game->current_map->tiles[0].w;
        game->current_map->dynamic_objects[i].y = game->mainCharacter->y; 
      }
    }
  }

  return;
}

// This is more like the function that gets called when
// you try to interact with object
void handleInteraction(Game *game) { 
  DynamicObject *townsperson = NULL;
  if (game->mainCharacter->direction == UP && game->current_map->tiles[game->mainCharacter->currentTile - game->current_map->width].dynamic_object_id) {
    townsperson = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[game->mainCharacter->currentTile - game->current_map->width].dynamic_object_id);
  } else if (game->mainCharacter->direction == LEFT && game->current_map->tiles[game->mainCharacter->currentTile - 1].dynamic_object_id) {
    townsperson = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[game->mainCharacter->currentTile - 1].dynamic_object_id);
  } else if (game->mainCharacter->direction == DOWN && game->current_map->tiles[game->mainCharacter->currentTile + game->current_map->width].dynamic_object_id) {
    townsperson = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[game->mainCharacter->currentTile + game->current_map->width].dynamic_object_id);
  } else if (game->mainCharacter->direction == RIGHT && game->current_map->tiles[game->mainCharacter->currentTile + 1].dynamic_object_id) {
    townsperson = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[game->mainCharacter->currentTile + 1].dynamic_object_id);
  } else {
    game->status = IS_ACTIVE;
    return;
  }

  if (townsperson->type == DOOR) {
    if (townsperson->direction == UP) {
      townsperson->direction = DOWNRIGHT; 
      townsperson->isPassable = 1;
    } else {
      townsperson->direction = UP; 
      townsperson->isPassable= 0;
    }
    return;
  }

  triggerEvent(game, townsperson);
}

void triggerEvent(Game *game, DynamicObject *dynamic_object) {
  

  int quest_active = 0;
  int completed_quest = 0;
  if (dynamic_object->quest != 0) {
    for (int i = 0; i < game->active_quests.size; i++) {
      if (dynamic_object->quest == game->active_quests.items[i].id) {
        Quest *quest = &game->active_quests.items[i];

        if (quest->type == SWITCH && quest->state == IN_PROGRESS) {
          // @TODO - Create a lookup quest information function of some kind
          for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
            // @TODO Add the switch location
            /* printf("this should work %d\n", quest->target_id); */
            /* fflush(stdout); */
            if (quest->target_id == game->current_map->dynamic_objects[i].id) {
              /* printf("game current tile %d", game->current_map->dynamic_objects[i].currentTile); */
              /* printf("game target tile %d", quest->target_tile); */
              fflush(stdout);
              if (game->current_map->dynamic_objects[i].currentTile == quest->target_tile) {
                completed_quest = 1;
                quest->state = COMPLETED;
                dynamic_object->state = QUEST_COMPLETED;
              }
            }
          }
        } else if (quest->type == TALK && quest->state == IN_PROGRESS) {
          for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
            if (game->current_map->dynamic_objects[i].id == quest->target_id && game->current_map->dynamic_objects[i].state != DEFAULT) {
              completed_quest = 1;
              quest->state = COMPLETED;
              dynamic_object->state = QUEST_COMPLETED;
            }
          }
        } else if (quest->type == ITEM && quest->state == IN_PROGRESS) {
          if (game->inventory.items[0] == quest->target_id) {
            completed_quest = 1;
            quest->state = COMPLETED;
            dynamic_object->state = QUEST_COMPLETED;
            game->inventory = removeFromInventory(NULL, game, 1);
          }
        }
      }
    }

    if (dynamic_object->state != QUEST_ACTIVE || dynamic_object->state != QUEST_COMPLETED || dynamic_object->state != QUEST_ACTIVE_SPOKEN_TWICE) {
      Quest *quest = NULL;
      for (int i = 0; i < game->quests_count; i++) {
        if (dynamic_object->quest == game->quests[i].id) {
          quest = &game->quests[i];
        }
      }
      if (quest && ((dynamic_object->state == DEFAULT && dynamic_object->interactions[SPOKEN].task_count == 0 ) || (dynamic_object->state == SPOKEN))) {
        add_quest(&game->active_quests, dynamic_object->id, quest);
        quest_active = 1;
      } 
    }
  }


  if (dynamic_object->id && dynamic_object->interactions[dynamic_object->state].task_count) {
    if (game->mainCharacter->isMoving) {
      game->mainCharacter->isMoving = 0;
      game->mainCharacter->ax = 0;
      game->mainCharacter->ay = 0;
      game->mainCharacter->dx = 0;
      game->mainCharacter->dy = 0;
      game->mainCharacter->directionX = 0;
      game->mainCharacter->thrustX = 0;
      game->mainCharacter->frictionalForceX = 0;
      game->mainCharacter->directionY = 0;
      game->mainCharacter->thrustY = 0;
      game->mainCharacter->frictionalForceY = 0;
    }
    for (int i = 0; i < dynamic_object->interactions[dynamic_object->state].task_count; i++) {
      TaskType task_type = dynamic_object->interactions[dynamic_object->state].tasks[i].type;

      char tile_id[3];
      int map_id;
      char filename[12]; // e.g. map_01.lvl
      char *token;

      if (task_type == LOAD_MAP) {
        token = strtok(dynamic_object->interactions[dynamic_object->state].tasks[i].data, ".");
        snprintf(filename, sizeof filename, "map_%.2s.lvl", token);
        map_id = atoi(token) - 1;
        printf("what is MAP ID %d \n", map_id);
        fflush(stdout);
        while (token != NULL) {
          token = strtok(NULL, ".");
          for (int i = 0; i < 3; i++) {
            if (token != NULL) {
              tile_id[i] = token[i];
            }
          }
          /* printf("what is it %s", filename); */
          fflush(stdout);
        }
      }

      switch (task_type) {
        case SPEAK:
          enqueue(&dynamic_object->task_queue, (void*)&speak, dynamic_object->interactions[dynamic_object->state].tasks[i].data, (void*)&game->dismissDialog, 0);
          break;
        case WALK_LEFT:
          enqueue(&dynamic_object->task_queue, (void*)&walkLeft, (void*)(size_t)atoi(dynamic_object->interactions[dynamic_object->state].tasks[i].data), (void*)&game->current_map->tileSize, NULL);
          break;
        case WALK_RIGHT:
          enqueue(&dynamic_object->task_queue, (void*)&walkRight, (void*)(size_t)atoi(dynamic_object->interactions[dynamic_object->state].tasks[i].data), (void*)&game->current_map->tileSize, NULL);
          break;
        case WALK_UP:
          enqueue(&dynamic_object->task_queue, (void*)&walkUp, (void*)(size_t)atoi(dynamic_object->interactions[dynamic_object->state].tasks[i].data), (void*)&game->current_map->tileSize, NULL);
          break;
        case WALK_DOWN:
          enqueue(&dynamic_object->task_queue, (void*)&walkDown, (void*)(size_t)atoi(dynamic_object->interactions[dynamic_object->state].tasks[i].data), (void*)&game->current_map->tileSize, NULL);
          break;
        case RUN_LEFT:
          enqueue(&dynamic_object->task_queue, (void*)&runLeft, (void*)(size_t)atoi(dynamic_object->interactions[dynamic_object->state].tasks[i].data), (void*)&game->current_map->tileSize, NULL);
          break;
        case RUN_RIGHT:
          enqueue(&dynamic_object->task_queue, (void*)&runRight, (void*)(size_t)atoi(dynamic_object->interactions[dynamic_object->state].tasks[i].data), (void*)&game->current_map->tileSize, NULL);
          break;
        case RUN_UP:
          enqueue(&dynamic_object->task_queue, (void*)&runUp, (void*)(size_t)atoi(dynamic_object->interactions[dynamic_object->state].tasks[i].data), (void*)&game->current_map->tileSize, NULL);
          break;
        case RUN_DOWN:
          enqueue(&dynamic_object->task_queue, (void*)&runDown, (void*)(size_t)atoi(dynamic_object->interactions[dynamic_object->state].tasks[i].data), (void*)&game->current_map->tileSize, NULL);
          break;
        case REMOVE:
          enqueue(&dynamic_object->task_queue, (void*)&removeObject, NULL, NULL, NULL);
          break;
        case ADD_ITEM:
          enqueue(&dynamic_object->task_queue, (void*)&addToInventory, (void*)(size_t)atoi(dynamic_object->interactions[dynamic_object->state].tasks[i].data), &game->inventory, NULL);
          break;
        case REMOVE_ITEM:
          enqueue(&dynamic_object->task_queue, (void*)&removeFromInventory, (void*)(size_t)atoi(dynamic_object->interactions[dynamic_object->state].tasks[i].data), &game->inventory, NULL);
          break;
        case LOAD_MAP:
          printf("Loading map \n");
          fflush(stdout);
          game->status = IS_CUTSCENE;
          loadMap(game, filename, atoi(tile_id), map_id);
          break;
        default:
          break;
      }
    }
  }


  for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
    if (dynamic_object->id == game->current_map->dynamic_objects[i].id && dynamic_object->interactions[dynamic_object->state].task_count) {
      game->status = IS_CUTSCENE;

      if (!completed_quest && dynamic_object->state == QUEST_ACTIVE) {
        dynamic_object->state = QUEST_ACTIVE_SPOKEN_TWICE;
      } else if (dynamic_object->state == QUEST_COMPLETED) {
        dynamic_object->state = QUEST_COMPLETED_SPOKEN_TWICE;
      } else if (quest_active) { 
        dynamic_object->state = QUEST_ACTIVE;
      } else if (dynamic_object->state == SPOKEN) {
        dynamic_object->state = SPOKEN_TWICE;
      } else if (dynamic_object->state == DEFAULT){
        dynamic_object->state = SPOKEN;
      }
    }
  }
}

void process_default_behavior(DynamicObject *dynamic_object, Map *map) {
  if (dynamic_object->default_behavior == WALKING && dynamic_object->task_queue.size == 0 && !dynamic_object->isMain) {
      int randomNumber = rand() % 4;

      if (randomNumber == 0) {
        enqueue(&dynamic_object->task_queue, (void*)&walkUp, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 1) {
        enqueue(&dynamic_object->task_queue, (void*)&walkRight, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 2) {
        enqueue(&dynamic_object->task_queue, (void*)&walkDown, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 4) {
        enqueue(&dynamic_object->task_queue, (void*)&walkLeft, (void*)1, (void*)&map->tileSize, NULL);
      }
    }

  if (dynamic_object->default_behavior == RUNNING && dynamic_object->task_queue.size == 0 && !dynamic_object->isMain) {
      int randomNumber = rand() % 4;

      if (randomNumber == 0) {
        enqueue(&dynamic_object->task_queue, (void*)&runUp, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 1) {
        enqueue(&dynamic_object->task_queue, (void*)&runRight, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 2) {
        enqueue(&dynamic_object->task_queue, (void*)&runDown, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 4) {
        enqueue(&dynamic_object->task_queue, (void*)&runLeft, (void*)1, (void*)&map->tileSize, NULL);
      }
    }

}

void process(Game *game) {
  game->time++;
  int task_running = 0;
  int no_tasks_left = 1;

  /* printf("hello %d\n", game->status); */
  /* fflush(stdout); */

  for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
    DynamicObject *dynamic_object = &game->current_map->dynamic_objects[i];
    Queue *task_queue = &dynamic_object->task_queue;
    int has_task = task_queue->size > 0;

    if (fmod(game->time, 180) == 0) {
      process_default_behavior(dynamic_object, game->current_map);
    }
    
    task_queue->prev_size = task_queue->size;

    if (has_task) {
      task_running = process_queue(dynamic_object, task_queue); 
      no_tasks_left = 0;
    }

    if (!task_running && !task_queue->is_enqueuing && has_task) {
      *task_queue = dequeue(task_queue);
      task_running = 1;

      if (task_queue->prev_size == 1 && task_queue->size == 0) {
        task_queue->prev_size = 0;
      }
    }
  }

  if (no_tasks_left && (game->status == IS_CUTSCENE)) {
    game->status = IS_ACTIVE;
  }

  for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
    if (game->status == IS_ACTIVE || !game->current_map->dynamic_objects[i].isMain) {
      handlePhysics(&game->current_map->dynamic_objects[i], &game->current_map->tiles[game->current_map->dynamic_objects[i].currentTile], &game->dt, game);
    }
  }

  game->scrollX = -game->mainCharacter->x+WINDOW_WIDTH/2;
  game->scrollY = -game->mainCharacter->y+WINDOW_HEIGHT/2;

  if (game->mainCharacter->x < 0) {
    game->mainCharacter->x = 0;
  }

  if (game->mainCharacter->x > game->current_map->width * game->current_map->tileSize - game->mainCharacter->w) {
    game->mainCharacter->x = game->current_map->width * game->current_map->tileSize - game->mainCharacter->w;
  }

  if (game->mainCharacter->y < 0) {
    game->mainCharacter->y = 0;
  }

  if (game->mainCharacter->y > game->current_map->height * game->current_map->tileSize - game->mainCharacter->h) {
    game->mainCharacter->y = game->current_map->height * game->current_map->tileSize - game->mainCharacter->h;
  }

  if(game->scrollX > 0) {
    game->scrollX = 0;
  }
  if(game->scrollX < -game->current_map->width * game->current_map->tileSize+WINDOW_WIDTH) {
    game->scrollX = -game->current_map->width * game->current_map->tileSize+WINDOW_WIDTH;
  }

  if(game->scrollY > 0) {
    game->scrollY = 0;
  }

  if(game->scrollY < -game->current_map->height * game->current_map->tileSize+WINDOW_HEIGHT) {
    game->scrollY = -game->current_map->height * game->current_map->tileSize+WINDOW_HEIGHT;
  }


  // handle animation
  // 60 FPS / 8 animations 
    for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
      if (game->current_map->dynamic_objects[i].type == MAN) {
        if (fmod(game->time, 7.5) == 0) {
        game->current_map->dynamic_objects[i].sprite = (game->current_map->dynamic_objects[i].sprite + 1) % 8;
        if (game->current_map->dynamic_objects[i].dx != 0 || game->current_map->dynamic_objects[i].dy != 0) { 
          game->current_map->dynamic_objects[i].angle = getAngle(game);
          game->current_map->dynamic_objects[i].direction = getDirection(game);
        }
        game->current_map->dynamic_objects[i].status = game->current_map->dynamic_objects[i].dx != 0 || game->current_map->dynamic_objects[i].dy != 0 ? IS_RUNNING : IS_IDLE;
        }
      } 
      handleObjectCollisions(game, &game->current_map->dynamic_objects[i]);
    }
};

void shutdownGame(Game *game) {
  SDL_DestroyTexture(game->terrainTexture);
  SDL_DestroyTexture(game->indoorTexture);
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < game->maps[i].dynamic_objects_count; i++) {
      SDL_DestroyTexture(game->maps[i].dynamic_objects[j].idleTexture);
      SDL_DestroyTexture(game->maps[i].dynamic_objects[j].runningTexture);
      SDL_DestroyTexture(game->maps[i].dynamic_objects[j].crateTexture);
      SDL_DestroyTexture(game->maps[i].dynamic_objects[j].jarTexture);
      SDL_DestroyTexture(game->maps[i].dynamic_objects[j].hatTexture);
      SDL_DestroyTexture(game->maps[i].dynamic_objects[j].doorTexture);
      SDL_DestroyTexture(game->maps[i].dynamic_objects[j].bedTexture);
      free(game->maps[i].dynamic_objects[j].task_queue.items);
    }
  }
  for (int i = 0; i < game->items_count; i++) {
    free(game->items[i].name);
    free(game->items[i].description);
    if (game->inventory.items) {
      free(game->inventory.items);
    }
  }
  if (game->active_quests.items) {
    free(game->active_quests.items);
  }
  free(game->quests);
  free(game->items);
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
