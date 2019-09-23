#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

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
    DynamicObject **dynamic_objects = malloc(map.dynamic_objects_count * sizeof(DynamicObject ));
    char c;
    for(int i=0; i < map.width * map.height; i++) {
      tiles[i] = (Tile *)malloc(sizeof(Tile));
      tiles[i]->w = tileSize;
      tiles[i]->h = tileSize;
      tiles[i]->x = (i % map.width) * tileSize;
      tiles[i]->y = floor(i/map.width) * tileSize;
      tiles[i]->dynamic_object_id = 0;
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
          dynamic_objects[dynamic_objects_count] = (DynamicObject *)malloc(sizeof(DynamicObject ));
          if (d == 'x' || d == 'o') {

            if (d == 'x') {
              dynamic_objects[dynamic_objects_count]->isMain = 1;        
            } else if (d == 'o') {
              dynamic_objects[dynamic_objects_count]->isMain = 0;
            }

            char z;
            int n = 0;
            while((z = fgetc(mapData)) != '\n' && !isspace(z) && n < 3) {
              if (z == '\n' && z == ' ' && isspace(z)) {
                continue;
              }
              dynamic_objects[dynamic_objects_count]->id[n] = z;
              n++;
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
    while((c = fgetc(mapData)) != ' ' && !isspace(c)) {
      if (c == '#') {
        for (int i = 0; i < 3; i++) {
          doId[i] = fgetc(mapData); 
        }
        for (int i = 0; i < map.dynamic_objects_count; i++) {
          if (!strncmp(doId, dynamic_objects[i]->id, 3)) {
            fscanf(mapData, "%d\n", &dynamic_objects[i]->default_behavior);
          }
        }
      } else {
        break;
      }
    }

    for (int i = 0; i < map.width * map.height; i++) {
      map.tiles[i] = *tiles[i];
      free(tiles[i]);
    }

    for (int i = 0; i < map.dynamic_objects_count; i++) {
      map.dynamic_objects[i] = *dynamic_objects[i];
      free(dynamic_objects[i]);
    }

    free(tiles);
    free(dynamic_objects);
  }

  fclose(mapData);
  return map;
};
