#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include "rpg.h"
#include "physics.h"

int handleEvents(Game *game) {
  SDL_Event event;
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:
        return 1;
        break;
      case SDL_KEYUP:
        switch (event.key.keysym.sym) {
          case SDLK_LEFT:
          case SDLK_RIGHT:
          case SDLK_UP:
          case SDLK_DOWN: 
            break;
        }
      case SDL_KEYDOWN:
        switch (event.key.keysym.sym) {
          case SDLK_LEFT:
          case SDLK_RIGHT:
          case SDLK_UP:
          case SDLK_DOWN: 
            if (!game->startTime) {
              game->startTime = SDL_GetTicks() * .001;
            }
            break;
        }
        switch (event.key.keysym.sym) {
          case SDLK_ESCAPE:
            return 1;
            break;
          case SDLK_LEFT:
            game->cat.direction = LEFT;
            game->row = 3;
            game->cat.sprite = 1;
            break; 
          case SDLK_RIGHT:
            game->cat.direction = RIGHT;
            game->row = 0;
            game->cat.sprite = 1;
            break;
          case SDLK_UP:
            game->cat.direction = UP;
            game->row = 1;
            game->cat.sprite = 1;
            break;
          case SDLK_DOWN:
            game->cat.direction = DOWN;
            game->row = 2;
            game->cat.sprite = 1;
            break;
          // TODO: Can we jump in 2d without isomorphic view?
          default:
            break;
        }
        break;
      default:
        break;
    }
  }
 

  if (game->status == IS_ACTIVE) {
    SDL_PumpEvents();
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    time_t timer;
    if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_DOWN]) {
      timer = SDL_GetTicks() * .001;
      game->dt =+ timer - game->startTime + 1;
    } else {
      game->startTime = SDL_GetTicks() * .001;
      game->dt = (SDL_GetTicks() * .001) - game->startTime * .001 + 1;
    }
    if (state[SDL_SCANCODE_LEFT]) {
      if (game->cat.thrustX < game->cat.maxThrust) {
        game->cat.thrustX += game->cat.maxThrust / 4.0f;
      }
      game->cat.ax = (game->cat.thrustX - (1 * game->cat.force)) / game->cat.mass;
      game->cat.dx = accelerate(game->cat.dx, -game->cat.ax, game->dt); 
    } else if (!state[SDL_SCANCODE_LEFT]) {
      if (game->cat.dx < 0) {
        printf("hello timer %ld\n", game->dt);
        printf("hello ax %f\n", game->cat.ax);
        printf("hello dx %f\n", game->cat.dx);
        game->cat.ax = (game->cat.thrustX + (1 * game->cat.force)) / game->cat.mass;
        game->cat.dx = accelerate(game->cat.dx, game->cat.ax, game->dt); 
        printf("hello 2 ax %f\n", game->cat.ax);
        printf("hello 2 dx %f\n", game->cat.dx);
      }
      if (game->cat.thrustX > 0 && !state[SDL_SCANCODE_RIGHT] && game->cat.dx >= 0) {
        game->cat.thrustX = 0;
      }
    }
    if (state[SDL_SCANCODE_RIGHT]) {
      if (game->cat.thrustX < game->cat.maxThrust) {
        game->cat.thrustX += game->cat.maxThrust / 4.0f;
      }
      game->cat.ax = (game->cat.thrustX - (1 * game->cat.force)) / game->cat.mass;
      game->cat.dx = accelerate(game->cat.dx, game->cat.ax, game->dt); 
    } else if (!state[SDL_SCANCODE_RIGHT]) {
      if (game->cat.dx > 0) {
        game->cat.ax = (game->cat.thrustX + (1 * game->cat.force)) / game->cat.mass;
        game->cat.dx = accelerate(game->cat.dx, -game->cat.ax, game->dt); 
      }
      if (game->cat.thrustX > 0 && !state[SDL_SCANCODE_LEFT] && game->cat.dx <= 0) {
        game->cat.thrustX = 0;
      }
    }
    if (state[SDL_SCANCODE_UP]) {
      if (game->cat.thrustY < game->cat.maxThrust) {
        game->cat.thrustY += game->cat.maxThrust / 4.0f;
      }
      game->cat.ay = (game->cat.thrustY - (1 * game->cat.force)) / game->cat.mass;
      game->cat.dy = accelerate(game->cat.dy, -game->cat.ay, game->dt);
    } else if (!state[SDL_SCANCODE_UP]) {
      if (game->cat.dy < 0) {
        game->cat.ay = (game->cat.thrustY + (1 * game->cat.force)) / game->cat.mass;
        game->cat.dy = accelerate(game->cat.dy, game->cat.ay, game->dt); 
      }
      if (game->cat.thrustY > 0 && !state[SDL_SCANCODE_DOWN] && game->cat.dy >= 0) {
        game->cat.thrustY = 0;
      }
    }
    if (state[SDL_SCANCODE_DOWN]) {
      if (game->cat.thrustY < game->cat.maxThrust) {
        game->cat.thrustY += game->cat.maxThrust / 4.0f;
      }
      game->cat.ay = (game->cat.thrustY - (1 * game->cat.force)) / game->cat.mass;
      game->cat.dy = accelerate(game->cat.dy, game->cat.ay, game->dt);
    } else if (!state[SDL_SCANCODE_DOWN]) {
      if (game->cat.dy > 0) {
        game->cat.ay = (game->cat.thrustY + (1 * game->cat.force)) / game->cat.mass;
        game->cat.dy = accelerate(game->cat.dy, -game->cat.ay, game->dt); 
      }
      if (game->cat.thrustY > 0 && !state[SDL_SCANCODE_UP] && game->cat.dy <= 0) {
        game->cat.thrustY = 0;
      }
    }

    if (!state[SDL_SCANCODE_UP] && !state[SDL_SCANCODE_DOWN]) {
      if (fabs(game->cat.dy) < 0.5f) {
        game->cat.dy = 0;
      }
    }

    if (!state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT]) {
      if (fabs(game->cat.dx) < 0.5f) {
        game->cat.dx = 0;
      }
    }

    if (!state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT] && !state[SDL_SCANCODE_UP] && !state[SDL_SCANCODE_DOWN]) {
      if (fabs(game->cat.dx) < 0.5f) {
        game->cat.dx = 0;
      }
      if (fabs(game->cat.dy) < 0.5f) {
        game->cat.dy = 0;
      }
    }

    if (game->cat.dx > 3.0f) {
      game->cat.dx = 3.0f;
    }
    if (game->cat.dx < -3.0f) {
      game->cat.dx = -3.0f;
    }
    if (game->cat.dy > 3.0f) {
      game->cat.dy = 3.0f;
    }
    if (game->cat.dy < -3.0f) {
      game->cat.dy = -3.0f;
    }
  }
 

  return 0;
}


