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
  game->man.x = WINDOW_WIDTH/2;
  game->man.y = WINDOW_HEIGHT/2;
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

void renderText(Game *game, char* text, SDL_Color color, int x, int y, int w, int h) { 
  SDL_Rect textRect = {x, y, w, h};
  SDL_Surface *surface = TTF_RenderText_Solid(game->text.font, text, color);
  game->text.texture = SDL_CreateTextureFromSurface(game->renderer, surface);
  SDL_RenderCopy(game->renderer, game->text.texture, NULL, &textRect);
  SDL_FreeSurface(surface);
}

void renderTile(Game *game, int x, int y, char tileId) {
  int tileRow;
  int tileColumn;
  if (tileId == '@') {
    tileRow = 4;
    tileColumn = 4;
  } else {
    tileRow = 12;
    tileColumn = 7;
  }
  SDL_Rect srcRect = {tileRow * 16, tileColumn * 16, 16, 16};
  SDL_Rect tileRect = {x + game->scrollX, y + game->scrollY, game->map.tileSize, game->map.tileSize};
  SDL_RenderCopy(game->renderer, game->terrainTexture, &srcRect, &tileRect);
};

void doRender(Game *game) {
  SDL_RenderClear(game->renderer);

  for (int y = -game->scrollY/game->map.tileSize; y < (-game->scrollY + WINDOW_HEIGHT)/ game->map.tileSize; y++)
    for (int x = -game->scrollX/game->map.tileSize; x < (-game->scrollX + WINDOW_WIDTH)/ game->map.tileSize; x++) {
      if (x >= 0 && x < game->map.width && y>= 0 && y < game->map.height) {
        renderTile(game, x * game->map.tileSize, y * game->map.tileSize, game->map.tiles[x + y * game->map.width].tileId);
      }
    }

  renderMan(game, game->scrollX+game->man.x, game->scrollY+game->man.y);

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
  game->map = initializeMap("map.lvl", 32);
  game->status = IS_ACTIVE;
};

// Detect if two objects in space have a collision
int hasCollision(float x1, float y1, float x2, float y2, float w1, float h1, float w2, float h2)
{
  return (!((x1 > (x2+w2)) || (x2 > (x1+w1)) || (y1 > (y2+h2)) || (y2 > (y1+h1))));
}

void detectCollisions(Game *game) {
  for (int y = -game->scrollY/game->map.tileSize; y < (-game->scrollY + WINDOW_HEIGHT)/ game->map.tileSize; y++)
    for (int x = -game->scrollX/game->map.tileSize; x < (-game->scrollX + WINDOW_WIDTH)/ game->map.tileSize; x++) {
    float manX = game->man.x;
    float manY = game->man.y;
    float manW = game->man.w;
    float manH = game->man.h;
    
    float floorX = game->map.tiles[x + y * game->map.width].x;
    float floorY = game->map.tiles[x + y * game->map.width].y;
    int floorW = game->map.tiles[x + y * game->map.width].w;
    int floorH = game->map.tiles[x + y * game->map.width].h; 

    if (x >= 0 && x < game->map.width && y>= 0 && y < game->map.height) {
      if (game->map.tiles[x + y * game->map.width].tileState == IS_SOLID) {

        if (manX+manW/2 > floorX && manX+manW/2<floorX+floorW) {
          if (manY < floorH+floorY && manY > floorY && game->man.dy < 0) {
            game->man.y = floorY+floorH;
            manY = floorY+floorH;
            game->man.dy = 0;
          } 
        }
        
        if (manX+manW > floorX && manX<floorX+floorW) {
          if (manY+manH > floorY && manY < floorY && game->man.dy > 0) {
            game->man.y = floorY-manH;
            manY = floorY-manH;
            game->man.dy = 0;
          }
        }

        if (manY+manH/2 > floorY && manY<floorY+floorH) {
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


  game->scrollX = -game->man.x+WINDOW_WIDTH/2;
  game->scrollY = -game->man.y+WINDOW_HEIGHT/2;

  if (game->man.x < 0) {
    game->man.x = 0;
  }

  if (game->man.x > game->map.width * game->map.tileSize - game->man.w) {
    game->man.x = game->map.width * game->map.tileSize - game->man.w;
  }

  if (game->man.y < 0) {
    game->man.y = 0;
  }

  if (game->man.y > game->map.height * game->map.tileSize - game->man.h) {
    game->man.y = game->map.height * game->map.tileSize - game->man.h;
  }

  if(game->scrollX > 0) {
    game->scrollX = 0;
  }
  if(game->scrollX < -game->map.width * game->map.tileSize+WINDOW_WIDTH) {
    game->scrollX = -game->map.width * game->map.tileSize+WINDOW_WIDTH;
  }

  if(game->scrollY > 0) {
    game->scrollY = 0;
  }

  if(game->scrollY < -game->map.height * game->map.tileSize+WINDOW_HEIGHT) {
    game->scrollY = -game->map.height * game->map.tileSize+WINDOW_HEIGHT;
  }

  // handle animation
  game->man.angle = getAngle(game);
  game->man.direction = getDirection(game);
  if (game->man.dx != 0 || game->man.dy != 0) {
    game->man.status = IS_RUNNING;
  } else {
    game->man.status = IS_IDLE;
  }

  if (game->time * 10 % 75 == 0) {
    game->man.sprite = (game->man.sprite + 1) % 8;
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
      WINDOW_WIDTH,                               
      WINDOW_HEIGHT,                               
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
