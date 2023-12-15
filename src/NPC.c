
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

Entity* npc_new(Vector3D position)
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
    ent->model = gf3d_model_load("models/npc2.model");
    ent->think = npc_think;
    ent->update = npc_update;
    vector3d_copy(ent->position, position);
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
        if (!talking_f) {
            talking_f = (keys[SDL_SCANCODE_F]) ? 1 : 0;
            c_data->talking = talking_f;
            //slog(message);
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
                    equip_item(c_data->item, 0, 0);
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
    return;
}

/*eol@eof*/
