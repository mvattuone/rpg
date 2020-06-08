#include <SDL2/SDL_image.h>
#include <time.h>

#ifndef UTILS_H
#define UTILS_H

typedef struct {
  int capacity;
  int size; 
  int* items;
} DynamicArray;


time_t toSeconds(time_t time);
SDL_Surface* createSurface(char* filename); 
SDL_Texture* createTexture(SDL_Renderer *renderer, char* filename);
int hasCollision(float x1, float y1, float x2, float y2, float w1, float h1, float w2, float h2);
void swap_ints(int *array, int a, int b); 
float getAngleFromCoordinates(float dx, float dy);
int getDirectionFromAngle(float angle); 

#endif
