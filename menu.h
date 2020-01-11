#include <SDL2/SDL.h>

typedef enum {
  INVENTORY
} MenuType;

typedef enum {
  DEFAULT_MENU,
  ITEM_SELECTED,
  USE_ITEM
} MenuState;

typedef struct {
  int id;
  char* name;
} MenuItem;


typedef struct {
  SDL_Color color;
  int index;
  int x;
  int y;
  int w;
  int h;
} Cursor;

typedef struct {
  MenuState state;
  int show_description;
  int active_item_index;
  int selected_item_index;
  MenuType type;
  Cursor cursor;
} Menu;

Cursor initializeMenuCursor();
