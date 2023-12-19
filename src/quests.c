#include "simple_logger.h"
#include "gfc_types.h"
#include "quests.h"


typedef struct
{
    Quest_data* quest_list;
    Uint32  quest_count;
}QuestManager;

static QuestManager quest_manager = { 0 };

void quest_system_close()
{
    int i;
    for (i = 0; i < quest_manager.quest_count; i++)
    {
        quest_free(&quest_manager.quest_list[i]);
    }
    free(quest_manager.quest_list);
    memset(&quest_manager, 0, sizeof(QuestManager));
    slog("quest_system closed");
}

void quest_free(Quest_data* self)
{
    if (!self)return;
    //MUST DESTROY
    sj_string_free(self->name);
    sj_string_free(self->text);
    memset(self, 0, sizeof(Quest_data));
}

void load_quests_from_config(char* filename) {
    SJson* json, *questjson, *arrayjson;
    int i = 0;
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to load json file (%s) for the quest data", filename);
        return NULL;
    }
    arrayjson = sj_object_get_value(json, "quests");
    if (!arrayjson)
    {
        slog("failed to find quest array in %s quest config", filename);
        sj_free(json);
        return NULL;
    }
    if (!sj_is_array(arrayjson)) {
        slog("Not an array silly");
        sj_free(json);
        return NULL;
    }
    int count = sj_array_get_count(arrayjson);
    for (i = 0; i < count ; i++)
    {
        questjson = sj_array_get_nth(arrayjson, i);
        if (!questjson) {
            continue;
        }
        quest_new(sj_string_new_text(sj_get_string_value(sj_object_get_value(questjson, "text")), 0), 
            sj_string_new_text(sj_get_string_value(sj_object_get_value(questjson, "name")), 0));
        //sj_free(questjson);
    }
    sj_free(json);
}

void quest_system_init(Uint32 maxquests)
{
    quest_manager.quest_list = gfc_allocate_array(sizeof(Quest_data), maxquests);
    if (quest_manager.quest_list == NULL)
    {
        slog("failed to allocate quest list, cannot allocate ZERO quests");
        return;
    }
    quest_manager.quest_count = maxquests;
    atexit(quest_system_close);
    slog("quest_system initialized");
}

Quest_data* quest_new(SJString* message, SJString* name)
{
    int i;
    for (i = 0; i < quest_manager.quest_count; i++)
    {
        if (!quest_manager.quest_list[i]._inuse)// not used yet, so we can!
        {
            quest_manager.quest_list[i]._inuse = 1;
            quest_manager.quest_list[i].name = name;
            quest_manager.quest_list[i].text = message;
            quest_manager.quest_list[i].state = Unobtained;
            return &quest_manager.quest_list[i];
        }
    }
    slog("entity_new: no free space in the entity list");
    return NULL;
}

Quest_data* getquests() {
    Quest_data* ent = NULL;
    ent = quest_manager.quest_list;
    if (!ent) {
        slog("UGH OHHHH, no quest list for you!");
    }
    return ent;
}

Uint32 getquestcount() {
    char* outval[128];
    sprintf(outval, "%d", quest_manager.quest_count);
    slog(outval);
    return quest_manager.quest_count;
}

int getqueststate(char* name) {
    int i;
    for (i = 0; i < quest_manager.quest_count; i++)
    {
        if (quest_manager.quest_list[i]._inuse && strcmp(quest_manager.quest_list[i].name->text, name) == 0)
        {
            return quest_manager.quest_list[i].state;
        }
    }
}

void quest_obtain(char* name) {
    int i;
    for (i = 0; i < quest_manager.quest_count; i++)
    {
        if (quest_manager.quest_list[i]._inuse && quest_manager.quest_list[i].state == Unobtained && strcmp(quest_manager.quest_list[i].name->text, name) == 0)// not used yet, so we can!
        {
            quest_manager.quest_list[i].state = In_progress;
        }
    }
}


void quest_complete(char* name) {
    int i;
    for (i = 0; i < quest_manager.quest_count; i++)
    {
        if (quest_manager.quest_list[i]._inuse && quest_manager.quest_list[i].state == In_progress && strcmp(quest_manager.quest_list[i].name->text, name) == 0)// not used yet, so we can!
        {
            quest_manager.quest_list[i].state = Completed;
        }
    }
}
