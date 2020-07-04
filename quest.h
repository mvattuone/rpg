#ifndef QUEST_H
#define QUEST_H

typedef enum {
  IN_PROGRESS,
  COMPLETED,
  FAILED
} QuestState;

typedef enum {
  SWITCH,
  ITEM,
  TALK
} QuestType;

typedef struct {
  char* description;
  char* name;
  int id;
  int assignee_id;
  int target_id;
  int target_tile;
  QuestState state;
  QuestType type;
} Quest;

typedef struct {
  int capacity;
  int size; 
  Quest* items;
} ActiveQuests;

void add_quest(ActiveQuests *quests, int assignee_id, Quest *quest);
Quest* load_quests(char* file_path, int *quests_count); 

#endif
