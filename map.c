#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

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

  printf("Could not find dynamic object with id %d", id);
  SDL_Quit();
  exit(1);
}

Map initializeMap(char* fileName, int tileSize, int startingTile) {
  Map map;
  strncpy(map.name, fileName, 12);
  map.tileSize = tileSize;

  FILE *mapData = fopen(fileName, "r");

  if (mapData == NULL) {
    printf("Map could not be loaded.");
    exit(1);
  } else {
    fscanf(mapData, "%d %d %d\n", &map.width, &map.height, &map.dynamic_objects_count);
    Tile **tiles = malloc(map.width * map.height * sizeof(Tile));
    DynamicObject **dynamic_objects = malloc(map.dynamic_objects_count * sizeof(DynamicObject));
    char c;
    int count = 0;
    int dynamic_objects_count = 0;
    char d;
    while (count < map.width * map.height && (c = fgetc(mapData))) {
      if (c != '\n' && c != ' ' && !isspace(c)) {
        char tempId[2];
        tempId[0] = c;
        tempId[1] = fgetc(mapData);
        tiles[count] = malloc(sizeof(Tile));
        tiles[count]->tileId = atoi(tempId);
        tiles[count]->w = tileSize;
        tiles[count]->h = tileSize;
        tiles[count]->x = (count % map.width) * tileSize;
        tiles[count]->y = floor(count/map.width) * tileSize;
        tiles[count]->dynamic_object_id = 0;
        tiles[count]->tileState = fgetc(mapData) - '0'; 
        tiles[count]->cof = getCofForTile(c);
        tiles[count]->maxSpeed = getMaxSpeedForTile(c);
        tiles[count]->maxRunningSpeed = tiles[count]->maxSpeed * 2;
        if (tiles[count]->tileState == IS_TELEPORT) {
          char mapId[2] = {0, 0};
          int n = 0;
          while (n < 2) {
            char e = fgetc(mapData);
            if (e == '\n' && e == ' ' && isspace(e)) {
              continue;
            }
            mapId[n] = e; 
            n++;
          }
          char tileId[2];
          int q = 0;
          while (q <= 2) {
            char e = fgetc(mapData);
            if (e == '\n' && e == ' ' && isspace(e)) {
              continue;
            }
            tileId[q] = e; 
            q++;
          }
          snprintf(tiles[count]->teleportTo, sizeof tiles[count]->teleportTo, "map_%.2s.lvl", mapId);
          tiles[count]->teleportTile = atoi(tileId);
        } else if ((d = fgetc(mapData)) == 'x' || d == 'o' || d == 's' || d == 'j' || d == 'b' || d == 'd') {
          dynamic_objects[dynamic_objects_count] = (DynamicObject *)malloc(sizeof(DynamicObject));
          if (d == 'x' || d == 'o' || d == 's' || d == 'j' || d == 'b' || d == 'd') {

            if (d == 's') {
              dynamic_objects[dynamic_objects_count]->isPassable = 0;
              dynamic_objects[dynamic_objects_count]->isMovable = 1;
              dynamic_objects[dynamic_objects_count]->isLiftable = 0;
              dynamic_objects[dynamic_objects_count]->type = CRATE;
            } else if (d == 'j') {
              dynamic_objects[dynamic_objects_count]->isPassable = 0;
              dynamic_objects[dynamic_objects_count]->isMovable = 0;
              dynamic_objects[dynamic_objects_count]->isLiftable = 1;
              dynamic_objects[dynamic_objects_count]->type = JAR;
            } else if (d == 'd') {
              dynamic_objects[dynamic_objects_count]->isPassable = 0;
              dynamic_objects[dynamic_objects_count]->isMovable = 0;
              dynamic_objects[dynamic_objects_count]->isLiftable = 0;
              dynamic_objects[dynamic_objects_count]->type = DOOR;
            } else if (d == 'b') {
              dynamic_objects[dynamic_objects_count]->isPassable = 0;
              dynamic_objects[dynamic_objects_count]->isMovable = 0;
              dynamic_objects[dynamic_objects_count]->isLiftable = 0;
              dynamic_objects[dynamic_objects_count]->type = BED;
            } else {
              dynamic_objects[dynamic_objects_count]->isPassable = 0;
              dynamic_objects[dynamic_objects_count]->isMovable = 0;
              dynamic_objects[dynamic_objects_count]->isLiftable = 0;
              dynamic_objects[dynamic_objects_count]->type = MAN;
            }

            if (d == 'x') {
              dynamic_objects[dynamic_objects_count]->isMain = 1;        
            } else {
              dynamic_objects[dynamic_objects_count]->isMain = 0;
            } 

            char z;
            char doId[3];
            int n = 0;
            while((z = fgetc(mapData)) != '\n' && !isspace(z) && n < 3) {
              if (z == ' ') {
                continue;
              }
              doId[n] = z;
              n++;
            }
            dynamic_objects[dynamic_objects_count]->id = atoi(doId);
            tiles[count]->dynamic_object_id = dynamic_objects[dynamic_objects_count]->id;
            tiles[count]->dynamic_object_type = dynamic_objects[dynamic_objects_count]->type;
          } 

          if (dynamic_objects[dynamic_objects_count]->type == BED) {
            dynamic_objects[dynamic_objects_count]->x = ((count % map.width) - 1) * tileSize;
            dynamic_objects[dynamic_objects_count]->y = ceil((count - (map.width * 2))/map.width) * tileSize; 
          } else if (dynamic_objects[dynamic_objects_count]->isMain && startingTile) {
            dynamic_objects[dynamic_objects_count]->x = ((startingTile % map.width) - 1) * tileSize;
            dynamic_objects[dynamic_objects_count]->y = ceil((startingTile - (map.width * 2))/map.width) * tileSize; 
          } else {
            dynamic_objects[dynamic_objects_count]->x = count % map.width * tileSize;
            dynamic_objects[dynamic_objects_count]->y = ceil(count/map.width) * tileSize; 
          }
          dynamic_objects[dynamic_objects_count]->startingX = dynamic_objects[dynamic_objects_count]->x;
          dynamic_objects[dynamic_objects_count]->startingY = dynamic_objects[dynamic_objects_count]->y;
          if (dynamic_objects[dynamic_objects_count]->isMain && startingTile) {
            dynamic_objects[dynamic_objects_count]->startingTile = startingTile % map.width + ceil(count/(map.width*2)) * map.width;
          } else {
            dynamic_objects[dynamic_objects_count]->startingTile = count % map.width + ceil(count/(map.width*2)) * map.width;
          }
          dynamic_objects[dynamic_objects_count]->currentTile = dynamic_objects[dynamic_objects_count]->startingTile;
          dynamic_objects_count++;
        }
        count++;
      }
    } 

    char doId[3];

    // Arbitrary initialization
    char e = '!';
    while ((e == '#' || (e = fgetc(mapData))) && e != EOF) {
      // get id
      if (e == '#') {
        for (int p = 0; p < 3; p++) {
          doId[p] = fgetc(mapData); 
          e = doId[p];
        }

        for (int i = 0; i < dynamic_objects_count; i++) {
          if (atoi(doId) == dynamic_objects[i]->id && !dynamic_objects[i]->isMain) {
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

              while ((e = fgetc(mapData)) && (e == '-' || e == '<' || e == '>' || e == '^' || e == 'v' || e == 'x' || e == '%')) {
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
                } else if ( e == '#') {
                  dynamic_objects[i]->interactions[interactionIndex].tasks[dynamic_objects[i]->interactions[interactions_count].task_count].type = REMOVE_ITEM;
                }

                int j = 0;
                while ((e = fgetc(mapData)) && e != ';') {
                  data[j] = e; 
                  j++;
                }
                data[j+1]='\0';
                for (int l = 0; l < MAX_TASKS; l++) {
                  if (l == dynamic_objects[i]->interactions[interactionIndex].task_count) {
                    for (int b = 0; b < MAX_TASK_SIZE; b++) {
                      dynamic_objects[i]->interactions[interactionIndex].tasks[l].data[b] = data[b];
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
          }
        }
      }
    }

    for (int i = 0; i < map.width * map.height; i++) {
      map.tiles[i] = *tiles[i];
      free(tiles[i]);
    }

    for (int i = 0; i < map.dynamic_objects_count; i++) {
      map.dynamic_objects[i] = *dynamic_objects[i];
      for (int m = 0; m < map.dynamic_objects_count; m++) {
        map.dynamic_objects[i] = *dynamic_objects[i];
      }

      free(dynamic_objects[i]);
    }

    free(tiles);
    free(dynamic_objects);
  }

  fclose(mapData);
  return map;
};
