#include "dynamic_object.h"
#define MAX_TILE_SIZE 5000
#define MAX_DYNAMIC_OBJECTS 100

typedef enum {
  IS_WALKABLE,
  IS_SOLID,
  IS_TELEPORT
} TileState;

typedef struct {
  char tileId;
  TileState tileState;
  int dynamic_object_id;
  int x,y,w,h;
  char teleportTo[20];
} Tile;

typedef struct {
  char name[12];
  int width; // in tile units
  int height; // in tile units
  int tileSize;
  Tile tiles[MAX_TILE_SIZE];
  DynamicObject dynamic_objects[MAX_DYNAMIC_OBJECTS];
  int dynamic_objects_count;
} Map;

DynamicObject * getDynamicObjectFromMap(Map *map, int id);
Map initializeMap(char* fileName, int tileSize); 

