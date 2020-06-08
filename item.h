#ifndef ITEM_H
#define ITEM_H

typedef struct {
  int id;
  char *description;
  char *name;
} Item;

Item* load_items(char *file_name, int *inventory_count);

#endif
