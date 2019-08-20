#include "physics.h"
#include "rpg.h"

float accelerate(float v, float a, float dt) { 
  v += a * dt;
  return v;
};
