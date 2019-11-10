#include <SDL2/SDL_image.h>
#include "rpg.h"

int moveLeft(DynamicObject *dynamic_object, int tileDistance, int *tileSize) {
  if (fabs(dynamic_object->totalMovedX) >= (tileDistance * *(int*)tileSize)/2) {
    dynamic_object->isMoving = 0;
    dynamic_object->moveLeft = 0;
    dynamic_object->totalMovedX = 0;
    dynamic_object->startingX = dynamic_object->x;
    return 0;
  } else { 
    dynamic_object->direction = LEFT;
    dynamic_object->isMoving = 1;
    dynamic_object->moveLeft = 1;
    dynamic_object->totalMovedX = dynamic_object->x - dynamic_object->startingX;
    return 1;
  }
}

int moveRight(DynamicObject *dynamic_object, int tileDistance, int *tileSize) {
  if (fabs(dynamic_object->totalMovedX) >= (tileDistance * *(int*)tileSize)/2) {
    dynamic_object->isMoving = 0;
    dynamic_object->moveRight = 0;
    dynamic_object->totalMovedX = 0;
    dynamic_object->startingX = dynamic_object->x;
    return 0;
  } else { 
    dynamic_object->direction = RIGHT;
    dynamic_object->isMoving = 1;
    dynamic_object->moveRight = 1;
    dynamic_object->totalMovedX = dynamic_object->x - dynamic_object->startingX;
    return 1;
  }
}

int moveUp(DynamicObject *dynamic_object, int tileDistance, int *tileSize) {
  if (fabs(dynamic_object->totalMovedY) >= (tileDistance * *(int*)tileSize)/2) {
    dynamic_object->isMoving = 0;
    dynamic_object->moveUp = 0;
    dynamic_object->totalMovedY = 0;
    dynamic_object->startingY = dynamic_object->y;
    return 0;
  } else { 
    dynamic_object->direction = UP;
    dynamic_object->isMoving = 1;
    dynamic_object->moveUp = 1;
    dynamic_object->totalMovedY = dynamic_object->y - dynamic_object->startingY;
    return 1;
  }
}

int moveDown(DynamicObject *dynamic_object, int tileDistance, int *tileSize) {
  if (fabs(dynamic_object->totalMovedY) >= (tileDistance * *(int*)tileSize)/2) {
    dynamic_object->isMoving = 0;
    dynamic_object->moveDown = 0;
    dynamic_object->totalMovedY = 0;
    dynamic_object->startingY = dynamic_object->y;
    return 0;
  } else { 
    dynamic_object->direction = DOWN;
    dynamic_object->isMoving = 1;
    dynamic_object->moveDown = 1;
    dynamic_object->totalMovedY = dynamic_object->y - dynamic_object->startingY;
    return 1;
  }
}

int speak(DynamicObject *dynamic_object, char* text, int *dismissDialog, time_t duration) {
  time_t timer = SDL_GetTicks() / 1000;
  time_t passedDuration = duration && timer - dynamic_object->dialogue_queue.timer;
  if (*dismissDialog || passedDuration) {
    dynamic_object->dialogue_queue.timer= 0;
    dynamic_object->currentDialog = NULL;
    *dismissDialog = 0;
    return 0;
  } else {
    dynamic_object->currentDialog = text;
    *dismissDialog = 0;
    return 1;
  }
}

int process_queue(DynamicObject *dynamic_object, Queue *queue) {
  int running;
  if (!queue->is_enqueuing) {
    QueueItem queue_item = queue->items[0];
    running = queue_item.action(dynamic_object, queue_item.arg1, queue_item.arg2, queue_item.arg3);
  } else {
    running = 0;
  }
  return running;
}

void enqueue(Queue *queue, generic_function action, void* arg1, void* arg2, void* arg3){
  queue->is_enqueuing = 1;
   queue->items = realloc(queue->items, sizeof(queue->items) * 10);
   if (queue->size >= queue->capacity) {
     queue->capacity = queue->size * 2;
   }
   queue->size++;
   queue->items[queue->size - 1].action = action;
   queue->items[queue->size - 1].arg1= arg1;
   queue->items[queue->size - 1].arg2= arg2;
   queue->items[queue->size - 1].arg3= arg3;
   queue->is_enqueuing = 0;
}