SDL_Surface* createSurface(char* filename) {
  SDL_Surface *surface;

  surface = IMG_Load(filename);

  if (surface == NULL) {
    printf("Could not find %s\n\n", filename);
    SDL_Quit();
    exit(1);
  }

  return surface;
}

void initializeCat(Game *game, int spriteValue) {
  SDL_Surface *catSurface = createSurface("images/cat.png");
  game->cat.texture= SDL_CreateTextureFromSurface(game->renderer, catSurface);
  game->cat.x = 0;
  game->cat.y = 0;
  game->cat.w = catSurface->w / 16;
  game->cat.h = catSurface->h / 5;
  game->cat.mass = 30;
  game->cat.force = game->cat.mass * game->gravity * cos(90*M_PI);
  game->cat.maxThrust = 500.0f / 1000.0f / 1000.0f * PIXELS_PER_METER;
  game->cat.thrustX = 0;
  game->cat.thrustY = 0;
  game->cat.ax= 0;
  game->cat.ay= 0;
  game->cat.dx = 0;
  game->cat.dy = 0;
  game->cat.health= 200;
  game->cat.isOnFloor = 0;
  game->cat.sprite = spriteValue;
  game->cat.direction = RIGHT;
  game->scrollX = 0;
  game->scrollY = 0;
  SDL_FreeSurface(catSurface);
}

