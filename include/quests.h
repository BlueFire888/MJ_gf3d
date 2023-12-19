#ifndef __QUEST_H__
#define __QUEST_H__

#include "entity.h"
#include "Items.h"


typedef enum {
    Unobtained = 0,
    In_progress,
    Completed
}Quest_state;

typedef struct {
    Uint8 _inuse;
    Quest_state state;
    SJString* name;
    SJString* text;

}Quest_data; 

void quest_system_close();
void quest_system_init(Uint32 maxquests);
Quest_data* quest_new(SJString* message, SJString* name);
void load_quests_from_config(char* filename);
void quest_free(Quest_data* self);
Quest_data* getquests();
Uint32 getquestcount();
int getqueststate(char* name);
void quest_obtain(char* name);
void quest_complete(char* name);

#endif
