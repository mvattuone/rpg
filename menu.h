#include <SDL2/SDL.h>

typedef enum {
  INVENTORY
} MenuType;


typedef struct {
  SDL_Color color;
  int x;
  int y;
  int w;
  int h;
} Cursor;

typedef struct {
  int show_description;
  int active_item_index;
  MenuType type;
  Cursor cursor;
  char* menu_items[]; 
} Menu;
