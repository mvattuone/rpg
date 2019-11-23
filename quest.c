#include <stdlib.h>
#include <stdio.h>
#include "quest.h"

void add_quest(Quest* quests[], int *quest_count, int assignee_id, QuestType type) {
    *quests = realloc(*quests, sizeof(*quests) + sizeof(Quest));
    *quest_count = *quest_count + 1;
    int index = *quest_count - 1;
    quests[index]->id = *quest_count;
    quests[index]->type = type;
    quests[index]->assignee_id = assignee_id;
    quests[index]->state = IN_PROGRESS;
    if (quests[index]->id == 1 && quests[index]->type == ITEM) {
      quests[index]->target_id = 6;
    } else if (quests[index]->id == 1 && quests[index]->type == TALK) {
      quests[index]->target_id = 3;
    }
};

