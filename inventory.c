#include "inventory.h"

// @TODO Pass items in this way?
// Might be good if we have different 
// inventories per character...
Menu load_inventory_menu() {
  Menu inventory_menu = {
   0 
  } ;
  inventory_menu.show_description = 0;
  inventory_menu.type = INVENTORY;
  inventory_menu.active_item_index = 0;
  inventory_menu.selected_item_index = -1;
  inventory_menu.cursor = initializeMenuCursor();
  return inventory_menu;
}

int addToInventory(DynamicObject *dynamic_object, int inventory_id, DynamicArray *inventory) {
  if (&inventory->size > &inventory->capacity) {
    inventory->items = realloc(inventory->items, (sizeof(inventory->items) * sizeof(Item)) + sizeof(Item));
    inventory->capacity = sizeof(inventory->items) + sizeof(Item);
  }

  inventory->items[inventory->size] = inventory_id;
  inventory->size++; 
  return 0;
}

DynamicArray removeFromInventory(DynamicObject *dynamic_object, Game *game, int inventory_id) {
  DynamicArray new_inventory; 
  new_inventory.items = malloc((sizeof(game->inventory.items) * sizeof(Item)) - sizeof(Item));
  new_inventory.capacity = (sizeof(new_inventory.items) * sizeof(Item)) - sizeof(Item);
  new_inventory.size = game->inventory.size - 1;
  for (int i = 0; i < game->inventory.size; i++) {
    if (game->inventory.items[i] != inventory_id) {
      new_inventory.items[i] = game->inventory.items[i];
    }
  }


  free(game->inventory.items);
  return new_inventory;
}
