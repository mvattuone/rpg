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
  int id;
  int assignee_id;
  int target_id;
  QuestState state;
  QuestType type;
} Quest;

void add_quest(Quest *quests[], int *quest_count, int assignee_id, QuestType type);
