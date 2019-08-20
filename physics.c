#include "physics.h"
#include "rpg.h"

float accelerate(float v, float a, float dt) { 
  v += a * dt;
  printf("velo %f\n", v);
  printf("accel %f\n", a);
  fflush(stdout);
  return v;
};
