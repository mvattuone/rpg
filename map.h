#define MAX_TILE_SIZE 5000

typedef struct {
  char tileId;
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