Queue dequeue(Queue *queue) 
{
    if (queue->size < 1) {
      printf("Attempted to remove action when there were none present.");
      SDL_Quit();
      exit(1);
    }

    Queue new_queue;
    new_queue.items = malloc((queue->size - 1) * sizeof(*new_queue.items)); 

    if (queue->size != 1) {
        memcpy(new_queue.items, &queue->items[1], (queue->size - 1) * sizeof(*new_queue.items)); 
    } 

    new_queue.size = queue->size - 1;
    new_queue.capacity = queue->capacity; 
    new_queue.prev_size = queue->size;
    new_queue.is_enqueuing = 0;

    free(queue->items);
    return new_queue;
}

DynamicObject initialize_dynamic_object(SDL_Renderer *renderer, DynamicObject *dynamic_object, int spriteValue, float angle, float mass, float walkThrust, float runThrust, Status status, Direction direction, ObjectType type) {
  dynamic_object->action_queue.items = malloc(sizeof(QueueItem));
  dynamic_object->action_queue.is_enqueuing = 0;
  dynamic_object->action_queue.prev_size = 0;
  dynamic_object->action_queue.size = 0;
  dynamic_object->action_queue.capacity = 1;
  dynamic_object->action_queue.timer = 0;
  dynamic_object->dialogue_queue.items = malloc(sizeof(QueueItem));
  dynamic_object->dialogue_queue.is_enqueuing = 0;
  dynamic_object->dialogue_queue.prev_size = 0;
  dynamic_object->dialogue_queue.size = 0;
  dynamic_object->dialogue_queue.capacity = 1;
  dynamic_object->dialogue_queue.timer = 0;
  dynamic_object->angle = angle;
  dynamic_object->mass = mass;
  dynamic_object->normalForce = dynamic_object->mass * GRAVITY * cos(90*M_PI); 
  dynamic_object->walkThrust = walkThrust; 
  dynamic_object->runThrust = runThrust; 
  dynamic_object->thrustX = 0;
  dynamic_object->thrustY = 0;
  dynamic_object->directionX = 0;
  dynamic_object->directionY = 0;
  dynamic_object->frictionalForceX = 0;
  dynamic_object->frictionalForceY = 0;
  dynamic_object->isMoving = 0;
  dynamic_object->isPushing = 0;
  dynamic_object->isRunning = 0;
  dynamic_object->moveLeft = 0;
  dynamic_object->moveRight = 0;
  dynamic_object->moveUp = 0;
  dynamic_object->moveDown = 0;
  dynamic_object->totalMovedX = 0;
  dynamic_object->totalMovedY = 0;
  dynamic_object->ax= 0;
  dynamic_object->ay= 0;
  dynamic_object->dx = 0;
  dynamic_object->dy = 0;
  dynamic_object->triggerDialog = 0;
  dynamic_object->status = status;
  dynamic_object->sprite = spriteValue;
  dynamic_object->direction = direction;
  dynamic_object->currentDialog = NULL;
  dynamic_object->state = DEFAULT;
  dynamic_object->idleTexture = NULL;
  dynamic_object->jarTexture = NULL;
  dynamic_object->crateTexture = NULL;
  dynamic_object->runningTexture = NULL;

  if (type == MAN) {
    SDL_Surface *manIdleSurface = createSurface("images/man-idle.png");
    SDL_Surface *manRunningSurface = createSurface("images/man-running.png");
    dynamic_object->idleTexture= SDL_CreateTextureFromSurface(renderer, manIdleSurface);
    dynamic_object->runningTexture= SDL_CreateTextureFromSurface(renderer, manRunningSurface);
    dynamic_object->w = manIdleSurface->w / 8;
    dynamic_object->h = manIdleSurface->h / 8;
    SDL_FreeSurface(manRunningSurface);
    SDL_FreeSurface(manIdleSurface);
  }

  if (type == CRATE) {
    SDL_Surface *crateSurface = createSurface("images/crate.png");
    dynamic_object->crateTexture= SDL_CreateTextureFromSurface(renderer, crateSurface);
    dynamic_object->w = crateSurface->w;
    dynamic_object->h = crateSurface->h;
    SDL_FreeSurface(crateSurface);
  }

  if (type == JAR) {
    SDL_Surface *jarSurface = createSurface("images/jar.png");
    dynamic_object->jarTexture= SDL_CreateTextureFromSurface(renderer, jarSurface);
    dynamic_object->w = jarSurface->w / 2;
    dynamic_object->h = jarSurface->h / 2;
    SDL_FreeSurface(jarSurface);
  }

  return *dynamic_object;
}
