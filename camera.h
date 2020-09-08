#ifndef CAMERA_H
#define CAMERA_H
#include "dynamic_object.h"
#include "map.h"

typedef struct {
  DynamicObject* base;
  DynamicObject* current_target;
} Camera;

void set_current_target(Map *map, Camera *camera, DynamicObject *target, time_t time);

#endif
