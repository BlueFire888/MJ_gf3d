#ifndef __NPC_H__
#define __NPC_H__

#include "entity.h"
#include "items.h"

typedef enum{
    NPC_dude = 0,
    NPC_obj,
    NPC_chest,
    NPC_heal,
    NPC_boost
}NPC_Type;

typedef struct {
    NPC_Type type;
    Sphere range;
    char* message;
    Uint8 talking;

    //NPC_obj
    Uint32 gold_c;

    //NPC_obj
    Inven_item item;
    //ITEM TYPE HERE

    //NPC_heal
    Uint32 heal_ammount;

} NPC_data;

/**
 * @brief Create a new agumon entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an agumon entity pointer on success
 */
Entity* npc_new(Vector3D position);


#endif
