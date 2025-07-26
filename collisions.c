#include <stdio.h>
#include "collisions.h"
#include "game.h"
#include "map.h"
#include "dynamic_object.h"

void detectCollision(Game *game, DynamicObject *active_dynamic_object, Target *target) {
  float mainX = active_dynamic_object->x;
  float mainY = active_dynamic_object->y;
  float mainDx = active_dynamic_object->dx;
  float mainDy = active_dynamic_object->dy;
  int mainW = active_dynamic_object->w;
  int mainH = active_dynamic_object->h;

  if (mainX + mainW / 2 > target->x && mainX + mainW/ 2 < target->x+target->w) {
    if (mainY < target->h+target->y && mainY > target->y && mainDy < 0) {
      active_dynamic_object->y = target->y+target->h;
      active_dynamic_object->dy = 0;
    }
  }

  if (mainX + mainW / 2 > target->x && mainX<target->x+target->w) {
    if (mainY + mainH > target->y && mainY < target->y && mainDy > 0) {
      active_dynamic_object->y = target->y-mainH;
      if (!game->mainCharacter->isPushing) {
        active_dynamic_object->dy = 0;
      }
    }
  }

  if (mainY + mainH/2 > target->y && mainY<target->y+target->h) {
    if (mainX < target->x+target->w && mainX+mainW > target->x+target->w && mainDx < 0) {
      active_dynamic_object->x = target->x + target->w;
      active_dynamic_object->dx = 0;
    } else if (mainX+mainW > target->x && mainX < target->x && mainDx > 0) {
      active_dynamic_object->x = target->x - mainW;
      active_dynamic_object->dx = 0;
    }
  }
}

void detectObjectCollision(Game *game, DynamicObject *active_dynamic_object, DynamicObject *target_object) {
  Target target = { .x=target_object->x, .y=target_object->y, .w=target_object->w, .h=target_object->h};
  detectCollision(game, active_dynamic_object, &target);
}

void detectTileCollision(Game *game, DynamicObject *active_dynamic_object, Tile *tile) {
  Target target = { .x=tile->x, .y=tile->y, .w=tile->w, .h=tile->h};
  detectCollision(game, active_dynamic_object, &target);
}

