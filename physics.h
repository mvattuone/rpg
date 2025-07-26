#ifndef PHYSICS_H
#define PHYSICS_H

#define PIXELS_PER_METER 16 // px / m
#define GRAVITY 9.8

struct DynamicObject;
struct Tile;
struct Game;

float accelerate(float v, float a, float dt);
int handlePhysics(struct DynamicObject *dynamic_object, struct Tile *currentTile, float *dt, struct Game *game);
#endif
