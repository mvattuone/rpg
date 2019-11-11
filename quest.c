#include <stdlib.h>
#include <stdio.h>
#include "quest.h"

void add_quest(Quest* quests[], int *quest_count, int assignee_id, QuestType type) {
    *quests = realloc(*quests, sizeof(*quests) + sizeof(Quest));
    *quest_count = *quest_count + 1;
    quests[*quest_count - 1]->id = *quest_count;
    quests[*quest_count - 1]->type = type;
    quests[*quest_count - 1]->assignee_id = assignee_id;
    quests[*quest_count - 1]->state = IN_PROGRESS;
    if (quests[*quest_count-1]->id == 1) {
      quests[*quest_count - 1]->target_id = 6;
    } else {
      quests[*quest_count -1]->target_id = 0;
    }
};

