#include "physics.h"
#include "dynamic_object.h"
#include "map.h"
#include "game.h"

float accelerate(float v, float a, float dt) { 
  v += a * dt;
  return v;
};

int handlePhysics(DynamicObject *dynamic_object, Tile *currentTile, float *dt, Game *game) {
  if (dynamic_object->dx || dynamic_object->dy) {
    dynamic_object->frictionalForceX = currentTile->cof * dynamic_object->normalForce;
    dynamic_object->frictionalForceY = currentTile->cof * dynamic_object->normalForce;
  }
  if (dynamic_object->moveLeft) {
    dynamic_object->directionX = -1;
    dynamic_object->thrustX = dynamic_object->isRunning ? dynamic_object->runThrust : dynamic_object->walkThrust;
    if (dynamic_object->moveRight) {
      dynamic_object->directionX = 0;
    }
  } else if (!dynamic_object->moveLeft) {
    dynamic_object->directionX = dynamic_object->dx < 0 ? 1 : 0;
    if (dynamic_object->moveRight) {
      dynamic_object->directionX = 1;
    } else {
      dynamic_object->thrustX = 0;
    }
  }
  if (dynamic_object->moveRight) {
    dynamic_object->directionX = 1;
    dynamic_object->thrustX = dynamic_object->isRunning ? dynamic_object->runThrust : dynamic_object->walkThrust;
    if (dynamic_object->moveLeft) {
      dynamic_object->directionX = 0;
    }
  } else if (!dynamic_object->moveRight) {
    dynamic_object->directionX = dynamic_object->dx > 0 ? -1 : 0;
    dynamic_object->directionX = dynamic_object->dx < 0 ? 1 : dynamic_object->directionX;
    if (dynamic_object->moveLeft) {
      dynamic_object->directionX = -1;
    } else {
      dynamic_object->thrustX = 0;
    }
  }
  if (dynamic_object->moveUp) {
    dynamic_object->directionY = -1;
    dynamic_object->thrustY = dynamic_object->isRunning ? dynamic_object->runThrust : dynamic_object->walkThrust;
    if (dynamic_object->moveDown) {
      dynamic_object->directionY = 0;
    }
  } else if (!dynamic_object->moveUp) {
    dynamic_object->directionY = dynamic_object->dy < 0 ? 1 : 0;
    if (dynamic_object->moveDown) {
      dynamic_object->directionY = 1;
    } else {
      dynamic_object->thrustY = 0;
    }
  }
  if (dynamic_object->moveDown) {
    dynamic_object->directionY = 1;
    dynamic_object->thrustY = dynamic_object->isRunning ? dynamic_object->runThrust : dynamic_object->walkThrust;
    if (dynamic_object->moveUp) {
      dynamic_object->directionY = 0;
    }
  } else if (!dynamic_object->moveDown) {
    dynamic_object->directionY = dynamic_object->dy > 0 ? -1 : 0;
    dynamic_object->directionY = dynamic_object->dy < 0 ? 1 : dynamic_object->directionY;
    if (dynamic_object->moveUp) {
      dynamic_object->directionY = -1;
    } else {
      dynamic_object->thrustY = 0;
    }
  }

  float prevDx = dynamic_object->dx;
  float prevDy = dynamic_object->dy;
  if (dynamic_object->moveLeft || dynamic_object->moveRight) {
    dynamic_object->ax = ((dynamic_object->directionX * dynamic_object->thrustX) - (dynamic_object->directionX * dynamic_object->frictionalForceX)) / dynamic_object->mass;
  } else {
    dynamic_object->ax = ((dynamic_object->directionX * dynamic_object->thrustX) + (dynamic_object->directionX * dynamic_object->frictionalForceX)) / dynamic_object->mass;
  }
  if (dynamic_object->moveUp || dynamic_object->moveDown) {
    dynamic_object->ay = ((dynamic_object->directionY * dynamic_object->thrustY) - (dynamic_object->directionY * dynamic_object->frictionalForceY)) / dynamic_object->mass;
  } else {
    dynamic_object->ay = ((dynamic_object->directionY * dynamic_object->thrustY) + (dynamic_object->directionY * dynamic_object->frictionalForceY)) / dynamic_object->mass;
  }
  dynamic_object->dx = accelerate(dynamic_object->dx, dynamic_object->ax, *dt);
  dynamic_object->dy = accelerate(dynamic_object->dy, dynamic_object->ay, *dt);

  if (dynamic_object->isMoving) {
    if (!dynamic_object->isRunning && dynamic_object->dx >= currentTile->maxSpeed) {
      dynamic_object->dx = currentTile->maxSpeed;
    }
    if (dynamic_object->isRunning && dynamic_object->dx >= currentTile->maxRunningSpeed) {
      dynamic_object->dx = currentTile->maxRunningSpeed;
    }
    if (!dynamic_object->isRunning && dynamic_object->dx <= -currentTile->maxSpeed) {
      dynamic_object->dx = -currentTile->maxSpeed;
    }
    if (dynamic_object->isRunning && dynamic_object->dx <= -currentTile->maxRunningSpeed) {
      dynamic_object->dx = -currentTile->maxRunningSpeed;
    }

    if (!dynamic_object->isRunning && dynamic_object->dy >= currentTile->maxSpeed) {
      dynamic_object->dy = currentTile->maxSpeed;
    }
    if (dynamic_object->isRunning && dynamic_object->dy >=currentTile->maxRunningSpeed) {
      dynamic_object->dy = currentTile->maxRunningSpeed;
    }
    if (!dynamic_object->isRunning && dynamic_object->dy <= -currentTile->maxSpeed) {
      dynamic_object->dy = -currentTile->maxSpeed;
    }
    if (dynamic_object->isRunning && dynamic_object->dy <= -currentTile->maxRunningSpeed) {
      dynamic_object->dy = -currentTile->maxRunningSpeed;
    }
  }

  if (!dynamic_object->isMoving) {
    if ((dynamic_object->dx < prevDx && prevDx > 0 && dynamic_object->dx < 0) || (dynamic_object->dx > prevDx && prevDx < 0 && dynamic_object->dx > 0)) {
      dynamic_object->dx = 0;
      dynamic_object->ax = 0;
      dynamic_object->directionX = 0;
      dynamic_object->thrustX = 0;
      dynamic_object->frictionalForceX = 0;
    }
    if ((dynamic_object->dy < prevDy && prevDy > 0 && dynamic_object->dy < 0) || (dynamic_object->dy > prevDy && prevDy < 0 && dynamic_object->dy > 0)) {
      dynamic_object->dy = 0;
      dynamic_object->ay = 0;
      dynamic_object->directionY = 0;
      dynamic_object->thrustY = 0;
      dynamic_object->frictionalForceY = 0;
    }
  }

  dynamic_object->x += dynamic_object->dx * *dt * PIXELS_PER_METER;
  dynamic_object->y += dynamic_object->dy * *dt * PIXELS_PER_METER;

  return 0;
}

