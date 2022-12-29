#include "simple_logger.h"

#include "player.h"
#include "mission.h"


typedef struct
{
    List *mission_list;
    Uint32 missionIdPool;
}MissionManager;

static MissionManager mission_manager = {0};

void mission_close()
{
    if (mission_manager.mission_list != NULL)
    {
        gfc_list_foreach(mission_manager.mission_list,(void (*)(void *))free);
        gfc_list_delete(mission_manager.mission_list);
    }
    memset(&mission_manager,0,sizeof(MissionManager));
}

void mission_init()
{
    mission_manager.mission_list = gfc_list_new();
    atexit(mission_close);
}

void mission_free(Mission *mission)
{
    if (!mission)return;
    
    free(mission);
}

Mission *mission_new()
{
    Mission *mission;
    mission = gfc_allocate_array(sizeof(Mission),1);
    if (!mission)return NULL;
    return mission;
}

Mission *mission_begin(
    const char *missionType,
    const char *missionSubject,
    const char *missionTarget,
    Uint32 dayStart,
    Uint32 dayFinished,
    Uint32 staff)
{
    Mission *mission;
    mission = mission_new();
    if (!mission)return NULL;
    
    mission->id = mission_manager.missionIdPool++;
    if (missionType)gfc_line_cpy(mission->missionType,missionType);
    if (missionTarget)gfc_line_cpy(mission->missionTarget,missionTarget);
    if (missionSubject)gfc_line_cpy(mission->missionSubject,missionSubject);
    mission->dayStart = dayStart;
    mission->dayFinished = dayFinished;
    mission->staff = staff;
    return mission;
}

void mission_execute(Mission *mission)
{
    if (!mission)return;
}

void mission_update_all()
{
    Uint32 day;
    Mission *mission;
    int i,c;
    day = player_get_day();
    c = gfc_list_get_count(mission_manager.mission_list);
    for (i = c - 1; i  >= 0; i--)
    {
        mission = gfc_list_get_nth(mission_manager.mission_list,i);
        if (!mission)return;
        if (day >= mission->dayFinished)
        {
            mission_execute(mission);
            mission_free(mission);
            gfc_list_delete_nth(mission_manager.mission_list,i);
        }
    }
}

SJson *mission_save_to_config(Mission *mission)
{
    SJson *json;
    if (!mission)return NULL;
    json = sj_object_new();
    if (!json)return NULL;
    
    sj_object_insert(json,"id",sj_new_uint32(mission->id));
    sj_object_insert(json,"dayStart",sj_new_uint32(mission->dayStart));
    sj_object_insert(json,"dayFinished",sj_new_uint32(mission->dayFinished));
    sj_object_insert(json,"staff",sj_new_uint32(mission->staff));
    sj_object_insert(json,"missionType",sj_new_str(mission->missionType));
    sj_object_insert(json,"missionTarget",sj_new_str(mission->missionTarget));
    sj_object_insert(json,"missionSubject",sj_new_str(mission->missionSubject));
    
    return json;
}

SJson *missions_save_to_config()
{
    int i,c;
    Mission *mission;
    SJson *json,*item,*array;
    json = sj_object_new();
    if (!json)return NULL;
    array = sj_array_new();
    if (!array)
    {
        sj_free(json);
        return NULL;
    }
    sj_object_insert(json,"idPool",sj_new_uint32(mission_manager.missionIdPool));
    c = gfc_list_get_count(mission_manager.mission_list);
    for (i = 0; i < c; i ++)
    {
        mission = gfc_list_get_nth(mission_manager.mission_list,i);
        if (!mission)continue;
        item = mission_save_to_config(mission);
        if (!item)continue;
        sj_array_append(array,item);
    }
    sj_object_insert(json,"missionList",array);
    return json;
}

Mission *mission_parse_from_config(SJson *config)
{
    const char *str;
    Mission *mission;
    if (!config)return NULL;
    mission = mission_new();
    if (!mission)return NULL;
   
    sj_object_get_value_as_uint32(config,"id",&mission->id);
    sj_object_get_value_as_uint32(config,"dayStart",&mission->dayStart);
    sj_object_get_value_as_uint32(config,"dayFinished",&mission->dayFinished);
    
    str = sj_object_get_value_as_string(config,"missionType");
    if (str)gfc_line_cpy(mission->missionType,str);
    str = sj_object_get_value_as_string(config,"missionTarget");
    if (str)gfc_line_cpy(mission->missionTarget,str);
    str = sj_object_get_value_as_string(config,"missionSubject");
    if (str)gfc_line_cpy(mission->missionSubject,str);
    return mission;
}

void missions_load_from_config(SJson *config)
{
    int i,c;
    Mission *mission;
    SJson *missions, *item;
    if (!config)return;
    if (mission_manager.mission_list!= NULL)
    {
        mission_close();//clear it all out!
    }
    mission_manager.mission_list = gfc_list_new();
    sj_object_get_value_as_uint32(config,"idPool",&mission_manager.missionIdPool);
    missions = sj_object_get_value(config,"missionList");
    c = sj_array_get_count(missions);
    for (i = 0; i < c; i++)
    {
        item = sj_array_get_nth(missions,i);
        if (!item)continue;
        mission = mission_parse_from_config(item);
        if (!mission)continue;
        mission_manager.mission_list = gfc_list_append(mission_manager.mission_list,mission);
    }
}

/*eol@eof*/
