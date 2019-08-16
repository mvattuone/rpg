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
            break; 
          case SDLK_RIGHT:
            break;
          case SDLK_UP:
            break;
          case SDLK_DOWN:
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
      if (game->man.thrustX < game->man.maxThrust) {
        game->man.thrustX += game->man.maxThrust / 4.0f;
      }
      game->man.ax = (game->man.thrustX - (1 * game->man.force)) / game->man.mass;
      game->man.dx = accelerate(game->man.dx, -game->man.ax, game->dt); 
    } else if (!state[SDL_SCANCODE_LEFT]) {
      if (game->man.dx < 0) {
        game->man.ax = (game->man.thrustX + (1 * game->man.force)) / game->man.mass;
        game->man.dx = accelerate(game->man.dx, game->man.ax, game->dt); 
      }
      if (game->man.thrustX > 0 && !state[SDL_SCANCODE_RIGHT] && game->man.dx >= 0) {
        game->man.thrustX = 0;
      }
    }
    if (state[SDL_SCANCODE_RIGHT]) {
      if (game->man.thrustX < game->man.maxThrust) {
        game->man.thrustX += game->man.maxThrust / 4.0f;
      }
      game->man.ax = (game->man.thrustX - (1 * game->man.force)) / game->man.mass;
      game->man.dx = accelerate(game->man.dx, game->man.ax, game->dt); 
    } else if (!state[SDL_SCANCODE_RIGHT]) {
      if (game->man.dx > 0) {
        game->man.ax = (game->man.thrustX + (1 * game->man.force)) / game->man.mass;
        game->man.dx = accelerate(game->man.dx, -game->man.ax, game->dt); 
      }
      if (game->man.thrustX > 0 && !state[SDL_SCANCODE_LEFT] && game->man.dx <= 0) {
        game->man.thrustX = 0;
      }
    }
    if (state[SDL_SCANCODE_UP]) {
      if (game->man.thrustY < game->man.maxThrust) {
        game->man.thrustY += game->man.maxThrust / 4.0f;
      }
      game->man.ay = (game->man.thrustY - (1 * game->man.force)) / game->man.mass;
      game->man.dy = accelerate(game->man.dy, -game->man.ay, game->dt);
    } else if (!state[SDL_SCANCODE_UP]) {
      if (game->man.dy < 0) {
        game->man.ay = (game->man.thrustY + (1 * game->man.force)) / game->man.mass;
        game->man.dy = accelerate(game->man.dy, game->man.ay, game->dt); 
      }
      if (game->man.thrustY > 0 && !state[SDL_SCANCODE_DOWN] && game->man.dy >= 0) {
        game->man.thrustY = 0;
      }
    }
    if (state[SDL_SCANCODE_DOWN]) {
      if (game->man.thrustY < game->man.maxThrust) {
        game->man.thrustY += game->man.maxThrust / 4.0f;
      }
      game->man.ay = (game->man.thrustY - (1 * game->man.force)) / game->man.mass;
      game->man.dy = accelerate(game->man.dy, game->man.ay, game->dt);
    } else if (!state[SDL_SCANCODE_DOWN]) {
      if (game->man.dy > 0) {
        game->man.ay = (game->man.thrustY + (1 * game->man.force)) / game->man.mass;
        game->man.dy = accelerate(game->man.dy, -game->man.ay, game->dt); 
      }
      if (game->man.thrustY > 0 && !state[SDL_SCANCODE_UP] && game->man.dy <= 0) {
        game->man.thrustY = 0;
      }
    }

    if (!state[SDL_SCANCODE_UP] && !state[SDL_SCANCODE_DOWN]) {
      if (fabs(game->man.dy) < 0.5f) {
        game->man.dy = 0;
      }
    }

    if (!state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT]) {
      if (fabs(game->man.dx) < 0.5f) {
        game->man.dx = 0;
      }
    }

    if (!state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT] && !state[SDL_SCANCODE_UP] && !state[SDL_SCANCODE_DOWN]) {
      if (fabs(game->man.dx) < 0.5f) {
        game->man.dx = 0;
      }
      if (fabs(game->man.dy) < 0.5f) {
        game->man.dy = 0;
      }
    }

    if (game->man.dx > 3.0f) {
      game->man.dx = 3.0f;
    }
    if (game->man.dx < -3.0f) {
      game->man.dx = -3.0f;
    }
    if (game->man.dy > 3.0f) {
      game->man.dy = 3.0f;
    }
    if (game->man.dy < -3.0f) {
      game->man.dy = -3.0f;
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

void initializeMan(Game *game, int spriteValue) {
  SDL_Surface *manIdleSurface = createSurface("images/man-idle.png");
  SDL_Surface *manRunningSurface = createSurface("images/man-running.png");
  game->man.idleTexture= SDL_CreateTextureFromSurface(game->renderer, manIdleSurface);
  game->man.runningTexture= SDL_CreateTextureFromSurface(game->renderer, manRunningSurface);
  game->man.x = 0;
  game->man.y = 0;
  game->man.angle = 0;
  game->man.w = manIdleSurface->w / 8;
  game->man.h = manIdleSurface->h / 8;
  game->man.mass = 45;
  game->man.force = game->man.mass * game->gravity * cos(90*M_PI);
  game->man.maxThrust = 1000.0f / 1000.0f / 1000.0f * PIXELS_PER_METER;
  game->man.thrustX = 0;
  game->man.thrustY = 0;
  game->man.ax= 0;
  game->man.ay= 0;
  game->man.dx = 0;
  game->man.dy = 0;
  game->man.status = IS_IDLE;
  game->man.health= 200;
  game->man.isOnFloor = 0;
  game->man.sprite = spriteValue;
  game->man.direction = RIGHT;
  game->scrollX = 0;
  game->scrollY = 0;
  SDL_FreeSurface(manRunningSurface);
  SDL_FreeSurface(manIdleSurface);
}

void renderMan(Game * game, int x, int y) {
  SDL_Rect manSrcRect = { game->man.sprite * game->man.w, game->man.h * game->man.direction, game->man.w, game->man.h};
  SDL_Rect manRect = {x, y, game->man.w, game->man.h};
  if (game->man.status == IS_RUNNING) {
    SDL_RenderCopy(game->renderer, game->man.runningTexture, &manSrcRect, &manRect);
  } else {
    SDL_RenderCopy(game->renderer, game->man.idleTexture, &manSrcRect, &manRect);
  }
}

void renderFloor(Game *game, int x, int y, int w, int h) { 
  SDL_Rect floorRect = {x, y, w, h};
  SDL_RenderFillRect(game->renderer, &floorRect);
}

void renderHealthBar(Game *game) {
  SDL_Rect healthBoundaryRect = { 200, 30, 200, 20 };
  SDL_Rect healthRect = { 200, 30, game->man.health, 20 };
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

void renderTile(Game *game, int x, int y, char* tileId) {
  int tileRow;
  int tileColumn;
  if (!strcmp(tileId,"%")) {
    tileRow = 1;
    tileColumn = 1;
  } else if (!strcmp(tileId, "@")) {
    tileRow = 4;
    tileColumn = 4;
  } else if (!strcmp(tileId, "#")) {
    tileRow = 12;
    tileColumn = 12;
  } else if (!strcmp(tileId, "&")) {
    tileRow = 2;
    tileColumn = 3;
  } else {
    tileRow = 12;
    tileColumn = 7;
  }
  SDL_Rect srcRect = {tileRow * 16, tileColumn * 16, 16, 16};
  SDL_Rect tileRect = {x + game->scrollX, y + game->scrollY, 80, 80};
  SDL_RenderCopy(game->renderer, game->terrainTexture, &srcRect, &tileRect);
};

void doRender(Game *game) {
  SDL_RenderClear(game->renderer);
  char* tiles[96] = {
    "@", "@", "@", "@", "@", "@", "@", "@",
    "@", "#", "#", "#", "#", "#", "#", "@",
    "@", "#", "#", "#", "#", "#", "#", "@",
    "@", "#", "#", "#", "#", "#", "#", "@",
    "@", "#", "#", "#", "#", "#", "#", "@",
    "@", "&", "&", "&", "&", "&", "&", "@",
    "@", "&", "&", "&", "&", "&", "&", "@",
    "@", "#", "#", "#", "#", "#", "#", "@",
    "@", "#", "#", "#", "#", "#", "#", "@",
    "@", "#", "#", "#", "#", "#", "#", "@",
    "@", "#", "#", "#", "#", "#", "#", "@",
    "@", "@", "@", "@", "@", "@", "@", "@"
  };

  for (int y = -game->scrollY/80; y < (-game->scrollY + 480)/ 80; y++)
    for (int x = -game->scrollX/80; x < (-game->scrollX + 640)/ 80; x++) {
      if (x >= 0 && x < 8 && y>= 0 && y < 12) {
        char* tileId = tiles[(x) + y*8];
        renderTile(game, x * 80, y * 80, tileId);
      }
    }

  renderMan(game, game->scrollX+game->man.x, game->scrollY+game->man.y);

  // Floor color
  SDL_SetRenderDrawColor(game->renderer, 139, 69, 19, 255);

  for (int i=0;i<100;i++) {
    renderFloor(game, game->scrollX+game->floors[i].x, game->scrollY+game->floors[i].y, game->floors[i].w, game->floors[i].h);
  }

  renderHealthBar(game);

  SDL_Color color = {0,0,0};
  renderText(game, "Health", color, 10, 20, 150, 48);

  if (0) {
    /* char bufferX[sizeof(game->man.x) * 4 + 1]; */
    /* char bufferY[sizeof(game->man.y) * 4 + 1]; */
    char bufferDX[sizeof(game->man.dx) * 4 + 1];
    char bufferDY[sizeof(game->man.dy) * 4 + 1];
    char bufferAX[sizeof(game->man.ax) * 4 + 1];
    char bufferAY[sizeof(game->man.ay) * 4 + 1];
    char bufferTX[sizeof(game->man.thrustX) * 4 + 1];
    char bufferTY[sizeof(game->man.thrustY) * 4 + 1];
    char bufferT[sizeof(game->dt) * 4 + 1];
    char bufferF[sizeof(game->man.angle) * 4 + 1];
    char bufferD[sizeof(game->man.direction) * 4 + 1];
    /* sprintf(bufferX, "%f", game->man.x); */
    /* sprintf(bufferY, "Y Position %f", game->man.y); */
    sprintf(bufferDX, "%f", game->man.dx);
    sprintf(bufferDY, "%f", game->man.dy);
    sprintf(bufferAX, "%f", game->man.ax);
    sprintf(bufferAY, "%f", game->man.ay);
    sprintf(bufferTX, "%f", game->man.thrustX);
    sprintf(bufferTY, "%f", game->man.thrustY);
    sprintf(bufferT, "%ld", game->dt);
    sprintf(bufferF, "%f", game->man.angle);
    sprintf(bufferD, "%i", game->man.direction);
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
    renderText(game, bufferD, color, 400, 220, 150, 20);
  }

  SDL_SetRenderDrawColor(game->renderer, 25, 100, 155, 255);

  SDL_RenderPresent(game->renderer);
};

void initializeTerrain(Game *game) {
  SDL_Surface *surface = createSurface("images/terrain.png");
  game->terrainTexture = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_FreeSurface(surface);
}

void loadGame(Game *game) {
  game->text.font = TTF_OpenFont("fonts/slkscr.ttf", 48);
  if (game->text.font == NULL) {
    printf("Could not find font");
    SDL_Quit();
    exit(1);
  }
  
  initializeTerrain(game);
  initializeMan(game, MAN_UP);

  for(int i=0; i < 100; i++) {
    game->floors[i].w = 256;
    game->floors[i].h = 50;
    game->floors[i].x = rand() % 6400;
    game->floors[i].y = rand() % 640;
  }

  game->floors[0].y = 400;

  game->floors[99].x = 350;
  game->floors[99].y = 200;

  game->status = IS_ACTIVE;
};

int hasCollision(float x1, float y1, float x2, float y2, float w1, float h1, float w2, float h2)
{
  return (!((x1 > (x2+w2)) || (x2 > (x1+w1)) || (y1 > (y2+h2)) || (y2 > (y1+h1))));
}

void detectCollisions(Game *game) {
  for (int i=0;i<100;i++) {
    float manX = game->man.x;
    float manY = game->man.y;
    float manW = game->man.w;
    float manH = game->man.h;
    
    float floorX = game->floors[i].x;
    float floorY = game->floors[i].y;
    float floorW = game->floors[i].w;
    float floorH = game->floors[i].h;

    if (manX+manW/2 > floorX && manX+manW/2<floorX+floorW) {
      if (manY < floorH+floorY && manY > floorY && game->man.dy < 0) {
        game->man.y = floorY+floorH;
        manY = floorY+floorH;
        game->man.dy = 0;
        game->man.isOnFloor = 1;
      } 
    }
      
    if (manX+manW > floorX && manX<floorX+floorW) {
      if (manY+manH > floorY && manY < floorY && game->man.dy > 0) {
        game->man.y = floorY-manH;
        manY = floorY-manH;
        game->man.dy = 0;
        game->man.isOnFloor = 1;
      }
    }

    if (manY+manH > floorY && manY<floorY+floorH) {
      if (manX < floorX+floorW && manX+manW > floorX+floorW && game->man.dx < 0) {
        game->man.x = floorX+floorW;
        manX = floorX + floorW;
        game->man.dx = 0;
      } else if (manX+manW > floorX && manX < floorX && game->man.dx > 0) {
        game->man.x = floorX-manW;
        manX = floorX - manW;
        game->man.dx = 0;
      }
    }
  }
};

float getAngle(Game *game) {
  float angle = atan2(game->man.dy, game->man.dx) * (180 / M_PI); 
  return angle > 0 ? angle : angle + 360;
};

int getDirection(Game *game) { 
  return (int)fabs(floor((game->man.angle < 270 ? game->man.angle + 90 : game->man.angle - 270)/45));
};

void process(Game *game) {
  game->time++;

  game->man.x += game->man.dx; 
  game->man.y += game->man.dy;


  game->scrollX = -game->man.x+320;
  game->scrollY = -game->man.y+240;

  if(game->scrollX > 0) {
    game->scrollX = 0;
  }
  if(game->scrollX < -38000+320) {
    game->scrollX = -38000+320;
  }
  if(game->scrollY > 0) {
    game->scrollY = 0;
  }
  if(game->scrollY < -38000+240) {
    game->scrollY = -38000+240;
  }

  // @TODO Make sure character can't go beyond width or height of tilemap

  if (game->man.health< 0) {
    game->man.health= 0;
  }

  if (game->man.health== 0) {
    // @TODO What happens
  }

  // handle animation
  game->man.angle = getAngle(game);
  game->man.direction = getDirection(game);
  if (game->man.dx != 0 || game->man.dy != 0) {
    game->man.status = IS_RUNNING;
    if (game->time * 10 % 75 == 0) {
      game->man.sprite = (game->man.sprite + 1) % 8;
    }
  } else {
    game->man.status = IS_IDLE;
  }
   
  detectCollisions(game);
};

void shutdownGame(Game *game) {
  SDL_DestroyTexture(game->terrainTexture);
  SDL_DestroyTexture(game->man.idleTexture);
  SDL_DestroyTexture(game->man.runningTexture);
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
  // Create an applimanion window with the following settings:
  game.window = SDL_CreateWindow( 
      "Man Jump",
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
