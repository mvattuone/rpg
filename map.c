#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

#define MAX_DIALOGUE 3

// We'll probably want to have the various tiles
// and their properties stored somewhere. Like a JSON file.
float getCofForTile(char id) {
  float cof;
  if (id == '.') { 
    cof = 0.50;
  } else if (id == '*') {
    cof = 0.05;
  } else if (id == '#') {
    cof = 0.50;
  } else {
    cof = 100.0;
  }

  return cof;
}

float getMaxSpeedForTile(char id) {
  float maxSpeed;

  if (id == '.') { 
    maxSpeed = 3.00;
  } else if (id == '*') {
    maxSpeed = 5.00;
  } else if (id == '#') {
    maxSpeed = 1.50;
  } else {
    maxSpeed = 0;
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

Map initializeMap(char* fileName, int tileSize) {
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
        tiles[count] = (Tile *)malloc(sizeof(Tile));
        tiles[count]->tileId = c; 
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
          snprintf(tiles[count]->teleportTo, sizeof tiles[count]->teleportTo, "map_%.2s.lvl", mapId);
        } else if ((d = fgetc(mapData)) == 'x' || d == 'o' || d == 's') {
          dynamic_objects[dynamic_objects_count] = (DynamicObject *)malloc(sizeof(DynamicObject));
          if (d == 'x' || d == 'o' || d == 's') {

            if (d == 's') {
              dynamic_objects[dynamic_objects_count]->isMovable = 1;
              dynamic_objects[dynamic_objects_count]->type = CRATE;
            } else {
              dynamic_objects[dynamic_objects_count]->isMovable = 0;
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
          } 

          dynamic_objects[dynamic_objects_count]->x = count % map.width * tileSize;
          dynamic_objects[dynamic_objects_count]->y = ceil(count/map.width) * tileSize; 
          dynamic_objects[dynamic_objects_count]->startingX = dynamic_objects[dynamic_objects_count]->x;
          dynamic_objects[dynamic_objects_count]->startingY = dynamic_objects[dynamic_objects_count]->y;
          dynamic_objects[dynamic_objects_count]->startingTile = count % map.width + ceil(count/map.width) * map.width;
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

            if (e == ';') {
              while (e != '*') {
                e = fgetc(mapData);
              }
            }

            int dialogueIndex = 0;
            int dialogue_index_total = 0;
            while (e == '*') {
              for (int d = 0; d < 1; d++) {
                dialogueIndex = fgetc(mapData) - '0'; 
              }

              if (e == '*') { 
                e = fgetc(mapData); 
              };

              dynamic_objects[i]->dialogues[dialogue_index_total].line_count = 0;
              char line[MAX_DIALOGUE_SIZE];

              while ((e = fgetc(mapData)) && e == '-') {
                int j = 0;
                while ((e = fgetc(mapData)) && e != ';') {
                  line[j] = e; 
                  j++;
                }
                line[j+1]='\0';
                for (int l = 0; l < MAX_DIALOGUE_LINES; l++) {
                  if (l == dynamic_objects[i]->dialogues[dialogueIndex].line_count) {
                    for (int b = 0; b < MAX_DIALOGUE_SIZE; b++) {
                      dynamic_objects[i]->dialogues[dialogueIndex].lines[l][b] = line[b];
                    }
                  }
                }
                dynamic_objects[i]->dialogues[dialogueIndex].line_count++; 
                // gross...
                e = fgetc(mapData);
              }
              dialogue_index_total++;
            } 
            dynamic_objects[i]->dialogue_index_total = dialogue_index_total;
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
