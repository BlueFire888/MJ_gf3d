#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"

/**
 * @brief Create a new player entity
 * @param position where to spawn the aguman at
 * @return NULL on error, or an player entity pointer on success
 */
Entity *player_new(Vector3D position);

typedef struct {
    Entity* main_player;
} PlayerManager;


typedef struct {
    char* item_name;
    //STATS
}Inven_item;

typedef struct {
    Uint16 inven_count;
    Uint16 inven_max;
    Inven_item* inven;

} Player_inven;

typedef struct {
    Uint32 gold;
    Player_inven p_inven;
    //MORE LMAO 
} Player_data;




Vector3D player_position_get();

void player_edit_gold(Uint32 gold);

void player_add_inven(char* item_name);

void player_heal(Uint32 heal_a);

void player_boost();

#endif
