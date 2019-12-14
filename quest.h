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
  QuestState state;
  QuestType type;
} Quest;

typedef struct {
  int capacity;
  int size; 
  Quest* items;
} ActiveQuests;

void add_quest(ActiveQuests *quests, int assignee_id, Quest *quest);
Quest* load_quests(char* file_name, int *quests_count); 
