
#include "simple_logger.h"

#include "gf3d_vgraphics.h"
#include "gf3d_pipeline.h"
#include "gf3d_swapchain.h"
#include "gf3d_model.h"
#include "gf3d_camera.h"
#include "gf3d_texture.h"
#include "gf3d_particle.h"

#include "gf2d_sprite.h"
#include "gf2d_font.h"
#include "gf2d_draw.h"

#include "player.h"
#include "NPC.h"
#include "Items.h"

void npc_update(Entity* self);

void npc_think(Entity* self);

void npc_free(Entity* self);

Entity* new_npc_from_config(char* filename) {
    SJson* json, *npcjson;
    Entity* npc = NULL;
    const char* modelName = NULL;
    npc = npc_new();
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to load json file (%s) for the world data", filename);
        free(npc);
        return NULL;
    }
    npcjson = sj_object_get_value(json, "npc");
    if (!npcjson)
    {
        slog("failed to find npc object in %s world config", filename);
        free(npc);
        sj_free(json);
        return NULL;
    }
    modelName = sj_get_string_value(sj_object_get_value(npcjson, "model"));
    if (!modelName)
    {
        slog("world data (%s) has no model", filename);
        sj_free(json);
        return npc;
    }
    npc->model = gf3d_model_load(modelName);
    sj_value_as_vector3d(sj_object_get_value(npcjson, "position"), &npc->position);
    npc->customData = npc_data_from_config(json, npc);
    sj_free(json);
    return npc;

}

NPC_data* npc_data_from_config(SJson* json, Entity* self) {
    if (!json) return NULL;
    if (!self) return NULL;
    //const char* message;
    SJString* message , *itemname;
    int radius;
    SJson* cdatajson, *itemdatajson;
    NPC_data* npc_cdata = NULL;
    npc_cdata = gfc_allocate_array(sizeof(NPC_data), 1);
    if (!npc_cdata) {
        slog("could not allocate custom data");
        return NULL;
    }
    cdatajson = sj_object_get_value(json, "custom_data");
    if (!cdatajson)
    {
        slog("failed to find custom data object in npc config");
        free(npc_cdata);
        return NULL;
    }
    sj_get_integer_value(sj_object_get_value(cdatajson, "type"), &npc_cdata->type);
    sj_get_integer_value(sj_object_get_value(cdatajson, "radius"), &radius);
    npc_cdata->range = gfc_sphere(self->position.x, self->position.y, self->position.z, radius);
    npc_cdata->talking = 0;
    message = sj_string_new_text(sj_get_string_value(sj_object_get_value(cdatajson, "message")),0);
    if (!message)
    {
        slog("npc has no message data");
    }
    npc_cdata->message = message->text;
    if (sj_object_get_value(cdatajson, "gold") == NULL) {
        npc_cdata->gold_c = 0;
    } else {
        sj_get_integer_value(sj_object_get_value(cdatajson, "gold"), &npc_cdata->gold_c);
    }
    if (sj_object_get_value(cdatajson, "Item") == NULL) {
        npc_cdata->item = (Inven_item)NO_ITEM;
    }
    else {
        itemdatajson = sj_object_get_value(cdatajson, "Item");
        itemname = sj_string_new_text(sj_get_string_value(sj_object_get_value(itemdatajson, "name")), 0);
        if (!itemname)
        {
            slog("item has no name");
            npc_cdata->item.item_name = "NO ITEM";
        }
        npc_cdata->item.item_name = itemname->text;
        sj_object_get_value_as_int(itemdatajson, "type", &npc_cdata->item.type);
        sj_object_get_value_as_int(itemdatajson, "health", &npc_cdata->item.health);
        sj_object_get_value_as_int(itemdatajson, "mana", &npc_cdata->item.mana);
        sj_object_get_value_as_int(itemdatajson, "attack", &npc_cdata->item.attack);
        sj_object_get_value_as_int(itemdatajson, "phys_def", &npc_cdata->item.phys_defense);
        sj_object_get_value_as_int(itemdatajson, "elem_def", &npc_cdata->item.magic_defense);
        sj_object_get_value_as_int(itemdatajson, "evasion", &npc_cdata->item.evasion);
        sj_object_get_value_as_int(itemdatajson, "speed", &npc_cdata->item.speed);
        npc_cdata->item.is_equipped = 0;
    }
    if (sj_object_get_value(cdatajson, "heal_ammount") == NULL) {
        npc_cdata->heal_ammount = 0;
    }
    else {
        sj_get_integer_value(sj_object_get_value(cdatajson, "heal_ammount"), &npc_cdata->heal_ammount);
    }
    return npc_cdata;
}

Entity* npc_new()
{
    Entity* ent = NULL;

    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no agumon for you!");
        return NULL;
    }
    ent->selectedColor = gfc_color(0.1, 0.1, 1, 1);
    ent->color = gfc_color(1, 1, 1, 1);
    ent->think = npc_think;
    ent->update = npc_update;
    return ent;
}

void npc_update(Entity* self)
{
    if (!self)
    {
        slog("self pointer not provided");
        return;
    }
    //vector3d_add(self->position, self->position, self->velocity);
    //self->rotation.z += 0.01;
}

void npc_think(Entity* self)
{
    if (!self) {
        slog("KABOOM");
        return;
    }
    NPC_data *c_data = (NPC_data*)(self->customData);
    if (!c_data) {
        slog("No custom data silly");
        return;
    }
    Sphere aggro_r = c_data->range;
    char* message = c_data->message;
    Uint8 talking_f = c_data->talking;
    const Uint8* keys;
    keys = SDL_GetKeyboardState(NULL);
    if (!keys) { return; }
    Vector3D player_pos = player_position_get();
    if (gfc_point_in_sphere(player_pos, aggro_r)) {
        //slog(message);
        if (!talking_f) {
            c_data->talking = (keys[SDL_SCANCODE_F]) ? 1 : 0;
            
        }
        switch (c_data->type) {
            case NPC_obj:
                if (!self->hidden && keys[SDL_SCANCODE_F]){
                    self->hidden = 1;
                    player_edit_gold(c_data->gold_c);
                    break;
                }
            case NPC_chest:
                if (!self->hidden && keys[SDL_SCANCODE_F]) {
                    self->hidden = 1;
                    player_add_inven(c_data->item);
                    equip_item(&c_data->item, 0, 0);
                    break;
                }
            case NPC_heal:
                if (!self->hidden && keys[SDL_SCANCODE_F]) {
                    self->hidden = 1;
                    player_heal(c_data->heal_ammount);
                    break;
                }
            case NPC_boost:
                if (!self->hidden && keys[SDL_SCANCODE_F]) {
                    self->hidden = 1;
                    player_boost();
                    break;
                }
        }
    } else {c_data->talking = 0;}
}
void npc_free(Entity* self) {
    if (!self) { return; }
    free(self->customData);
    return;
}

/*eol@eof*/
