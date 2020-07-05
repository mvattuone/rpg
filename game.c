#include "game.h"
#include "map.h"
#include "item.h"
#include "inventory.h"

void loadGame(Game *game) {
  game->dt = 1.0f/60.0f;
  game->font = initializeFont("fonts/slkscr.ttf", 24);
  game->camera.x = 0;
  game->camera.y = 0;
  game->camera.width = 0;
  game->camera.height = 0;
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
  game->items = load_items("data/items.dat", &game->items_count);
  game->inventory.size = 4;
  game->inventory.capacity = sizeof(Item) * game->inventory.size; 
  game->inventory.items = malloc(sizeof(Item) * game->inventory.size); 
  memset(game->inventory.items, -1, sizeof(Item) * game->inventory.size);
  game->inventory.items[0] = 1;
  game->inventory.items[1] = 2;
  *game->inventory_menu = load_inventory_menu();
  game->quests = load_quests("data/quests.dat", &game->quests_count);
  char bufferPtr[10] = "No Name";
  for (int i = 0; i < 2; i++ ) {
    strcpy(game->maps[i].name, bufferPtr);
  }
  loadMap(game, "maps/map_01.lvl", 0, -1, NULL);
};

void loadMap(Game *game, char* filePath, int map_id, int startingTile, DynamicObject *mainCharacter) {
  game->status = IS_LOADING;
  printf("what is map id %d\n", map_id);
  printf("what is map name %s\n", game->maps[map_id].name);

  if (game->mainCharacter != NULL) {
    game->mainCharacter = NULL;
  } else {
    printf("Not removing main character since it does not exist");
    fflush(stdout);
  }

  
  if (strcmp(game->maps[map_id].name, "No Name") != 0) {
    printf("map %d exists, setting \n", map_id);
    /* printf("startingTile argument passed is %d", startingTile); */
    game->current_map = &game->maps[map_id];
    for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
      if (game->current_map->dynamic_objects[i].isMain) {
        if (mainCharacter != NULL) {
          game->current_map->dynamic_objects[i] = *mainCharacter;
        }

        game->mainCharacter = &game->current_map->dynamic_objects[i];
        if (startingTile < 0) {
          game->mainCharacter->currentTile = game->mainCharacter->startingTile;
          game->mainCharacter->x = (game->mainCharacter->startingTile % game->current_map->width) * game->current_map->tileSize;
          game->mainCharacter->y = ceil((game->mainCharacter->startingTile - (game->current_map->width * 2))/game->current_map->width) * game->current_map->tileSize; 
        } else {
          game->mainCharacter->currentTile = startingTile;
          game->mainCharacter->x = (startingTile % game->current_map->width) * game->current_map->tileSize;
          game->mainCharacter->y = ceil((startingTile/game->current_map->width)) * game->current_map->tileSize; 
        }
      }
      if (game->current_map->dynamic_objects[i].type == EVENT) {
        for (int j = 0; j < 3; j++) {
          for (int k = 0; k < 1; k++) {
          printf("data for event %d is %s\n", game->current_map->dynamic_objects[i].id, game->current_map->dynamic_objects[i].interactions[j].tasks[k].data);
          fflush(stdout);
          }
        }
      }
    }
    game->status = IS_ACTIVE;
    return;
  }

  printf("Map %d was not loaded, initializing...\n", map_id);
  fflush(stdout);
  game->maps[map_id] = initializeMap(filePath, 32, startingTile, mainCharacter);
  game->current_map = &game->maps[map_id];
  printf("current map name is %s \n", game->current_map->name);
  fflush(stdout);
  for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
    ObjectType type = game->current_map->dynamic_objects[i].type;
    /* printf("what is the type here %d", game->current_map->dynamic_objects[i].type); */
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
    if (game->current_map->dynamic_objects[i].isMain) {
      game->mainCharacter = &game->current_map->dynamic_objects[i];
    }
  }
  game->status = IS_ACTIVE;
}

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