void renderCat(Game * game, int x, int y) {
  SDL_Rect catSrcRect = { game->cat.sprite * 32, 32 * game->row, 32, 27 };
  SDL_Rect catRect = {x, y, game->cat.w, game->cat.h};
  SDL_RenderCopy(game->renderer, game->cat.texture, &catSrcRect, &catRect);
}

void renderDog(Game *game, int x, int y, int w, int h, int sprite, SDL_Texture *texture) {
  SDL_Rect dogSrcRect = { sprite * 32, 0, 32, 27 };
  SDL_Rect dogRect = { x, y, w, h };
  SDL_RenderCopy(game->renderer, texture, &dogSrcRect, &dogRect);
}

void renderFloor(Game *game, int x, int y, int w, int h) { 
  SDL_Rect floorRect = {x, y, w, h};
  SDL_RenderFillRect(game->renderer, &floorRect);
}

void renderHealthBar(Game *game) {
  SDL_Rect healthBoundaryRect = { 200, 30, 200, 20 };
  SDL_Rect healthRect = { 200, 30, game->cat.health, 20 };
  SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 255);
  SDL_RenderFillRect(game->renderer, &healthBoundaryRect);
  SDL_SetRenderDrawColor(game->renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(game->renderer, &healthRect);
}

void renderText(Game *game, char* text, SDL_Color color, int x, int y, int w, int h) { 
  SDL_Rect textRect = {x, y, w, h};
  SDL_Surface *surface = TTF_RenderText_Solid(game->text.font, text, color);
  game->text.texture = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_RenderCopy(game->renderer, game->text.texture, NULL, &textRect);
  SDL_FreeSurface(surface);
}

void fadeOut() {

}

void doRender(Game *game) {
  SDL_RenderClear(game->renderer);

  if (game->status == IS_RESTING && game->cat.sprite == CAT_RESTING_RIGHT) {
    // Game Over Screen
    renderCat(game, 640/2-game->cat.w, 480/2-game->cat.h/2);
    SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 0);
    SDL_Color color = {255,255,255};
    renderText(game, "Game Over", color, 640/2-game->cat.w-150, 480/2-game->cat.h-100, 300, 48);
  } else {
    renderCat(game, game->scrollX+game->cat.x, game->scrollY+game->cat.y);

    // Floor color
    SDL_SetRenderDrawColor(game->renderer, 139, 69, 19, 255);

    for (int i=0;i<100;i++) {
      renderFloor(game, game->scrollX+game->floors[i].x, game->scrollY+game->floors[i].y, game->floors[i].w, game->floors[i].h);
    }

    renderHealthBar(game);

    SDL_Color color = {0,0,0};
    renderText(game, "Health", color, 10, 20, 150, 48);
    /* char bufferX[sizeof(game->cat.x) * 4 + 1]; */
    /* char bufferY[sizeof(game->cat.y) * 4 + 1]; */
    char bufferDX[sizeof(game->cat.dx) * 4 + 1];
    char bufferDY[sizeof(game->cat.dy) * 4 + 1];
    char bufferAX[sizeof(game->cat.ax) * 4 + 1];
    char bufferAY[sizeof(game->cat.ay) * 4 + 1];
    char bufferTX[sizeof(game->cat.thrustX) * 4 + 1];
    char bufferTY[sizeof(game->cat.thrustY) * 4 + 1];
    char bufferT[sizeof(game->dt) * 4 + 1];
    char bufferF[sizeof(game->cat.force) * 4 + 1];
    /* char bufferG[sizeof(game->gravity) * 4 + 1]; */
    /* sprintf(bufferX, "%f", game->cat.x); */
    /* sprintf(bufferY, "Y Position %f", game->cat.y); */
    sprintf(bufferDX, "%f", game->cat.dx);
    sprintf(bufferDY, "%f", game->cat.dy);
    sprintf(bufferAX, "%f", game->cat.ax);
    sprintf(bufferAY, "%f", game->cat.ay);
    sprintf(bufferTX, "%f", game->cat.thrustX);
    sprintf(bufferTY, "%f", game->cat.thrustY);
    sprintf(bufferT, "%ld", game->dt);
    sprintf(bufferF, "%f", game->cat.force);
    /* sprintf(bufferG, "Gravity %f", game->gravity); */
    /* renderText(game, bufferX, color, 400, 20, 150, 20); */
    /* renderText(game, bufferY, color, 400, 40, 150, 20); */
    renderText(game, bufferDX, color, 400, 60, 150, 20);
    renderText(game, bufferDY, color, 400, 80, 150, 20);
    renderText(game, bufferAX, color, 400, 100, 150, 20);
    renderText(game, bufferAY, color, 400, 120, 150, 20);
    renderText(game, bufferTX, color, 400, 140, 150, 20);
    renderText(game, bufferTY, color, 400, 160, 150, 20);
    renderText(game, bufferT, color, 400, 180, 150, 20);
    renderText(game, bufferF, color, 400, 200, 150, 20);
    /* renderText(game, bufferG, color, 400, 220, 150, 20); */

    SDL_SetRenderDrawColor(game->renderer, 25, 100, 155, 255);
  }

  SDL_RenderPresent(game->renderer);
};

