#include "dynamic_object.h"

#ifndef MAP_H
#define MAP_H

#define MAX_TILE_SIZE 5000
#define MAX_DYNAMIC_OBJECTS 13


typedef enum {
  IS_WALKABLE,
  IS_SOLID,
  IS_ABOVE
} TileState;

typedef struct {
  int tileId;
  int tileIndex;
  TileState tileState;
  float cof;
  float maxSpeed;
  float maxRunningSpeed;
  int dynamic_object_id;
  ObjectType dynamic_object_type;
  int x,y,w,h;
} Tile;

typedef struct {
  char name[50];
  int width; // in tile units
  int height; // in tile units
  int tileSize;
  Tile tiles[MAX_TILE_SIZE];
  DynamicObject dynamic_objects[MAX_DYNAMIC_OBJECTS];
  int dynamic_objects_count;
} Map;

DynamicObject * getDynamicObjectFromMap(Map *map, int id);
Map initializeMap(char* filePath, int tileSize, int startingTile, DynamicObject *mainCharacter); 

#endif
