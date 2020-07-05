#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include "rpg.h"
#include "game.h"
#include "physics.h"

void togglePauseState(Game *game) {
  if (!game->is_paused) {
    game->is_paused = 1;
  } else {
    game->is_paused =  0;
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
            loadMap(game, "maps/map_01.lvl", 0, -1, NULL);
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
          case SDL_SCANCODE_RETURN:
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
  /* printf("what is currentTile %d \n", dynamic_object->currentTile); */
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

void doRender(Game *game) {
  SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
  SDL_RenderClear(game->renderer);
  int dialogueCount = 0;

  
  if (fmod(game->time, 7.5) == 0) {
    printf("mainX is %f\n", game->mainCharacter->x);
    printf("mainY is %f\n", game->mainCharacter->y);
    printf("camera->x is %f\n", game->camera.x);
    printf("scrollY is %f\n", game->camera.y);
  }

  SDL_Rect cameraRect = { game->camera.x, game->camera.y, game->current_map->tileSize, game->current_map->tileSize};
  SDL_SetRenderDrawColor(game->renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(game->renderer, &cameraRect);

  for (int y = -game->camera.y/game->current_map->tileSize; y < (-game->camera.y + WINDOW_HEIGHT)/ game->current_map->tileSize; y++) {
    for (int x = -game->camera.x/game->current_map->tileSize; x < (-game->camera.x + WINDOW_WIDTH)/ game->current_map->tileSize; x++) {
      if (x >= 0 && x < game->current_map->width && y>= 0 && y < game->current_map->height) {
        renderTile(x * game->current_map->tileSize, y * game->current_map->tileSize, game->camera, game->current_map->tileSize, game->current_map->tiles[x + y * game->current_map->width].tileId, game->indoorTexture, game->renderer);
      }
    }
  }

  for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
    if (game->current_map->dynamic_objects[i].type == MAN) {
      renderMan(&game->current_map->dynamic_objects[i], game->current_map->dynamic_objects[i].x+game->camera.x, game->current_map->dynamic_objects[i].y+game->camera.y, game->renderer);
    } else if (game->current_map->dynamic_objects[i].type == JAR) {
        if (game->current_map->dynamic_objects[i].isLifted) {
          game->current_map->dynamic_objects[i].x = game->mainCharacter->x;
          game->current_map->dynamic_objects[i].y = game->mainCharacter->y - game->mainCharacter->h;
        }
        renderJar(&game->current_map->dynamic_objects[i], game->current_map->dynamic_objects[i].x+game->camera.x, game->current_map->dynamic_objects[i].y+game->camera.y, game->renderer);
    } else if (game->current_map->dynamic_objects[i].type == BED) {
      renderBed(&game->current_map->dynamic_objects[i], game->current_map->dynamic_objects[i].x+game->camera.x, game->current_map->dynamic_objects[i].y+game->camera.y, game->renderer);
    } else if (game->current_map->dynamic_objects[i].type == CRATE) {
      renderCrate(&game->current_map->dynamic_objects[i], game->current_map->dynamic_objects[i].x+game->camera.x, game->current_map->dynamic_objects[i].y+game->camera.y, game->renderer);
    } else if (game->current_map->dynamic_objects[i].type == DOOR) {
      renderDoor(&game->current_map->dynamic_objects[i], game->current_map->dynamic_objects[i].x+game->camera.x, game->current_map->dynamic_objects[i].y+game->camera.y, game->renderer);
    } else if (game->current_map->dynamic_objects[i].type == EVENT) {
      renderMan(&game->current_map->dynamic_objects[i], game->current_map->dynamic_objects[i].x+game->camera.x, game->current_map->dynamic_objects[i].y+game->camera.y, game->renderer);
    }
  }

  for (int y = -game->camera.y/game->current_map->tileSize; y < (-game->camera.y + WINDOW_HEIGHT)/ game->current_map->tileSize; y++) {
    for (int x = -game->camera.x/game->current_map->tileSize; x < (-game->camera.x + WINDOW_WIDTH)/ game->current_map->tileSize; x++) {
      if (x >= 0 && x < game->current_map->width && y>= 0 && y < game->current_map->height && game->current_map->tiles[x+y*game->current_map->width].tileState == IS_ABOVE) {
        renderTile(x * game->current_map->tileSize, y * game->current_map->tileSize, game->camera, game->current_map->tileSize, game->current_map->tiles[x + y * game->current_map->width].tileId, game->indoorTexture, game->renderer);
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
    renderMenu(game->inventory_menu, game->items, game->inventory, game->font, game->renderer, game->items_count);
  }

  if (game->is_paused) {
    renderPauseState(game->renderer, game->font);
  }

  game->mainCharacter->isLifting = 0;
  SDL_RenderPresent(game->renderer);
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

  if (mainX + mainW / 2 > target->x && mainX<target->x+target->w) {
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
  for (int y = -game->camera.y/game->current_map->tileSize; y < (-game->camera.y + WINDOW_HEIGHT)/ game->current_map->tileSize; y++)
    for (int x = -game->camera.x/game->current_map->tileSize; x < (-game->camera.x + WINDOW_WIDTH)/ game->current_map->tileSize; x++) {
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
      }
      game->current_map->dynamic_objects[i].currentTile = doIndexX + doIndexY * game->current_map->width;

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
        int tileHasObject = game->current_map->tiles[tileIndex].dynamic_object_id >= 0;
        int tileHasEvent = game->current_map->tiles[tileIndex].dynamic_object_type == EVENT;
        fflush(stdout);
        if (tileHasEvent) {
          /* printf("current tile %d\n", game->mainCharacter->currentTile); */
          /* printf("previousMainTile  %d\n", *previousMainTile); */
          /* printf("tileIndex %d\n", tileIndex); */
        }
        if (game->status != IS_CUTSCENE && tileHasEvent && tileIndex == game->mainCharacter->currentTile && previousMainTile > 0 && *previousMainTile != game->mainCharacter->currentTile) { 
          game->mainCharacter->currentTile = 0;
          /* printf("my dude %d\n", game->mainCharacter->id); */
          /* printf("current tile %d\n", game->mainCharacter->currentTile); */
          /* printf("previousMainTile  %d\n", *previousMainTile); */
          /* printf("tileIndex %d\n", tileIndex); */
          DynamicObject *event = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[tileIndex].dynamic_object_id);
          /* printf("Sup %d \n", event->id); */
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

        if (tileIsSolid && !tileHasObject) {
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

          int tileIsAboveAndToLeftOfObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile - game->current_map->width - 1 && active_dynamic_object->direction == UPLEFT;
          int tileIsBelowAndToLeftOfObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile + game->current_map->width - 1 && active_dynamic_object->direction == DOWNLEFT;
          int tileIsAboveAndToRightOfObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile - game->current_map->width + 1 && active_dynamic_object->direction == UPRIGHT;
          int tileIsBelowAndToRightOfObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile + game->current_map->width + 1 && active_dynamic_object->direction == DOWNRIGHT;

          if (game->current_map->dynamic_objects[i].isLiftable && active_dynamic_object->isLifting)  {
            if (tileIsAboveObject || tileIsBelowObject || tileIsToLeftOfObject || tileIsToRightOfObject || tileIsAboveAndToLeftOfObject || tileIsBelowAndToLeftOfObject || tileIsAboveAndToRightOfObject || tileIsBelowAndToRightOfObject) {
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
      if (game->mainCharacter->direction == UPRIGHT) {
        game->current_map->dynamic_objects[i].x = game->mainCharacter->x;
        game->current_map->dynamic_objects[i].y = game->mainCharacter->y - game->current_map->tiles[0].h + game->current_map->tileSize;
      }
      if (game->mainCharacter->direction == DOWNRIGHT) {
        game->current_map->dynamic_objects[i].x = game->mainCharacter->x;
        game->current_map->dynamic_objects[i].y = game->mainCharacter->y + game->current_map->tiles[0].h + game->current_map->tileSize;
      }
      if (game->mainCharacter->direction == UPLEFT) {
        game->current_map->dynamic_objects[i].x = game->mainCharacter->x;
        game->current_map->dynamic_objects[i].y = game->mainCharacter->y - game->current_map->tiles[0].h - game->current_map->tileSize;
      }
      if (game->mainCharacter->direction == DOWNLEFT) {
        game->current_map->dynamic_objects[i].x = game->mainCharacter->x;
        game->current_map->dynamic_objects[i].y = game->mainCharacter->y + game->current_map->tiles[0].h - game->current_map->tileSize;
      }
    }
  }

  return;
}

void toggleDoorStatus(DynamicObject *door) {
  if (door->direction == UP) {
    door->direction = DOWNRIGHT; 
    door->isPassable = 1;
  } else {
    door->direction = UP; 
    door->isPassable= 0;
  }
};

// This is more like the function that gets called when
// you try to interact with object
void handleInteraction(Game *game) { 
  DynamicObject *dynamic_object = NULL;
  if (game->mainCharacter->direction == UP && game->current_map->tiles[game->mainCharacter->currentTile - game->current_map->width].dynamic_object_id) {
    dynamic_object = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[game->mainCharacter->currentTile - game->current_map->width].dynamic_object_id);
  } else if (game->mainCharacter->direction == LEFT && game->current_map->tiles[game->mainCharacter->currentTile - 1].dynamic_object_id) {
    dynamic_object = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[game->mainCharacter->currentTile - 1].dynamic_object_id);
  } else if (game->mainCharacter->direction == DOWN && game->current_map->tiles[game->mainCharacter->currentTile + game->current_map->width].dynamic_object_id) {
    dynamic_object = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[game->mainCharacter->currentTile + game->current_map->width].dynamic_object_id);
  } else if (game->mainCharacter->direction == RIGHT && game->current_map->tiles[game->mainCharacter->currentTile + 1].dynamic_object_id) {
    dynamic_object = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[game->mainCharacter->currentTile + 1].dynamic_object_id);
  } else if (game->mainCharacter->direction == UPLEFT && game->current_map->tiles[game->mainCharacter->currentTile - game->current_map->width - 1].dynamic_object_id) {
    dynamic_object = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[game->mainCharacter->currentTile - game->current_map->width - 1].dynamic_object_id);
  } else if (game->mainCharacter->direction == UPRIGHT && game->current_map->tiles[game->mainCharacter->currentTile - game->current_map->width + 1].dynamic_object_id) {
    dynamic_object = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[game->mainCharacter->currentTile - game->current_map->width + 1].dynamic_object_id);
  } else if (game->mainCharacter->direction == DOWNLEFT && game->current_map->tiles[game->mainCharacter->currentTile + game->current_map->width - 1].dynamic_object_id) {
    dynamic_object = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[game->mainCharacter->currentTile + game->current_map->width - 1].dynamic_object_id);
  } else if (game->mainCharacter->direction == DOWNRIGHT && game->current_map->tiles[game->mainCharacter->currentTile + game->current_map->width + 1].dynamic_object_id) {
    dynamic_object = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[game->mainCharacter->currentTile + game->current_map->width + 1].dynamic_object_id);
  } else {
    game->status = IS_ACTIVE;
    return;
  }

  if (dynamic_object != NULL) {
    if (dynamic_object->type == DOOR) {
      toggleDoorStatus(dynamic_object);
      return;
    }
    triggerEvent(game, dynamic_object);
  }
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
      char filename[20]; // e.g. maps/map_01.lvl
      char *token;

      if (task_type == LOAD_MAP) {
        printf("data is %s\n", dynamic_object->interactions[dynamic_object->state].tasks[i].data);
        fflush(stdout);
        char* tempstr = calloc(strlen(dynamic_object->interactions[dynamic_object->state].tasks[i].data)+1, sizeof(char));
        strcpy(tempstr, dynamic_object->interactions[dynamic_object->state].tasks[i].data);
        token = strtok(tempstr, ".");
        snprintf(filename, sizeof filename, "maps/map_%.2s.lvl", token);
        map_id = atoi(token) - 1;
        while (token != NULL) {
          token = strtok(NULL, ".");
          for (int i = 0; i < 3; i++) {
            if (token != NULL) {
              tile_id[i] = token[i];
            }
          }
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
          loadMap(game, filename, map_id, atoi(tile_id), game->mainCharacter);
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
      printf("%d\n", randomNumber);
      fflush(stdout);

      if (randomNumber == 0 && map->tiles[dynamic_object->currentTile - map->width].tileState != IS_SOLID) {
        enqueue(&dynamic_object->task_queue, (void*)&walkUp, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 1 && map->tiles[dynamic_object->currentTile + 1].tileState != IS_SOLID) {
        enqueue(&dynamic_object->task_queue, (void*)&walkRight, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 2 && map->tiles[dynamic_object->currentTile + map->width].tileState != IS_SOLID) {
        enqueue(&dynamic_object->task_queue, (void*)&walkDown, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 3 && map->tiles[dynamic_object->currentTile - 1].tileState != IS_SOLID) {
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

  if (no_tasks_left == 1) {
    /* puts("hi"); */
  } else {
    /* puts("bye"); */
  }
  if (no_tasks_left == 1 && game->status == IS_CUTSCENE) {
    puts("No tasks are left, make active");
    game->status = IS_ACTIVE;
  }

  for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
    if (game->status == IS_ACTIVE || !game->current_map->dynamic_objects[i].isMain) {
      handlePhysics(&game->current_map->dynamic_objects[i], &game->current_map->tiles[game->current_map->dynamic_objects[i].currentTile], &game->dt, game);
    }
  }

  game->camera.x = (WINDOW_WIDTH / 2) - game->mainCharacter->x;
  game->camera.y = (WINDOW_HEIGHT / 2) - game->mainCharacter->y;

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

  if(game->camera.x > 0) {
    game->camera.x = 0;
  }
  if(game->camera.x < -game->current_map->width * game->current_map->tileSize+WINDOW_WIDTH) {
    game->camera.x = -game->current_map->width * game->current_map->tileSize+WINDOW_WIDTH;
  }

  if(game->camera.y > 0) {
    game->camera.y = 0;
  }

  if(game->camera.y < -game->current_map->height * game->current_map->tileSize+WINDOW_HEIGHT) {
    game->camera.y = -game->current_map->height * game->current_map->tileSize+WINDOW_HEIGHT;
  }


  // handle animation
  // 60 FPS / 8 animations 
    for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
      DynamicObject *current_object = &game->current_map->dynamic_objects[i];
      if (current_object->type == MAN) {
        if (fmod(game->time, 7.5) == 0) {
        current_object->sprite = (current_object->sprite + 1) % 8;
        if (current_object->dx != 0 || current_object->dy != 0) { 
          current_object->angle = getAngleFromCoordinates(current_object->dx, current_object->dy);
          current_object->direction = getDirectionFromAngle(current_object->angle);
        }
        current_object->status = current_object->dx != 0 || current_object->dy != 0 ? IS_RUNNING : IS_IDLE;
        }
      } 
      handleObjectCollisions(game, current_object);
    }
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

    if (!game.is_paused && game.status != IS_LOADING) {
      process(&game);
    }

    doRender(&game);

    SDL_Delay(10);
  }

  shutdownGame(&game);

  return 0;
}
