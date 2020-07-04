#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "quest.h"
#include "utils.h"

#define QUEST_ID '*'
#define QUEST_TYPE '#'
#define QUEST_TARGET_ID '@'
#define QUEST_NAME '$'
#define QUEST_TARGET_TILE '&'
#define QUEST_DESCRIPTION '%'

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

Quest* load_quests(char* file_path, int *quests_count) {
  FILE *fp;
  char buffer[1024];
  int i = 1;
  Quest *quests = malloc(sizeof(Quest));

  fp = load_file(file_path);

  while (fgets(buffer, 1024, fp) != NULL) {
    char dataLineSymbol = buffer[0];
    if (dataLineSymbol == QUEST_ID) {
      quests = realloc(quests , sizeof(quests) + sizeof(buffer) * 2);
      quests[i].id = i;
      continue;
    } 
    
    char *bufferPtr = buffer;
    bufferPtr++;
    bufferPtr[strlen(bufferPtr) - 1] = 0;

    if (dataLineSymbol == QUEST_TYPE) {
      quests[i].type = atoi(bufferPtr);
    } else if (dataLineSymbol == QUEST_TARGET_ID) {
      quests[i].target_id = atoi(bufferPtr);
      continue;
    } else if (dataLineSymbol == QUEST_NAME) {
      quests[i].name = malloc(sizeof(buffer));
      strcpy(quests[i].name, bufferPtr);
      continue;
    } else if (dataLineSymbol == QUEST_TARGET_TILE) {
      quests[i].target_tile = atoi(bufferPtr);
      continue;
    } else if (dataLineSymbol == QUEST_DESCRIPTION) {
      quests[i].description = malloc(sizeof(buffer));
      strcpy(quests[i].description, bufferPtr);
      *quests_count = i + 1;
      i++;
      continue;
    }
  }   

  fclose(fp);
  return quests;
};

