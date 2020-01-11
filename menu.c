#include "menu.h"

Cursor initializeMenuCursor() {
  int x = 0;
  int y = 0;
  int w = 20;
  int h = 40;
  int index = 0;
  Cursor cursor = { .x=x, .y=y, .w=w, .h=h, .index=index };
  return cursor;
}
