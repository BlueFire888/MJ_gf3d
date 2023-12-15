#include "simple_logger.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "player.h"
#include "Items.h"

void player_add_inven(Inven_item item) {
    Entity* ent = NULL;
    ent = getPlayer();
    if (!ent) {
        slog("UGH OHHHH, no player for you!");
        return;
    }
    Player_data* p_data = (Player_data*)ent->customData;
    if (!p_data) {
        slog("L");
        return;
    }
    p_data->p_inven.inven_count++;
    //slog("%d", p_data->p_inven.inven_max);
    if (p_data->p_inven.inven_count > p_data->p_inven.inven_max) {
        slog("cannot add to inventory");
        return;
    }
    p_data->p_inven.inven[p_data->p_inven.inven_count] = item;
    //slog("%s", "ADDED");
}

void player_remove_inven_last(Inven_item item) {
    Entity* ent = NULL;
    ent = getPlayer();
    if (!ent) {
        slog("UGH OHHHH, no player for you!");
        return;
    }
    Player_data* p_data = (Player_data*)ent->customData;
    if (!p_data) {
        slog("L");
        return;
    }
    if (p_data->p_inven.inven_count == 0) {
        slog("cannot remove from inventory");
        return;
    }
    p_data->p_inven.inven_count--;
    p_data->p_inven.inven[p_data->p_inven.inven_count] = (Inven_item) { 0 };
}

void player_remove_inven(Inven_item item, int position) {
    Entity* ent = NULL;
    ent = getPlayer();
    if (!ent) {
        slog("UGH OHHHH, no player for you!");
        return;
    }
    Player_data* p_data = (Player_data*)ent->customData;
    if (!p_data) {
        slog("L");
        return;
    }
    Player_inven p_inven = p_data->p_inven;
    if (p_data->p_inven.inven_count == 0 || position < 0 || position >= p_data->p_inven.inven_max) {
        slog("cannot remove from inventory");
        return;
    }
    p_data->p_inven.inven_count--;
    p_data->p_inven.inven[position] = (Inven_item) NO_ITEM;
}

void equip_item(Inven_item item, int character, int slot) {
    Entity* ent = NULL;
    ent = getPlayer();
    if (!ent) {
        slog("UGH OHHHH, no player for you!");
        return;
    }
    Player_data* p_data = (Player_data*)ent->customData;
    if (!p_data) {
        slog("L");
        return;
    }
    Party_equipment* p_equipment = p_data->party.equipment;
    Inven_item* c_equipment = p_equipment[character].items_equipped;
    if ( c_equipment[slot].type != IT_NULL) {
        Inven_item t_Item = c_equipment[slot];
        t_Item.is_equipped = 0;
        player_add_inven(t_Item);
    }   
     item.is_equipped = 1;
     c_equipment[slot] = item;
}


