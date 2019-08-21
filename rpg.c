#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include "rpg.h"
#include "physics.h"
#define GRAVITY 9.8 * PIXELS_PER_METER

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
              game->startTime = toSeconds(SDL_GetTicks());
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
    float frictionalForceX = 0;
    float frictionalForceY = 0;
    float cof = 0.41; // This will be attached to the tile the player is on eventually
    int directionX = 0;
    int directionY = 0;
    if (state[SDL_SCANCODE_LEFT] || state[SDL_SCANCODE_RIGHT] || state[SDL_SCANCODE_UP] || state[SDL_SCANCODE_DOWN]) {
      frictionalForceX = cof * game->man.normalForce;
      frictionalForceY = cof * game->man.normalForce;
    } 
    if (state[SDL_SCANCODE_LEFT]) {
      directionX = -1;
      game->man.thrustX = state[SDL_SCANCODE_SPACE] ? game->man.runThrust : game->man.walkThrust;
      if (state[SDL_SCANCODE_RIGHT]) {
        directionX = 0;
      }
    } else if (!state[SDL_SCANCODE_LEFT]) {
      directionX = game->man.dx < 0 ? 1 : 0;
      if (state[SDL_SCANCODE_RIGHT]) {
        directionX = 1;
      } else {
        game->man.thrustX *= -1;
      }
    }
    if (state[SDL_SCANCODE_RIGHT]) {
      directionX = 1;
      game->man.thrustX = state[SDL_SCANCODE_SPACE] ? game->man.runThrust : game->man.walkThrust;
      if (state[SDL_SCANCODE_LEFT]) {
        directionX = 0;
      }
    } else if (!state[SDL_SCANCODE_RIGHT]) {
      directionX = game->man.dx > 0 ? -1 : 0;
      directionX = game->man.dx < 0 ? 1 : directionX;
      if (state[SDL_SCANCODE_LEFT]) {
        directionX = -1;
      } else {
        game->man.thrustX *= -1;
      }
    }
    if (state[SDL_SCANCODE_UP]) {
      directionY = -1;
      game->man.thrustY = state[SDL_SCANCODE_SPACE] ? game->man.runThrust : game->man.walkThrust; 
      if (state[SDL_SCANCODE_DOWN]) {
        directionY = 0;
      }
    } else if (!state[SDL_SCANCODE_UP]) {
      directionY = game->man.dy < 0 ? 1 : 0;
      if (state[SDL_SCANCODE_DOWN]) {
        directionY = 1;
      } else {
        game->man.thrustY *= -1;
      }
    }
    if (state[SDL_SCANCODE_DOWN]) {
      directionY = 1;
      game->man.thrustY = state[SDL_SCANCODE_SPACE] ? game->man.runThrust : game->man.walkThrust; 
      if (state[SDL_SCANCODE_UP]) {
        directionY = 0;
      }
    } else if (!state[SDL_SCANCODE_DOWN]) {
      directionY = game->man.dy > 0 ? -1 : 0;
      directionY = game->man.dy < 0 ? 1 : directionY;
      if (state[SDL_SCANCODE_UP]) {
        directionY = -1;
      } else {
        game->man.thrustY *= -1;
      }
    }

    if (frictionalForceX > game->man.thrustX) {
      frictionalForceX = game->man.thrustX;
    }

    if (frictionalForceY > game->man.thrustY) {
      frictionalForceY = game->man.thrustY;
    }

    game->man.ax = (((directionX * game->man.thrustX) - (directionX * frictionalForceX)) / game->man.mass);
    game->man.ay = (((directionY * game->man.thrustY) - (directionY * frictionalForceY)) / game->man.mass);
    game->man.dx = accelerate(game->man.dx, game->man.ax, game->dt); 
    game->man.dy = accelerate(game->man.dy, game->man.ay, game->dt); 

    if (!state[SDL_SCANCODE_UP] && !state[SDL_SCANCODE_DOWN] && !state[SDL_SCANCODE_LEFT] && !state[SDL_SCANCODE_RIGHT]) {
      if (fabs(game->man.dx) < 2) {
        game->man.dx = 0;
        game->man.ax = 0;
        game->man.thrustX = 0;
        directionX = 0;
      }
      if (fabs(game->man.dy) < 2) {
        game->man.dy = 0;
        game->man.ay = 0;
        game->man.thrustY = 0;
        directionY = 0;
      }
    }
  }
 
  return 0;
}

