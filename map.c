#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "map.h"

Map initializeMap(char* fileName, int tileSize) {
  Map map;
  map.name = fileName;
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
          printf("at first %c\n", d);
          fflush(stdout);
          if (d == 'x' || d == 'o') {
            printf("%c\n", d);
            fflush(stdout);

            if (d == 'x') {
              characters[characterCount]->isMain = 1;        
            } else if (d == 'o') {
              characters[characterCount]->isMain = 0;
            }

            char z;
            int n = 0;
            while((z = fgetc(mapData)) != '\n' && !isspace(z) && n < 3) {
              printf("z ist %c\n", z);
              fflush(stdout);
              if (z == '\n' && z == ' ' && isspace(z)) {
                continue;
              }
              characters[characterCount]->id[n] = z;
              n++;
            }
          }
           
          characters[characterCount]->x = count % map.width * tileSize;
          characters[characterCount]->y = ceil(count/map.width) * tileSize; 
          characters[characterCount]->currentTile = count % map.width + ceil(count/map.width) * map.width;
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
      printf("ismain %d\n", characters[i]->isMain);
      fflush(stdout);
      map.characters[i] = *characters[i];
      free(characters[i]);
    }

    free(tiles);
    free(characters);
  }

  fclose(mapData);
  return map;
};
