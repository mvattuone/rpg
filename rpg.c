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
#include "collisions.h"
#include "ai.h"
#include "camera.h"

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
          case SDL_SCANCODE_M: {
            DynamicObject *target = getDynamicObjectFromMap(game->current_map, 11);
            set_current_target(game->current_map, &game->camera, target, game->time);
            break;
          }
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
      case SDL_WINDOWEVENT:
        switch (event.window.event) {
            case SDL_WINDOWEVENT_CLOSE:   // exit game
                shutdownGame(game);
                break;
            default:
                break;
        }
        break;
      default:
        break;
    }
  }

  const Uint8 *state = SDL_GetKeyboardState(NULL);

  if (state[SDL_SCANCODE_Q] && (state[SDL_SCANCODE_LGUI] || state[SDL_SCANCODE_RGUI])) {
    shutdownGame(game);
  }

  if (game->status == IS_ACTIVE) {
    SDL_PumpEvents();
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


void doRender(Game *game) {
  SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
  SDL_RenderClear(game->renderer);
  int dialogueCount = 0;

  
  /* if (fmod(game->time, 7.5) == 0) { */
  /*   printf("mainX is %f\n", game->mainCharacter->x); */
  /*   printf("mainY is %f\n", game->mainCharacter->y); */
  /*   printf("camera->x is %f\n", game->camera.base->x); */
  /*   printf("scrollY is %f\n", game->camera.base->y); */
  /* } */


  for (int y = -game->camera.base->y/game->current_map->tileSize; y < (-game->camera.base->y + WINDOW_HEIGHT)/ game->current_map->tileSize; y++) {
    for (int x = -game->camera.base->x/game->current_map->tileSize; x < (-game->camera.base->x + WINDOW_WIDTH)/ game->current_map->tileSize; x++) {
      Tile *activeTile = &game->current_map->tiles[x+y*game->current_map->width];
      if (x >= 0 && x < game->current_map->width && y>= 0 && y < game->current_map->height) {
        if (activeTile->tileState != IS_ABOVE) {
          renderTile(x * game->current_map->tileSize, y * game->current_map->tileSize, game->camera, game->current_map->tileSize, activeTile->tileId, game->indoorTexture, game->renderer);
        }
      }
    }
  }

  for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
    
    DynamicObject *active_dynamic_object = &game->current_map->dynamic_objects[i];

    fflush(stdout);
    if (active_dynamic_object->type == MAN) {
      renderMan(active_dynamic_object, active_dynamic_object->x+game->camera.base->x, active_dynamic_object->y+game->camera.base->y, game->renderer);
    } else if (active_dynamic_object->type == JAR) {
        if (active_dynamic_object->isLifted) {
          active_dynamic_object->x = game->mainCharacter->x;
          active_dynamic_object->y = game->mainCharacter->y - game->mainCharacter->h;
        }
        renderJar(active_dynamic_object, active_dynamic_object->x+game->camera.base->x, active_dynamic_object->y+game->camera.base->y, game->renderer);
    } else if (active_dynamic_object->type == BED) {
      renderBed(active_dynamic_object, active_dynamic_object->x+game->camera.base->x, active_dynamic_object->y+game->camera.base->y, game->renderer);
    } else if (active_dynamic_object->type == CRATE) {
      renderCrate(active_dynamic_object, active_dynamic_object->x+game->camera.base->x, active_dynamic_object->y+game->camera.base->y, game->renderer);
    } else if (active_dynamic_object->type == DOOR) {
      renderDoor(active_dynamic_object, active_dynamic_object->x+game->camera.base->x, active_dynamic_object->y+game->camera.base->y, game->renderer);
    } else if (active_dynamic_object->type == EVENT || active_dynamic_object->type == CAMERA) {
      //  should make some simple render rectangle for debug
    }
  }

  for (int y = -game->camera.base->y/game->current_map->tileSize; y < (-game->camera.base->y + WINDOW_HEIGHT)/ game->current_map->tileSize; y++) {
    for (int x = -game->camera.base->x/game->current_map->tileSize; x < (-game->camera.base->x + WINDOW_WIDTH)/ game->current_map->tileSize; x++) {
      Tile *activeTile = &game->current_map->tiles[x+y*game->current_map->width];
      if (x >= 0 && x < game->current_map->width && y>= 0 && y < game->current_map->height) {

        if (activeTile->tileState == IS_ABOVE) {
          renderTile(x * game->current_map->tileSize, y * game->current_map->tileSize, game->camera, game->current_map->tileSize, activeTile->tileId, game->indoorTexture, game->renderer);
        }
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
  printf("what is id here %d", game->current_map->tiles[game->mainCharacter->currentTile + 1].dynamic_object_id);
  fflush(stdout);
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
    fflush(stdout);
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

void dispatchEvent(DynamicObject *dynamic_object, TaskType task_type, void* data, Game *game) {
  char tile_id[3];
  int map_id;
  char filename[20]; // e.g. maps/map_01.lvl
  char *token;

  if (task_type == LOAD_MAP) {
    char* tempstr = calloc(strlen(data)+1, sizeof(char));
    strcpy(tempstr, data);
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
      enqueue(&dynamic_object->task_queue, (void*)&speak, data, (void*)&game->dismissDialog, 0);
      break;
    case WALK_LEFT:
      enqueue(&dynamic_object->task_queue, (void*)&walkLeft, (void*)(size_t)atoi(data), (void*)&game->current_map->tileSize, NULL);
      break;
    case WALK_RIGHT:
      enqueue(&dynamic_object->task_queue, (void*)&walkRight, (void*)(size_t)atoi(data), (void*)&game->current_map->tileSize, NULL);
      break;
    case WALK_UP:
      enqueue(&dynamic_object->task_queue, (void*)&walkUp, (void*)(size_t)atoi(data), (void*)&game->current_map->tileSize, NULL);
      break;
    case WALK_DOWN:
      enqueue(&dynamic_object->task_queue, (void*)&walkDown, (void*)(size_t)atoi(data), (void*)&game->current_map->tileSize, NULL);
      break;
    case RUN_LEFT:
      enqueue(&dynamic_object->task_queue, (void*)&runLeft, (void*)(size_t)atoi(data), (void*)&game->current_map->tileSize, NULL);
      break;
    case RUN_RIGHT:
      enqueue(&dynamic_object->task_queue, (void*)&runRight, (void*)(size_t)atoi(data), (void*)&game->current_map->tileSize, NULL);
      break;
    case RUN_UP:
      enqueue(&dynamic_object->task_queue, (void*)&runUp, (void*)(size_t)atoi(data), (void*)&game->current_map->tileSize, NULL);
      break;
    case RUN_DOWN:
      enqueue(&dynamic_object->task_queue, (void*)&runDown, (void*)(size_t)atoi(data), (void*)&game->current_map->tileSize, NULL);
      break;
    case REMOVE:
      enqueue(&dynamic_object->task_queue, (void*)&removeObject, NULL, NULL, NULL);
      break;
    case ADD_ITEM:
      enqueue(&dynamic_object->task_queue, (void*)&addToInventory, (void*)(size_t)atoi(data), &game->inventory, NULL);
      break;
    case REMOVE_ITEM:
      enqueue(&dynamic_object->task_queue, (void*)&removeFromInventory, (void*)(size_t)atoi(data), &game->inventory, NULL);
      break;
    case LOAD_MAP:
      loadMap(game, filename, map_id, atoi(tile_id), game->mainCharacter);
      break;
    case EXTERNAL_COMMAND:
      handleExternalEvent(game, data);
      break;
    default:
      break;
  }
}

void handleExternalEvent(Game *game, char* data) {
  game->status = IS_CUTSCENE;
  game->camera.current_target = NULL;
  char doId[4];
  for (int p = 0; p < 3; p++) {
    doId[p] = data[p]; 
  }
  doId[3] = '\0';
  TaskType task_type = SPEAK;
  int r = 3;
  char e = data[r];
  
  if ( e == '-') {
    r++;
    task_type = SPEAK;
  } else if ( e == '<') {
    r++;
    e = data[r];
    if (e == '<') {
      r++;
      task_type = RUN_LEFT;
    } else {
      e = data[r];
      task_type = WALK_LEFT;
    }
  } else if ( e == '>') {
    r++;
    e = data[r];
    if (e == '>') {
      r++;
      task_type = RUN_RIGHT;
    } else {
      e = data[r];
      task_type = WALK_RIGHT;
    }
  } else if ( e == 'v') {
    r++;
    e = data[r];
    if (e == 'v') {
      r++;
      task_type = RUN_DOWN;
    } else {
      printf("oh ok");
      e = data[r];
      task_type= WALK_DOWN;
    }
  } else if ( e == '^') {
    r++;
    e = data[r];
    if (e == '^') {
      r++;
      task_type= RUN_UP;
    } else {
      e = data[r];
      task_type= WALK_UP;
    }
  } else if ( e == 'x') {
    r++;
    task_type= REMOVE;
  } else if ( e == '%') {
    r++;
    task_type= ADD_ITEM;
  } else if ( e == '@') {
    r++;
    task_type= LOAD_MAP;
  } else if ( e == '#') {
    r++;
    task_type= REMOVE_ITEM;
  } 

  char *newData = malloc(MAX_TASK_SIZE); 
  int px = 0;
  while (data[r] !=  '\0') {
    newData[px] = data[r];
    r++;
    px++;
  }


  DynamicObject *dynamic_object = getDynamicObjectFromMap(game->current_map, atoi(doId));
  dispatchEvent(dynamic_object, task_type, newData, game);

  free(newData);
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
            printf("this should work %d\n", quest->target_id);
            if (quest->target_id == game->current_map->dynamic_objects[i].id) {
              printf("game current tile %d", game->current_map->dynamic_objects[i].currentTile);
              printf("game target tile %d", quest->target_tile);
              fflush(stdout);
              if (game->current_map->dynamic_objects[i].currentTile == quest->target_tile) {
                printf("hey now you're an all star\n");
                fflush(stdout);
                completed_quest = 1;
                quest->state = COMPLETED;
                dynamic_object->state = QUEST_COMPLETED;
              }
            }
           }
        } else if (quest->type == TALK && quest->state == IN_PROGRESS) {
          for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
            if (game->current_map->dynamic_objects[i].id == quest->target_id && game->current_map->dynamic_objects[i].state != DEFAULT) {
              fflush(stdout);
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
      /* printf("what is dynamic_object state %d", dynamic_object->state); */
      /* printf("what is quest %d", dynamic_object->quest); */
      if (quest && ((dynamic_object->state == DEFAULT && dynamic_object->interactions[SPOKEN].task_count == 0 ) || (dynamic_object->state == SPOKEN))) {
        printf("adding new quest...");
        fflush(stdout);
        add_quest(&game->active_quests, dynamic_object->id, quest);
        quest_active = 1;
      } 
      fflush(stdout);
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
      dispatchEvent(dynamic_object, task_type, dynamic_object->interactions[dynamic_object->state].tasks[i].data, game);
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


void process(Game *game) {
  game->time++;
  int task_running = 0;
  int no_tasks_left = 1;

  for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
    DynamicObject *dynamic_object = &game->current_map->dynamic_objects[i];
    Queue *task_queue = &dynamic_object->task_queue;
    int has_task = game->current_map->dynamic_objects[i].task_queue.size > 0;

    if (fmod(game->time, 180) == 0) {
      process_default_behavior(dynamic_object, game->current_map);
    }
    
    task_queue->prev_size = task_queue->size;

    if (has_task) {
      if (game->status != IS_CUTSCENE || (game->status == IS_CUTSCENE && !task_running)) {
        task_running = process_queue(dynamic_object, task_queue); 
      } 
      
      /* printf("is task running %d\n", task_running); */
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
    game->camera.current_target = game->mainCharacter;
  }

  for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
    if (game->status == IS_ACTIVE || !game->current_map->dynamic_objects[i].isMain || game->current_map->dynamic_objects[i].id != game->camera.base->id) {
      handlePhysics(&game->current_map->dynamic_objects[i], &game->current_map->tiles[game->current_map->dynamic_objects[i].currentTile], &game->dt, game);
    }
  }

  handlePhysics(game->camera.base, &game->current_map->tiles[game->camera.base->currentTile], &game->dt, game);

  if (game->camera.current_target != NULL && !game->camera.base->isMoving) {

    game->camera.base->x = (WINDOW_WIDTH / 2) - game->camera.current_target->x;
    game->camera.base->y = (WINDOW_HEIGHT / 2) - game->camera.current_target->y;

    if (game->camera.current_target->x < 0) {
      game->camera.current_target->x = 0;
    }

    if (game->camera.current_target->x > game->current_map->width * game->current_map->tileSize - game->camera.current_target->w) {
      game->camera.current_target->x = game->current_map->width * game->current_map->tileSize - game->camera.current_target->w;
    }

    if (game->camera.current_target->y < 0) {
      game->camera.current_target->y = 0;
    }

    if (game->camera.current_target->y > game->current_map->height * game->current_map->tileSize - game->camera.current_target->h) {
      game->camera.current_target->y = game->current_map->height * game->current_map->tileSize - game->camera.current_target->h;
    }

    if(game->camera.base->x < -game->current_map->width * game->current_map->tileSize+WINDOW_WIDTH) {
      game->camera.base->x = -game->current_map->width * game->current_map->tileSize+WINDOW_WIDTH;
    }

    if(game->camera.base->y < -game->current_map->height * game->current_map->tileSize+WINDOW_HEIGHT) {
      game->camera.base->y = -game->current_map->height * game->current_map->tileSize+WINDOW_HEIGHT;
    }
    
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
