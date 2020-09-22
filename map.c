#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynamic_object.h"
#include "game.h"

// We'll probably want to have the various tiles
// and their properties stored somewhere. Like a JSON file.
float getCofForTile(char id) {
  float cof;
  if (id == 1) { 
    cof = 0.50;
  } else if (id == 2) {
    cof = 0.05;
  } else if (id == 3) {
    cof = 0.50;
  } else {
    cof = 0.50;
  }

  return cof;
}

float getMaxSpeedForTile(char id) {
  float maxSpeed;

  if (id == 1) { 
    maxSpeed = 3.00;
  } else if (id == 2) {
    maxSpeed = 5.00;
  } else if (id == 3) {
    maxSpeed = 1.50;
  } else {
    maxSpeed = 3.00;
  }

  return maxSpeed;
}

DynamicObject* getDynamicObjectFromMap(Map *map, int id) {
  for (int i = 0; i < map->dynamic_objects_count; i++) {
    if (map->dynamic_objects[i].id == id) {
      return &map->dynamic_objects[i];
    } 
  }

  printf("Could not find dynamic object with id %d\n", id);
  return NULL;
  /* SDL_Quit(); */
  /* exit(1); */
}

Map initializeMap(char* filePath, int tileSize, int starting_tile, DynamicObject *mainCharacter) {
  Map map;
  char* fileName;
  fileName = strrchr(filePath, '/');
  strncpy(map.name, fileName, 12);
  map.tileSize = tileSize;

  FILE *mapData = fopen(filePath, "r");

  if (mapData == NULL) {
    printf("Map could not be loaded.");
    exit(1);
  } else {
    fscanf(mapData, "%d %d %d", &map.width, &map.height, &map.dynamic_objects_count);
    // Move to next line
    fgetc(mapData);
    fgets(map.name, sizeof(map.name), mapData);
    /* printf("what is map name %s\n", map.name); */
    if (map.dynamic_objects_count > MAX_DYNAMIC_OBJECTS) {
      printf("More dynamic objects than allowed. Either increase MAX_DYNAMIC_OBJECTS or remove dynamic objects. Max Object Size is %d and current count is %d\n", MAX_DYNAMIC_OBJECTS, map.dynamic_objects_count);
      exit(1);
    }
    Tile **tiles = malloc(map.width * map.height * sizeof(Tile));
    DynamicObject **dynamic_objects = malloc(map.dynamic_objects_count * sizeof(DynamicObject));
    char c;
    int count = 0;
    while (count < map.width * map.height && (c = fgetc(mapData))) {
      if (c != '\n' && c != ' ' && !isspace(c)) {
        char tempId[3];
        tempId[0] = c;
        tempId[1] = fgetc(mapData);
        tempId[2] = '\0';
        tiles[count] = malloc(sizeof(Tile));
        tiles[count]->tileId = atoi(tempId);
        tiles[count]->w = tileSize;
        tiles[count]->h = tileSize;
        tiles[count]->x = (count % map.width) * tileSize;
        tiles[count]->y = floor(count/map.width) * tileSize;
        tiles[count]->dynamic_object_id = -1;
        tiles[count]->tileState = fgetc(mapData) - '0'; 
        tiles[count]->cof = getCofForTile(c);
        tiles[count]->maxSpeed = getMaxSpeedForTile(c);
        tiles[count]->maxRunningSpeed = tiles[count]->maxSpeed * 2;
        count++;
      }
    } 

    char doId[4];

    // Arbitrary initialization
    char e = '!';
    int dynamic_objects_count = 0;
    while ((e == '#' || (e = fgetc(mapData))) && e != EOF) {
      // get id
      if (e == '#') {
        dynamic_objects[dynamic_objects_count] = (DynamicObject *)malloc(sizeof(DynamicObject));
        for (int p = 0; p < 3; p++) {
          doId[p] = fgetc(mapData); 
          e = doId[p];
        }
        doId[3] = '\0';

        int i = atoi(doId);
        dynamic_objects[i]->id = atoi(doId);
        if (dynamic_objects[i]->id == 0) {
          dynamic_objects[i]->isCamera  = 1;        
        } else {
          dynamic_objects[i]->isCamera = 0;
        } 
        if (dynamic_objects[i]->id == 1) {
          if (mainCharacter != NULL) {
            *dynamic_objects[i] = *mainCharacter;
          }
          dynamic_objects[i]->isMain = 1;        
        } else {
          dynamic_objects[i]->isMain = 0;
        } 

        if (starting_tile > 0 && dynamic_objects[i]->isMain) {
          dynamic_objects[i]->startingTile = starting_tile; 
          while ((e = fgetc(mapData)) && e != ';') {
            continue;
          }
        } else {
          char s[3];
          int o = 0;
          while ((e = fgetc(mapData)) && e != ';') {
            if (!isspace(e) && e != '\n') {
              s[o] = e;
              o++;
            }
          }
          dynamic_objects[i]->startingTile = atoi(s);
        }

        dynamic_objects[i]->currentTile = dynamic_objects[i]->startingTile;

        while ((e = fgetc(mapData)) && e != ';') {
          if (!isspace(e) && e != '\n') {
            dynamic_objects[i]->type = e - '0';
          }
        }
        
        ObjectType type = dynamic_objects[i]->type;

        if (type == BED) {
          int tile = dynamic_objects[dynamic_objects_count]->startingTile;
          dynamic_objects[dynamic_objects_count]->x = (tile % map.width) * tileSize;
          dynamic_objects[dynamic_objects_count]->y = ceil((tile - (map.width * 2))/map.width) * tileSize; 
        } else {
          dynamic_objects[dynamic_objects_count]->x = (dynamic_objects[i]->startingTile % map.width) * tileSize;
          dynamic_objects[dynamic_objects_count]->y = floor(dynamic_objects[i]->startingTile/map.width) * tileSize; 
        } 

        dynamic_objects[i]->startingX = dynamic_objects[i]->x;
        dynamic_objects[i]->startingY = dynamic_objects[i]->y;

        if (type == BED) {
          // Even though the current tile is in the upper right quadrant, we want
          // the bottom tiles to be interactable.
          tiles[dynamic_objects[i]->startingTile + map.width + 1]->dynamic_object_id = dynamic_objects[i]->id;
          tiles[dynamic_objects[i]->startingTile + map.width]->dynamic_object_id = dynamic_objects[i]->id;
        } else {
          tiles[dynamic_objects[i]->startingTile]->dynamic_object_id = dynamic_objects[i]->id;
        }
        tiles[dynamic_objects[i]->startingTile]->dynamic_object_type = type;
        if (type == CRATE) {
          dynamic_objects[dynamic_objects_count]->isPassable = 0;
          dynamic_objects[dynamic_objects_count]->isMovable = 1;
          dynamic_objects[dynamic_objects_count]->isLiftable = 0;
        } else if (type == JAR) {
          dynamic_objects[dynamic_objects_count]->isPassable = 0;
          dynamic_objects[dynamic_objects_count]->isMovable = 0;
          dynamic_objects[dynamic_objects_count]->isLiftable = 1;
        } else if (type == DOOR) {
          dynamic_objects[dynamic_objects_count]->isPassable = 0;
          dynamic_objects[dynamic_objects_count]->isMovable = 0;
          dynamic_objects[dynamic_objects_count]->isLiftable = 0;
        } else if (type == BED) {
          dynamic_objects[dynamic_objects_count]->isPassable = 0;
          dynamic_objects[dynamic_objects_count]->isMovable = 0;
          dynamic_objects[dynamic_objects_count]->isLiftable = 0;
        } else if (type == EVENT) {
          dynamic_objects[dynamic_objects_count]->isPassable = 1;
          dynamic_objects[dynamic_objects_count]->isMovable = 0;
          dynamic_objects[dynamic_objects_count]->isLiftable = 0;
        } else {
          dynamic_objects[dynamic_objects_count]->isPassable = 0;
          dynamic_objects[dynamic_objects_count]->isMovable = 0;
          dynamic_objects[dynamic_objects_count]->isLiftable = 0;
        }             
        while ((e = fgetc(mapData)) && e != ';') {
          if (!isspace(e) && e != '\n') {
            dynamic_objects[i]->default_behavior = e - '0';
          }
        }
        while ((e = fgetc(mapData)) && e != ';') {
          if (!isspace(e) && e != '\n') {
            dynamic_objects[i]->quest = e - '0';
          }
        }
        while ((e = fgetc(mapData)) && e != ';') {
          if (!isspace(e) && e != '\n') {
            dynamic_objects[i]->equipment.hat = e - '0';
          }
        }

        if (e == ';') {
          while (e != '*') {
            e = fgetc(mapData);
          }
        }

        int interactionIndex = 0;
        int interactions_count = 0;
        while (e == '*') {
          for (int d = 0; d < 1; d++) {
            interactionIndex = fgetc(mapData) - '0'; 
          }

          if (e == '*') { 
            e = fgetc(mapData); 
          };

          dynamic_objects[i]->interactions[interactions_count].task_count = 0;
          char data[MAX_TASK_SIZE];

          while ((e = fgetc(mapData)) && (e == '-' || e == '<' || e == '>' || e == '^' || e == 'v' || e == 'x' || e == '%' || e == '@' || e == '$' )) {
            if ( e == '-') {
              dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = SPEAK;
            } else if ( e == '<') {
              e = fgetc(mapData);
              if (e == '<') {
                dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = RUN_LEFT;
              } else {
                ungetc(e, mapData);
                dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = WALK_LEFT;
              }
            } else if ( e == '>') {
              e = fgetc(mapData);
              if (e == '>') {
                dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = RUN_RIGHT;
              } else {
                ungetc(e, mapData);
                dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = WALK_RIGHT;
              }
            } else if ( e == 'v') {
              e = fgetc(mapData);
              if (e == 'v') {
                dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = RUN_DOWN;
              } else {
                ungetc(e, mapData);
                dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = WALK_DOWN;
              }
            } else if ( e == '^') {
              e = fgetc(mapData);
              if (e == '^') {
                dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = RUN_UP;
              } else {
                dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = WALK_UP;
                ungetc(e, mapData);
              }
            } else if ( e == 'x') {
              dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = REMOVE;
            } else if ( e == '%') {
              dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = ADD_ITEM;
            } else if ( e == '@') {
              dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = LOAD_MAP;
            } else if ( e == '#') {
              dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = REMOVE_ITEM;
            } else if ( e == '$') {
              dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = EXTERNAL_COMMAND;
            }

            int j = 0;
            while ((e = fgetc(mapData)) && e != ';') {
              data[j] = e; 
              j++;
            }
            data[j] = '\0';
            for (int l = 0; l < MAX_TASKS; l++) {
              if (l == dynamic_objects[i]->interactions[interactionIndex].task_count) {
                for (int b = 0; b < MAX_TASK_SIZE; b++) {
                  dynamic_objects[i]->interactions[interactionIndex].tasks[l].data[b] = data[b];
                  if (dynamic_objects[i]->type == EVENT) {
                    /* printf("data is %s\n", dynamic_objects[i]->interactions[interactionIndex].tasks[l].data); */
                  }
                }
              }
            }
            // gross...
            e = fgetc(mapData);
            dynamic_objects[i]->interactions[interactionIndex].task_count++;
          }
          interactions_count++;
        } 
        dynamic_objects[i]->interactions_count = interactions_count;
        dynamic_objects_count++;
      }
    }

    for (int i = 0; i < map.width * map.height; i++) {
      map.tiles[i] = *tiles[i];
      free(tiles[i]);
    }

    map.dynamic_objects_count = dynamic_objects_count;

    for (int i = 0; i < map.dynamic_objects_count; i++) {
      map.dynamic_objects[i] = *dynamic_objects[i];
      map.dynamic_objects_count = dynamic_objects_count;

      free(dynamic_objects[i]);
    }

    free(tiles);
    free(dynamic_objects);
  }

  fclose(mapData);
  return map;
};


