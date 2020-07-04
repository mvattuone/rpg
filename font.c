#include "font.h"

TTF_Font* initializeFont(char* filePath, int fontSize) {
  TTF_Font *font = TTF_OpenFont(filePath, fontSize);
  if (font == NULL) {
    printf("Could not find font");
    SDL_Quit();
    exit(1);
  }

  return font;
}
