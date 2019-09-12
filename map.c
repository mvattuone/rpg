#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

Man* getCharacterFromMap(Map *map, char* id) {
  for (int i = 0; i < map->characterCount; i++) {
    if (!strncmp(map->characters[i].id, id, sizeof map->characters[i].id)) {
      return &map->characters[i];
    }
  }

  printf("Could not find character with id %s", id);
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
    fscanf(mapData, "%d %d %d\n", &map.width, &map.height, &map.characterCount);
    Tile **tiles = malloc(map.width * map.height * sizeof(Tile));
    Man **characters = malloc(map.characterCount * sizeof(Man));
    char c;
    for(int i=0; i < map.width * map.height; i++) {
      tiles[i] = (Tile *)malloc(sizeof(Tile));
      tiles[i]->w = tileSize;
      tiles[i]->h = tileSize;
      tiles[i]->x = (i % map.width) * tileSize;
      tiles[i]->y = floor(i/map.width) * tileSize;
    }

    int count = 0;
    int characterCount = 0;
    char d;
    while ((c = fgetc(mapData)) && count < map.width * map.height) {
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
          characters[characterCount] = (Man *)malloc(sizeof(Man));
          if (d == 'x' || d == 'o') {

            if (d == 'x') {
              characters[characterCount]->isMain = 1;        
            } else if (d == 'o') {
              characters[characterCount]->isMain = 0;
            }

            char z;
            int n = 0;
            while((z = fgetc(mapData)) != '\n' && !isspace(z) && n < 3) {
              if (z == '\n' && z == ' ' && isspace(z)) {
                continue;
              }
              characters[characterCount]->id[n] = z;
              n++;
            }
          }
           
          characters[characterCount]->x = count % map.width * tileSize;
          characters[characterCount]->y = ceil(count/map.width) * tileSize; 
          characters[characterCount]->startingX = characters[characterCount]->x;
          characters[characterCount]->startingY = characters[characterCount]->y;
          characters[characterCount]->startingTile = count % map.width + ceil(count/map.width) * map.width;
          characters[characterCount]->currentTile = characters[characterCount]->startingTile;
          characterCount++;
        }
        count++;
      }
    } 

    for (int i = 0; i < map.width * map.height; i++) {
      map.tiles[i] = *tiles[i];
      free(tiles[i]);
    }

    for (int i = 0; i < map.characterCount; i++) {
      map.characters[i] = *characters[i];
      free(characters[i]);
    }

    free(tiles);
    free(characters);
  }

  fclose(mapData);
  return map;
};
