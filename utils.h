#include <SDL2/SDL_image.h>
#include <time.h>

time_t toSeconds(time_t time);
SDL_Surface* createSurface(char* filename); 
int hasCollision(float x1, float y1, float x2, float y2, float w1, float h1, float w2, float h2);
