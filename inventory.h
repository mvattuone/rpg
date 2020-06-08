#include "dynamic_object.h"
#include "utils.h"
#include "game.h"

#ifndef INVENTORY_H
#define INVENTORY_H

int addToInventory(DynamicObject *dynamic_object, int inventory_id, DynamicArray *inventory); 
DynamicArray removeFromInventory(DynamicObject *dynamic_object, Game *game, int inventory_id); 
Menu load_inventory_menu();

#endif
