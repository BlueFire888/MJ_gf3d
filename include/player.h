#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "entity.h"
#include "Items.h"

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
    Uint16 health;
    Uint16 mana;
    Uint16 attack;
    Uint16 phys_defense;
    Uint16 magic_defense;
    Uint16 evasion;
    Uint8 speed;
    
}Party_stats;

typedef struct {
    Inven_item* items_equipped;
}Party_equipment;

typedef struct {
    Party_stats* members;
    Party_equipment* equipment;

}Player_Party;

typedef struct {
    Uint16 inven_count;
    Uint16 inven_max;
    Inven_item* inven;

} Player_inven;

typedef struct {
    Uint32 gold;
    Player_inven p_inven;
    Player_Party party;
    //MORE LMAO 
} Player_data;




Vector3D player_position_get();

Entity* getPlayer();

void player_edit_gold(Uint32 gold);

void player_heal(Uint32 heal_a);

void player_boost();

#endif
