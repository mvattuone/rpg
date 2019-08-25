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
      if (c != '\n' && !isspace(c)) {
        tiles[count]->tileId = c; 
        tiles[count]->tileState = fgetc(mapData) - '0'; 
        if ((d = fgetc(mapData)) != '\n' && !isspace(d)) {
          characters[characterCount] = (Man *)malloc(sizeof(Man));
          characters[characterCount]->id = d;
          characters[characterCount]->x = count % map.width * tileSize;
          characters[characterCount]->y = ceil(count/map.width) * tileSize; 
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
