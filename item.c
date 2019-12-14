#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "item.h"

Item* load_items(char* file_name, int *items_count) {
  FILE *fp;
  char buffer[1024];
  int i = 0;
  Item *items = malloc(sizeof(Item));

  fp = fopen(file_name, "r");
  if (fp == NULL) {
    printf("Could not open file");
    exit(1);
  }

  while (fgets(buffer, 1024, fp) != NULL) {
    fflush(stdout);
    if (buffer[0] == '*') {
      items = realloc(items, sizeof(items) + sizeof(buffer) * 2);
      items[i].id = i;
    } else if (buffer[0] == '$') {
      char *bufferPtr = buffer;
      bufferPtr++;
      bufferPtr[strlen(bufferPtr) - 1] = 0;
      items[i].name = malloc(sizeof(buffer));
      strcpy(items[i].name, bufferPtr);
    } else if (buffer[0] == '%') {
      char *bufferPtr = buffer;
      bufferPtr++;
      bufferPtr[strlen(bufferPtr) - 1] = 0;
      items[i].description = malloc(sizeof(buffer));
      strcpy(items[i].description, bufferPtr);
      *items_count = i + 1;
      i++;
    }
  }   

  fclose(fp);
  return items;
};

