#include <stdlib.h>
#include "ai.h"
#include "dynamic_object.h"

void process_default_behavior(DynamicObject *dynamic_object, Map *map) {
  if (dynamic_object->default_behavior == WALKING && dynamic_object->task_queue.size == 0 && !dynamic_object->isMain) {
      int randomNumber = rand() % 4;

      if (randomNumber == 0 && map->tiles[dynamic_object->currentTile - map->width].tileState != IS_SOLID) {
        enqueue(&dynamic_object->task_queue, (void*)&walkUp, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 1 && map->tiles[dynamic_object->currentTile + 1].tileState != IS_SOLID) {
        enqueue(&dynamic_object->task_queue, (void*)&walkRight, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 2 && map->tiles[dynamic_object->currentTile + map->width].tileState != IS_SOLID) {
        enqueue(&dynamic_object->task_queue, (void*)&walkDown, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 3 && map->tiles[dynamic_object->currentTile - 1].tileState != IS_SOLID) {
        enqueue(&dynamic_object->task_queue, (void*)&walkLeft, (void*)1, (void*)&map->tileSize, NULL);
      }
    }

  if (dynamic_object->default_behavior == RUNNING && dynamic_object->task_queue.size == 0 && !dynamic_object->isMain) {
      int randomNumber = rand() % 4;

      if (randomNumber == 0) {
        enqueue(&dynamic_object->task_queue, (void*)&runUp, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 1) {
        enqueue(&dynamic_object->task_queue, (void*)&runRight, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 2) {
        enqueue(&dynamic_object->task_queue, (void*)&runDown, (void*)1, (void*)&map->tileSize, NULL);
      } else if (randomNumber == 4) {
        enqueue(&dynamic_object->task_queue, (void*)&runLeft, (void*)1, (void*)&map->tileSize, NULL);
      }
    }
}

