#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "item.h"
#include "utils.h"

#define ITEM_ID '*'
#define ITEM_NAME '$'
#define ITEM_DESCRIPTION '%'

Item* load_items(char* file_path, int *items_count) {
  FILE *fp;
  char buffer[1024];
  int i = 0;
  Item *items = malloc(sizeof(Item));

  fp = load_file(file_path);

  while (fgets(buffer, 1024, fp) != NULL) {
    char dataLineSymbol = buffer[0];
    if (dataLineSymbol == ITEM_ID) {
      items = realloc(items , sizeof(items) + sizeof(buffer) * 2);
      items[i].id = i;
      continue;
    } 
    
    char *bufferPtr = buffer;
    bufferPtr++;
    bufferPtr[strlen(bufferPtr) - 1] = 0;

    if (dataLineSymbol == ITEM_NAME) {
      items[i].name = malloc(sizeof(buffer));
      strcpy(items[i].name, bufferPtr);
      continue;
    } else if (dataLineSymbol == ITEM_DESCRIPTION) {
      items[i].description = malloc(sizeof(buffer));
      strcpy(items[i].description, bufferPtr);
      *items_count = i + 1;
      i++;
      continue;
    }
  }   

  fclose(fp);
  return items;
};

