#ifndef __ITEM_H__
#define __ITEM_H__

#include "entity.h"

#define NO_ITEM {"No Item",0,0,0,0,0,0,0,0,0}

typedef enum
{
    IT_NULL = 0,
    IT_sword,
    IT_dagger,
    IT_bow,
    IT_staff,
    IT_helmet,
    IT_armor,
    IT_accessory
}Item_Type;

typedef struct {
    char* item_name;
    Item_Type type;
    Uint8 is_equipped;
    Uint16 health;
    Uint16 mana;
    Uint16 attack;
    Uint16 phys_defense;
    Uint16 magic_defense;
    Uint16 evasion;
    Uint8 speed;
    //STATS
}Inven_item;

void player_add_inven(Inven_item item);

void player_remove_inven_last(Inven_item item);

void player_remove_inven(Inven_item item, int position);

void equip_item(Inven_item item, int character,int slot);

int is_noitem(Inven_item item);


#endif