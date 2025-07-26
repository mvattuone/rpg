#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "game.h"
#include "dynamic_object.h"
#include "map.h"
#include "rpg.h"

typedef struct {
  int w;
  int h;
  float x;
  float y;
} Target;

void detectCollision(Game *game, DynamicObject *active_dynamic_object, Target *target);
void detectObjectCollision(Game *game, DynamicObject *active_dynamic_object, DynamicObject *target_object);
void detectTileCollision(Game *game, DynamicObject *active_dynamic_object, Tile *tile);
void handleObjectCollisions(Game *game, DynamicObject *active_dynamic_object);

#endif