void loadGame(Game *game) {
  game->text.font = TTF_OpenFont("fonts/slkscr.ttf", 48);
  if (game->text.font == NULL) {
    printf("Could not find font");
    SDL_Quit();
    exit(1);
  }
  
  initializeCat(game, CAT_STANDING_RIGHT);

  for(int i=0; i < 100; i++) {
    game->floors[i].w = 256;
    game->floors[i].h = 50;
    game->floors[i].x = rand() % 6400;
    game->floors[i].y = rand() % 640;
  }

  game->floors[0].y = 400;

  game->floors[99].x = 350;
  game->floors[99].y = 200;

  // @TODO move tilemap related stuff somewhere else
  game->row = 0;
  game->status = IS_ACTIVE;
};

void restartGame(Game *game) {
  game->status = IS_RESTING;
  initializeCat(game, CAT_RESTING_RIGHT);
  game->time = 0;
}

int hasCollision(float x1, float y1, float x2, float y2, float w1, float h1, float w2, float h2)
{
  return (!((x1 > (x2+w2)) || (x2 > (x1+w1)) || (y1 > (y2+h2)) || (y2 > (y1+h1))));
}

void detectCollisions(Game *game) {
  for (int i=0;i<100;i++) {
    float catX = game->cat.x;
    float catY = game->cat.y;
    float catW = game->cat.w;
    float catH = game->cat.h;
    
    float floorX = game->floors[i].x;
    float floorY = game->floors[i].y;
    float floorW = game->floors[i].w;
    float floorH = game->floors[i].h;

    if (catX+catW/2 > floorX && catX+catW/2<floorX+floorW) {
      if (catY < floorH+floorY && catY > floorY && game->cat.dy < 0) {
        game->cat.y = floorY+floorH;
        catY = floorY+floorH;
        game->cat.dy = 0;
        game->cat.isOnFloor = 1;
      } 
    }
      
    if (catX+catW > floorX && catX<floorX+floorW) {
      if (catY+catH > floorY && catY < floorY && game->cat.dy > 0) {
        game->cat.y = floorY-catH;
        catY = floorY-catH;
        game->cat.dy = 0;
        game->cat.isOnFloor = 1;
      }
    }

    if (catY+catH > floorY && catY<floorY+floorH) {
      if (catX < floorX+floorW && catX+catW > floorX+floorW && game->cat.dx < 0) {
        game->cat.x = floorX+floorW;
        catX = floorX + floorW;
        game->cat.dx = 0;
      } else if (catX+catW > floorX && catX < floorX && game->cat.dx > 0) {
        game->cat.x = floorX-catW;
        catX = floorX - catW;
        game->cat.dx = 0;
      }
    }
  }
};

