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
    items = realloc(items, sizeof(items) + sizeof(buffer));
    items[i].id = i + 1;
    items[i].name = malloc(sizeof(buffer));
    strcpy(items[i].name, buffer);
    i++;
    *items_count = i;
  }   

  fclose(fp);
  return items;
};

