#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "camera.h"
#include "rpg.h"
#include "renderers.h"

int getHatDirection(Direction direction) {
  switch (direction) {
    case UP:
      return 0;
      break;
    case LEFT:
      return 1;
      break;
    case DOWN:
      return 2;
      break;
    case RIGHT:
      return 3;
      break;
    default:
      break;
  }

  return -1;
};

void renderText(SDL_Renderer *renderer, TTF_Font *font, char* text, SDL_Color color, int x, int y, SDL_Surface *surface) {
  if (surface == NULL) {
    surface = TTF_RenderText_Blended_Wrapped(font, text, color, WINDOW_WIDTH - 50);
  }
  SDL_Rect textRect = {x, y, surface->w, surface->h};
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_RenderCopy(renderer, texture, NULL, &textRect);
  SDL_FreeSurface(surface);
  SDL_DestroyTexture(texture);
}

void renderCursor(SDL_Renderer *renderer, int x, int y, int w, int h) {
  SDL_Rect cursorRect = { x, y, w, h };
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &cursorRect); 
}

void renderDialogBox(SDL_Renderer *renderer, int i) {
  SDL_Rect dialogBoxBorderRect = { 0, WINDOW_HEIGHT - (220 * i), WINDOW_WIDTH, 220};
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderFillRect(renderer, &dialogBoxBorderRect); 
  SDL_Rect dialogBoxRect = { 20, WINDOW_HEIGHT - (210 * i), WINDOW_WIDTH - 40, 180};
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  SDL_RenderFillRect(renderer, &dialogBoxRect); 
}

void renderPauseState(SDL_Renderer *renderer, TTF_Font *font) {
  SDL_Rect pauseOverlayRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 50);
  SDL_RenderFillRect(renderer, &pauseOverlayRect); 
  SDL_Color color = {255, 255, 255};
  renderText(renderer, font, "Paused", color, WINDOW_WIDTH/2 - 40, WINDOW_HEIGHT/2, NULL);
}

void renderCrate(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect srcRect = { dynamic_object->sprite * dynamic_object->w, dynamic_object->h * dynamic_object->direction, dynamic_object->w, dynamic_object->h};
  SDL_Rect destRect = {x, y, dynamic_object->w, dynamic_object->h};
  SDL_RenderCopy(renderer, dynamic_object->crateTexture, &srcRect, &destRect);
}

void renderBed(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect srcRect = { dynamic_object->sprite * dynamic_object->w, dynamic_object->h * dynamic_object->direction, dynamic_object->w, dynamic_object->h};
  SDL_Rect destRect = {x, y, dynamic_object->w, dynamic_object->h};
  SDL_RenderCopy(renderer, dynamic_object->bedTexture, &srcRect, &destRect);
}

void renderDoor(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect srcRect = { dynamic_object->sprite * dynamic_object->w, dynamic_object->h * dynamic_object->direction, dynamic_object->w, dynamic_object->h};
  SDL_Rect destRect = {x, y, dynamic_object->w, dynamic_object->h};
  SDL_RenderCopy(renderer, dynamic_object->doorTexture, &srcRect, &destRect);
}

void renderJar(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect srcRect = { dynamic_object->sprite * dynamic_object->w, dynamic_object->h * dynamic_object->direction, dynamic_object->w, dynamic_object->h};
  SDL_Rect destRect = {x, y, dynamic_object->w, dynamic_object->h};
  SDL_RenderCopy(renderer, dynamic_object->jarTexture, &srcRect, &destRect);
}

void renderMan(DynamicObject *dynamic_object, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect srcRect = { dynamic_object->sprite * dynamic_object->w, dynamic_object->h * dynamic_object->direction, dynamic_object->w, dynamic_object->h};
  SDL_Rect destRect = {x, y, dynamic_object->w, dynamic_object->h};
  dynamic_object->status == IS_RUNNING
    ? SDL_RenderCopy(renderer, dynamic_object->runningTexture, &srcRect, &destRect)
    : SDL_RenderCopy(renderer, dynamic_object->idleTexture, &srcRect, &destRect);
  if (dynamic_object->hatTexture != NULL) {
    int hat_direction = getHatDirection(dynamic_object->direction);
    SDL_Rect srcRect = { hat_direction * 64, 0, 64, 64};
    SDL_Rect destRect = {x - 10, y - 10, dynamic_object->w * 2, dynamic_object->h * 2};
    SDL_RenderCopy(renderer, dynamic_object->hatTexture, &srcRect, &destRect);
  }
}

void renderMenu(Menu *inventory_menu, Item *items, DynamicArray inventory, TTF_Font *font, SDL_Renderer *renderer, int items_count) {
  SDL_Rect MenuRect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 50);
  SDL_RenderFillRect(renderer, &MenuRect); 
  SDL_Color text_color = { 255, 255, 255 }; 
  int item_position_index = 0;
  if (inventory.size <= 0) {
    renderText(renderer, font, "No items", text_color, 80, 40, NULL);
  } else {
    for (int i = 0; i < inventory.size; i++) {
      for (int j = 0; j < items_count; j++) {
        if (inventory.items[i] == items[j].id) {
          char *name = items[j].name;
          if (inventory_menu->state == ITEM_SELECTED && inventory_menu->selected_item_index == i) {
            SDL_Surface *surface = TTF_RenderText_Blended_Wrapped(font, name, text_color, WINDOW_WIDTH - 50);
            SDL_Rect SelectedItemRect = {80, (i + 1) * 40, surface->w, surface->h};
            SDL_SetRenderDrawColor(renderer, 90, 110, 140, 100);
            SDL_RenderFillRect(renderer, &SelectedItemRect); 
          }
          renderText(renderer, font, name, text_color, 80, (i + 1) * 40, NULL);
          renderCursor(renderer, 60, (inventory_menu->active_item_index + 1) * 40, 20, 20);
          if (inventory_menu->show_description && i == inventory_menu->active_item_index) {
            renderText(renderer, font, items[j].description, text_color, 80, WINDOW_HEIGHT - 100, NULL);
          }
          item_position_index++;
        }
      }
    }
  }
}

void renderTile(int x, int y, Camera camera, int tileSize, char tileId, SDL_Texture *texture, SDL_Renderer *renderer) {
 int tileRow = tileId % 16;
 int tileColumn = tileId / 16;
 SDL_Rect srcRect = {tileRow * 16, tileColumn * 16, 16, 16};
 SDL_Rect tileRect = {x + camera.base->x, y + camera.base->y, tileSize, tileSize};
 SDL_RenderCopy(renderer, texture, &srcRect, &tileRect);
};