void handleObjectCollisions(Game *game, DynamicObject *active_dynamic_object) {
  for (int y = -game->camera.base->y/game->current_map->tileSize; y < (-game->camera.base->y + WINDOW_HEIGHT)/ game->current_map->tileSize; y++)
    for (int x = -game->camera.base->x/game->current_map->tileSize; x < (-game->camera.base->x + WINDOW_WIDTH)/ game->current_map->tileSize; x++) {
    int tileIndex = x + y * game->current_map->width;
    if (tileIndex < 0) continue;

    int *previousMainTile = NULL;
    for (int i = 0; i < game->current_map->dynamic_objects_count; i++) {
      float objectX = game->current_map->dynamic_objects[i].x;
      float objectY = game->current_map->dynamic_objects[i].y;
      float objectW = game->current_map->dynamic_objects[i].w;
      float objectH = game->current_map->dynamic_objects[i].h;
      int doIndexX = (objectX + objectW/2)/game->current_map->tileSize;
      int doIndexY = (objectY + objectH/2)/game->current_map->tileSize;
      int previousTile = game->current_map->dynamic_objects[i].currentTile;
      if (game->current_map->dynamic_objects[i].isMain) {
        previousMainTile = &previousTile;
      }
      game->current_map->dynamic_objects[i].currentTile = doIndexX + doIndexY * game->current_map->width;

      if (previousTile != game->current_map->dynamic_objects[i].currentTile && game->status == IS_ACTIVE) {
        if (game->current_map->tiles[game->current_map->dynamic_objects[i].currentTile].dynamic_object_type != DOOR && game->current_map->tiles[game->current_map->dynamic_objects[i].currentTile].dynamic_object_type != EVENT) {
          game->current_map->tiles[game->current_map->dynamic_objects[i].currentTile].dynamic_object_id = game->current_map->dynamic_objects[i].id;
        }
        if (game->current_map->tiles[previousTile].dynamic_object_type != DOOR && game->current_map->tiles[previousTile].dynamic_object_type != EVENT) {
          game->current_map->tiles[previousTile].dynamic_object_id = 0;
        }
      }

      if (x >= 0 && x < game->current_map->width && y>= 0 && y < game->current_map->height) {
        int tileIsSolid = game->current_map->tiles[tileIndex].tileState == IS_SOLID;
        int tileHasObject = game->current_map->tiles[tileIndex].dynamic_object_id >= 0;
        int tileHasEvent = game->current_map->tiles[tileIndex].dynamic_object_type == EVENT;
        if (game->status != IS_CUTSCENE && tileHasEvent && tileIndex == game->mainCharacter->currentTile && previousMainTile > 0 && *previousMainTile != game->mainCharacter->currentTile) {
          game->mainCharacter->currentTile = 0;
          DynamicObject *event = getDynamicObjectFromMap(game->current_map, game->current_map->tiles[tileIndex].dynamic_object_id);
          triggerEvent(game, event);
          return;
        }

        int isNotSelf = game->current_map->tiles[tileIndex].dynamic_object_id != active_dynamic_object->id;

        if (game->current_map->dynamic_objects[i].isMovable && !game->mainCharacter->isPushing)  {
          game->current_map->dynamic_objects[i].isMoving = 0;
          game->current_map->dynamic_objects[i].moveUp = 0;
          game->current_map->dynamic_objects[i].moveLeft = 0;
          game->current_map->dynamic_objects[i].moveRight = 0;
          game->current_map->dynamic_objects[i].moveDown = 0;
        }

        if (tileIsSolid && !tileHasObject && !active_dynamic_object->isCamera) {
          detectTileCollision(game, active_dynamic_object, &game->current_map->tiles[tileIndex]);
        }

        if (game->status == IS_ACTIVE && (game->current_map->dynamic_objects[i].isPassable == 0 || active_dynamic_object->isCamera)) {
          detectObjectCollision(game, active_dynamic_object, &game->current_map->dynamic_objects[i]);
        }

        if (tileHasObject && isNotSelf) {
          int tileIsAboveObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile - game->current_map->width && active_dynamic_object->direction == UP;
          int tileIsBelowObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile + game->current_map->width && active_dynamic_object->direction == DOWN;
          int tileIsToLeftOfObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile - 1 && active_dynamic_object->direction == LEFT;
          int tileIsToRightOfObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile + 1 && active_dynamic_object->direction == RIGHT;

          int tileIsAboveAndToLeftOfObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile - game->current_map->width - 1 && active_dynamic_object->direction == UPLEFT;
          int tileIsBelowAndToLeftOfObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile + game->current_map->width - 1 && active_dynamic_object->direction == DOWNLEFT;
          int tileIsAboveAndToRightOfObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile - game->current_map->width + 1 && active_dynamic_object->direction == UPRIGHT;
          int tileIsBelowAndToRightOfObject = game->current_map->dynamic_objects[i].currentTile == active_dynamic_object->currentTile + game->current_map->width + 1 && active_dynamic_object->direction == DOWNRIGHT;

          if (game->current_map->dynamic_objects[i].isLiftable && active_dynamic_object->isLifting)  {
            if (tileIsAboveObject || tileIsBelowObject || tileIsToLeftOfObject || tileIsToRightOfObject || tileIsAboveAndToLeftOfObject || tileIsBelowAndToLeftOfObject || tileIsAboveAndToRightOfObject || tileIsBelowAndToRightOfObject) {
              game->current_map->dynamic_objects[i].isLifted = 1;
              active_dynamic_object->has_object = 1;
              game->current_map->dynamic_objects[i].isPassable = 1;
            }
          }

          if (game->current_map->dynamic_objects[i].isMovable && active_dynamic_object->isPushing)  {
            game->current_map->dynamic_objects[i].isMoving = active_dynamic_object->isPushing ? 1 : 0;
            game->current_map->dynamic_objects[i].moveLeft = tileIsToLeftOfObject ? 1 : 0;
            game->current_map->dynamic_objects[i].moveUp = tileIsAboveObject ? 1 : 0;
            game->current_map->dynamic_objects[i].moveRight = tileIsToRightOfObject ? 1 : 0;
            game->current_map->dynamic_objects[i].moveDown = tileIsBelowObject ? 1 : 0;
          }
        }
      }
    }
  }
}

