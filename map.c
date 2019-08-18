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
    fscanf(mapData, "%d %d\n", &map.width, &map.height);
    Tile **arr = malloc(map.width * map.height * sizeof(Tile));
    char c;
    for(int i=0; i < map.width * map.height; i++) {
      arr[i] = (Tile *)malloc(sizeof(Tile));
      arr[i]->w = tileSize;
      arr[i]->h = tileSize;
      arr[i]->x = (i % map.width) * tileSize;
      arr[i]->y = floor(i/map.width) * tileSize;
    }

    int count = 0;
    while ((c = fgetc(mapData)) && count < map.width * map.height) {
      if (c != '\n' && !isspace(c)) {
        arr[count]->tileId = c; 
        arr[count]->tileState = fgetc(mapData) - '0'; 
        count++;
      }

    } 

    for (int i = 0; i < map.width * map.height; i++) {
      map.tiles[i] = *arr[i];
      free(arr[i]);
    }

    fflush(stdout);
    free(arr);
  }

  fclose(mapData);
  return map;
};
