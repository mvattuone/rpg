#include "dynamicObject.h"
#define MAX_TILE_SIZE 5000
#define MAX_CHARACTERS 255

typedef enum {
  IS_WALKABLE,
  IS_SOLID,
  IS_TELEPORT
} TileState;

typedef struct {
  char tileId;
  TileState tileState;
  int x,y,w,h;
  char teleportTo[20];
} Tile;

typedef struct {
  char name[12];
  int width; // in tile units
  int height; // in tile units
  int tileSize;
  Tile tiles[MAX_TILE_SIZE];
  Man characters[MAX_CHARACTERS];
  int characterCount;
} Map;

Man* getCharacterFromMap(Map *map, char* id);
Map initializeMap(char* fileName, int tileSize); 

