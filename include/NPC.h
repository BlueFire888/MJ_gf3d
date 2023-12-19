#ifndef __NPC_H__
#define __NPC_H__

#include "entity.h"
#include "items.h"

typedef enum{
    NPC_dude = 0,
    NPC_obj,
    NPC_chest,
    NPC_heal,
    NPC_boost,
    NPC_lost,
    NPC_shop,
    NPC_board
}NPC_Type;

typedef struct {
    Uint8 max_depth;
    Uint8 current_depth;
    Uint8 *choices;

}Tree_data;


typedef struct {
    NPC_Type type;
    Sphere range;
    
    SJString* message;
    Uint8 talking; // used to determine if the NPC is activley talking\ 
    
    Uint8 has_dtree; // the current message being displayed by an NPC has branching paths 
    Tree_data t_data;
    SJson* file; //Json file object that is associated with this npc

    //NPC_obj
    Uint32 gold_c;

    //NPC_obj
    Inven_item item;
    //ITEM TYPE HERE

    //NPC_heal
    Uint32 heal_ammount;

    int quest_counter;

    Player_inven shop_inventory;

} NPC_data;

/**
 * @brief Create a new agumon entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an agumon entity pointer on success
 */
Entity* npc_new();

Entity* new_npc_from_config(char* filename);

NPC_data* npc_data_from_config(SJson* json, Entity* self);

SJson* get_next_message(SJson* cdatajson, Uint8* choice_array , int current_depth);

void npc_quest_update_3_1(Entity* self);
void npc_quest_update_3_2(Entity* self);

void npc_quest_update_1(Entity* self);

#endif