void process(Game *game) {
  game->time++;

  if (game->status == IS_RESTING) {
    if (game->time > 120) {
      game->status = IS_ACTIVE;
      game->cat.sprite = CAT_STANDING_RIGHT;
    }
  }

  game->cat.x += game->cat.dx; 
  game->cat.y += game->cat.dy;

  game->scrollX = -game->cat.x+320;
  game->scrollY = -game->cat.y+240;

  // @TODO Make sure character can't go beyond width or height of tilemap

  if (game->cat.health< 0) {
    game->cat.health= 0;
  }

  if (game->cat.health== 0) {
    restartGame(game);
  }

  if (game->cat.dx != 0) {
    if (game->time % 8 == 0) {
      if (game->cat.direction == RIGHT) {
        game->row = 0;
        if (game->cat.sprite == CAT_WALKING_RIGHT_LEFT_FOOT) {
          game->cat.sprite = CAT_WALKING_RIGHT_RIGHT_FOOT;
        } else {
          game->cat.sprite = CAT_WALKING_RIGHT_LEFT_FOOT;
        }
      } else if (game->cat.direction == LEFT) {
        game->row = 3;
        if (game->cat.sprite == CAT_WALKING_LEFT_RIGHT_FOOT) {
          game->cat.sprite = CAT_WALKING_LEFT_LEFT_FOOT;
        } else {
          game->cat.sprite = CAT_WALKING_LEFT_RIGHT_FOOT;
        }
      }
    }
  } else if (game->cat.dy != 0) {
    if (game->time % 8 == 0) {
      if (game->cat.direction == UP) {
        game->row = 1;
        if (game->cat.sprite == CAT_WALKING_UP_LEFT_FOOT) {
          game->cat.sprite = CAT_WALKING_UP_RIGHT_FOOT;
        } else {
          game->cat.sprite = CAT_WALKING_UP_LEFT_FOOT;
        }
      } else {
        game->row = 2;
        if (game->cat.sprite == CAT_WALKING_DOWN_RIGHT_FOOT) {
          game->cat.sprite = CAT_WALKING_DOWN_LEFT_FOOT;
        } else {
          game->cat.sprite = CAT_WALKING_DOWN_RIGHT_FOOT;
        }
      }
    }
  } else if (game->cat.dy == 0 && game->cat.dx == 0) {
    game->row = 0;
    game->cat.sprite = 5;
  } else if (game->cat.dy != 0 && game->cat.dx != 0) {
    game->row = 1;
    game->cat.sprite = 3;
  }
   
  detectCollisions(game);
};

void shutdownGame(Game *game) {
  SDL_DestroyTexture(game->cat.texture);
  TTF_CloseFont(game->text.font);
  SDL_DestroyWindow(game->window); 

  TTF_Quit();
  SDL_Quit(); 
};

int main(int argc, char *argv[]) {
  srand(time(NULL));

  TTF_Init();

  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
    return 1;
  }

  Game game;
  // Create an application window with the following settings:
  game.window = SDL_CreateWindow( 
      "Cat Jump",
      SDL_WINDOWPOS_UNDEFINED,           
      SDL_WINDOWPOS_UNDEFINED,           
      640,                               
      480,                               
      SDL_WINDOW_SHOWN
      );

  game.gravity = 9.8 / 1000 / 1000 * PIXELS_PER_METER;

  if (game.window == NULL) {
    fprintf(stderr, "could not create window: %s\n", SDL_GetError());
    return 1;
  }

  game.renderer = SDL_CreateRenderer(game.window, -1, SDL_RENDERER_ACCELERATED);
  if (game.renderer == NULL) {
    fprintf(stderr, "could not create renderer: %s\n", SDL_GetError());
    return 1;
  }

  loadGame(&game);

  int done = 0;
  while (!done) {
    done = handleEvents(&game);
    process(&game);
    doRender(&game);

    SDL_Delay(10);
  }

  shutdownGame(&game);

  return 0;
}
