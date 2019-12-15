#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "quest.h"

void add_quest(ActiveQuests *quests, int assignee_id, Quest *quest) {
  if (&quests->size > &quests->capacity) {
    quests->items = realloc(quests->items, (sizeof(quests->items) * sizeof(Quest)) + sizeof(Quest));
    quests->capacity = sizeof(quests->items) + sizeof(Quest);
  }

    quests->items[quests->size] = *quest;
    quests->items[quests->size].assignee_id = assignee_id;
    quests->items[quests->size].state = IN_PROGRESS;
    quests->size++;
};

Quest* load_quests(char* file_name, int *quests_count) {
  FILE *fp;
  char buffer[1024];
  int i = 1;
  Quest *quests = malloc(sizeof(Quest));

  fp = fopen(file_name, "r");
  if (fp == NULL) {
    printf("Could not open file");
    exit(1);
  }

  while (fgets(buffer, 1024, fp) != NULL) {
    if (buffer[0] == '*') {
      quests = realloc(quests , sizeof(quests) + sizeof(buffer) * 2);
      quests[i].id = i;
    } else if (buffer[0] == '#') {
      char *bufferPtr = buffer;
      bufferPtr++;
      bufferPtr[strlen(bufferPtr) - 1] = 0;
      quests[i].type = atoi(bufferPtr);
    } else if (buffer[0] == '@') {
      char *bufferPtr = buffer;
      bufferPtr++;
      bufferPtr[strlen(bufferPtr) - 1] = 0;
      quests[i].target_id = atoi(bufferPtr);
    } else if (buffer[0] == '$') {
      char *bufferPtr = buffer;
      bufferPtr++;
      bufferPtr[strlen(bufferPtr) - 1] = 0;
      quests[i].name = malloc(sizeof(buffer));
      strcpy(quests[i].name, bufferPtr);
    } else if (buffer[0] == '%') {
      char *bufferPtr = buffer;
      bufferPtr++;
      bufferPtr[strlen(bufferPtr) - 1] = 0;
      quests[i].description = malloc(sizeof(buffer));
      strcpy(quests[i].description, bufferPtr);
      *quests_count = i + 1;
      i++;
    }
  }   

  fclose(fp);
  return quests;
};