Man initializeMan(SDL_Renderer *renderer, int spriteValue, float x, float y, float angle, float mass, float walkThrust, float runThrust, Status status, Direction direction) {
  SDL_Surface *manIdleSurface = createSurface("images/man-idle.png");
  SDL_Surface *manRunningSurface = createSurface("images/man-running.png");
  Man man;
  man.idleTexture= SDL_CreateTextureFromSurface(renderer, manIdleSurface);
  man.runningTexture= SDL_CreateTextureFromSurface(renderer, manRunningSurface);
  man.x = x;
  man.y = y;
  man.angle = angle;
  man.w = manIdleSurface->w / 8;
  man.h = manIdleSurface->h / 8;
  man.mass = mass;
  man.normalForce = man.mass * GRAVITY * cos(90*M_PI); 
  man.walkThrust = walkThrust * PIXELS_PER_METER;
  man.runThrust = runThrust * PIXELS_PER_METER;
  man.thrustX = 0;
  man.thrustY = 0;
  man.ax= 0;
  man.ay= 0;
  man.dx = 0;
  man.dy = 0;
  man.status = status;
  man.sprite = spriteValue;
  man.direction = direction;
  SDL_FreeSurface(manRunningSurface);
  SDL_FreeSurface(manIdleSurface);

  return man;
}

void renderMan(Man *man, int x, int y, SDL_Renderer *renderer) {
  SDL_Rect manSrcRect = { man->sprite * man->w, man->h * man->direction, man->w, man->h};
  SDL_Rect manRect = {x, y, man->w, man->h};
  man->status == IS_RUNNING
    ? SDL_RenderCopy(renderer, man->runningTexture, &manSrcRect, &manRect)
    : SDL_RenderCopy(renderer, man->idleTexture, &manSrcRect, &manRect);
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

  renderMan(&game->man, game->man.x+game->scrollX, game->man.y+game->scrollY, game->renderer);
  renderMan(&game->man2, game->man2.x+game->scrollX, game->man2.y+game->scrollY, game->renderer);

  SDL_SetRenderDrawColor(game->renderer, 25, 100, 155, 255);

  SDL_RenderPresent(game->renderer);
};

SDL_Texture* initializeTerrain(SDL_Renderer *renderer) {
  SDL_Surface *surface = createSurface("images/terrain.png");
  SDL_Texture *terrainTexture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  return terrainTexture;
}

TTF_Font* initializeFont(char* fileName, int fontSize) {
  TTF_Font *font = TTF_OpenFont(fileName, fontSize);
  if (font == NULL) {
    printf("Could not find font");
    SDL_Quit();
    exit(1);
  }

  return font;
}

void loadGame(Game *game) {
  game->dt = 1.0f/60.0f;
  game->text.font = initializeFont("fonts/slkscr.ttf", 48);
  game->scrollX = 0;
  game->scrollY = 0;
  game->terrainTexture = initializeTerrain(game->renderer);
  game->man = initializeMan(game->renderer, MAN_UP, WINDOW_WIDTH/2, WINDOW_HEIGHT/2, 0, 80, 1250, 2400, IS_IDLE, RIGHT);
  game->man2 = initializeMan(game->renderer, MAN_UP, 220, 440, 45, 55, 1250, 2400, IS_IDLE, RIGHT);
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
    int tileIndex = x + y * game->map.width;

    // @TODO make this better
    int man2IndexX = (game->man2.x + game->man2.w)/game->map.tileSize;
    int man2IndexY = (game->man2.y + game->man2.h)/game->map.tileSize;
    printf("%d\n", man2IndexX);
    printf("%d\n", man2IndexY);
    int man2Index = (man2IndexX) + (man2IndexY) * game->map.width;
    if (tileIndex == man2Index) {
      game->map.tiles[tileIndex].isOccupied = 1;
    }


    if (x >= 0 && x < game->map.width && y>= 0 && y < game->map.height) {
      if (game->map.tiles[tileIndex].tileState == IS_SOLID || game->map.tiles[tileIndex].isOccupied == 1) {

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

  game->man.x += game->man.dx * game->dt; 
  game->man.y += game->man.dy * game->dt;


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
  game->man.status = game->man.dx != 0 || game->man.dy != 0 ? IS_RUNNING : IS_IDLE;

  // 60 FPS / 8 animations 
  if (fmod(game->time, 7.5) == 0) {
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
  // Create an application window with the following settings:
  game.window = SDL_CreateWindow( 
      "Man Jump",
      SDL_WINDOWPOS_UNDEFINED,           
      SDL_WINDOWPOS_UNDEFINED,           
      WINDOW_WIDTH,                               
      WINDOW_HEIGHT,                               
      SDL_WINDOW_SHOWN
      );

  game.gravity = 9.8 * PIXELS_PER_METER;

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
