#define MAX_TILE_SIZE 5000

typedef enum {
  IS_WALKABLE,
  IS_SOLID,
} TileState;

typedef struct {
  char tileId;
  TileState tileState;
  int isOccupied;
  int x,y,w,h;
} Tile;

typedef struct {
  char* name;
  int width; // in tile units
  int height; // in tile units
  int tileSize;
  Tile tiles[MAX_TILE_SIZE];
} Map;

Map initializeMap(char* fileName, int tileSize); 

