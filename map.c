#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

#define MAX_DIALOGUE 3

DynamicObject * getDynamicObjectFromMap(Map *map, char* id) {
  for (int i = 0; i < map->dynamic_objects_count; i++) {
    if (!strcmp(map->dynamic_objects[i].id, id)) {
      return &map->dynamic_objects[i];
    }
  }

  printf("Could not find dynamic object with id %s", id);
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
    for(int i=0; i < map.width * map.height; i++) {
      tiles[i] = (Tile *)malloc(sizeof(Tile));
      tiles[i]->w = tileSize;
      tiles[i]->h = tileSize;
      tiles[i]->x = (i % map.width) * tileSize;
      tiles[i]->y = floor(i/map.width) * tileSize;
      for (int j = 0; j < 3; j++) {
        tiles[i]->dynamic_object_id[j] = '0';
      }
    }

    int count = 0;
    int dynamic_objects_count = 0;
    char d;
    while (count < map.width * map.height && (c = fgetc(mapData))) {
      if (c != '\n' && c != ' ' && !isspace(c)) {
        tiles[count]->tileId = c; 
        tiles[count]->tileState = fgetc(mapData) - '0'; 
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
        } else if ((d = fgetc(mapData)) == 'x' || d == 'o') {
          dynamic_objects[dynamic_objects_count] = (DynamicObject *)malloc(sizeof(DynamicObject));
          if (d == 'x' || d == 'o') {

            if (d == 'x') {
              dynamic_objects[dynamic_objects_count]->isMain = 1;        
            } else if (d == 'o') {
              dynamic_objects[dynamic_objects_count]->isMain = 0;
            }

            char z;
            int n = 0;
            while((z = fgetc(mapData)) != '\n' && !isspace(z) && n < 3) {
              if (z == ' ') {
                continue;
              }
              dynamic_objects[dynamic_objects_count]->id[n] = z;
              n++;
            }
            for (int ao=0; ao < 4; ao++) {
              tiles[count]->dynamic_object_id[ao] = dynamic_objects[dynamic_objects_count]->id[ao];
            }
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
    Dialogue *dialogues[dynamic_objects_count];
    for (int i = 0; i < dynamic_objects_count; i++) {
      dialogues[i] = malloc(sizeof(Dialogue));
      for (int j = 0; j < MAX_DIALOGUE_LINES; j++) {
        dialogues[i]->lines[j][MAX_DIALOGUE_SIZE-1] = '\0'; 
      }
    }
    for (int i = 0; i < dynamic_objects_count; i++) {
      char e;
      while ((e = fgetc(mapData)) && e == '#') {
        // get id
        for (int p = 0; p < 3; p++) {
          doId[p] = fgetc(mapData); 
        }

        if (!strncmp(doId, dynamic_objects[i]->id, 3) && !dynamic_objects[i]->isMain) {
          while ((e = fgetc(mapData)) && e != ';') {
            if (!isspace(e) && e != '\n') {
              dynamic_objects[i]->default_behavior = e - '0';
            }
          }

          if (e == ';') {
            e = fgetc(mapData);
          }

          int dialogueIndex = 0;
          int dialogue_index_total = 0;
          while ((e = fgetc(mapData)) && e == '*') {

            for (int d = 0; d < 1; d++) {
              dialogueIndex = fgetc(mapData) - '0'; 
            }

            if (e == '*') { 
              e = fgetc(mapData); 
            };
            dialogues[i][dialogue_index_total].line_count = 0;
            char line[MAX_DIALOGUE_SIZE];

            while ((e = fgetc(mapData)) && e == '-') {
              int j = 0;
              while ((e = fgetc(mapData)) && e != ';') {
                line[j] = e; 
                j++;
              }
              line[j+1]='\0';
              for (int l = 0; l < MAX_DIALOGUE_LINES; l++) {
                if (l == dialogues[i][dialogueIndex].line_count) {
                  for (int b = 0; b < MAX_DIALOGUE_SIZE; b++) {
                    dialogues[i][dialogueIndex].lines[l][b] = line[b];
                  }
                }
              }
              dialogues[i][dialogueIndex].line_count++; 
              // gross...
              e = fgetc(mapData);
              e = fgetc(mapData);
              if (e == '$') break;
            }
            dialogue_index_total++;
            if (e == '$') break;
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
      for (int m = 0; m < map.dynamic_objects_count - 1; m++) {
        if (!strcmp(map.dynamic_objects[i].id, dialogues[m]->id)) {
            map.dynamic_objects[i] = *dynamic_objects[i];
        }
      }

      if (!map.dynamic_objects[i].isMain) {
        for (int j = 0; j < sizeof(*dialogues[i])/sizeof(dialogues[i][0]); j++) {
          map.dynamic_objects[i].dialogues[j] = dialogues[i][j];
          fflush(stdout);
        }
      }

      free(dynamic_objects[i]);
    }

    free(tiles);
    fflush(stdout);
    free(dynamic_objects);
    fflush(stdout);
  }

  fclose(mapData);
  return map;
};
