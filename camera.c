#include "camera.h"
#include "map.h"
#include "dynamic_object.h"
#include "rpg.h"

void set_current_target(Map *map, Camera *camera, DynamicObject *target, time_t time) {
  if (target->id == camera->current_target->id) {
    puts("selected the same target");  
    return;
  }
  camera->current_target = target;
  return;
}

