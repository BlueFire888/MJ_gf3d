#include "simple_logger.h"
#include "gfc_types.h"

#include "gf3d_camera.h"
#include "player.h"
#include "Items.h"

static int thirdPersonMode = 0;
void player_think(Entity *self);
void player_update(Entity *self);
void player_free(Entity* self);


void player_free(Entity* self) {
    if (!self) return;
    Player_data* p_data = (Player_data*)self->customData;
    free(&p_data->p_inven);
    free(p_data->party.members);
    for (int i  = 0; i < 4; i++) {
        free(&p_data->party.equipment[i]);
    }
    free(p_data->party.equipment);
}


static PlayerManager Player_manager = { 0 };

Entity *player_new(Vector3D position)
{
    Entity *ent = NULL;
    int i = 0, j = 0;
    
    ent = entity_new();
    if (!ent)
    {
        slog("UGH OHHHH, no player for you!");
        return NULL;
    }
    
    ent->model = gf3d_model_load("models/dino.model");
    ent->think = player_think;
    ent->update = player_update;
    ent->free = player_free;
    vector3d_copy(ent->position,position);
    ent->rotation.x = -GFC_PI;
    ent->rotation.z = -GFC_HALF_PI;
    ent->hidden = 1;
    ent->health = 50;
    ent->maxhealth = 100;
    ent->speed_f = 1;

    //make a reference to this player always avalaible 
    Player_manager.main_player = ent;
    return ent;
}


void player_think(Entity *self)
{
    Vector3D forward = {0};
    Vector3D right = {0};
    Vector2D w,mouse;
    int mx,my;
    SDL_GetRelativeMouseState(&mx,&my);
    const Uint8 * keys;
    keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
    if (!self)return;
    Uint8 factor = self->speed_f;

    mouse.x = mx;
    mouse.y = my;
    w = vector2d_from_angle(self->rotation.z);
    forward.x = w.x;
    forward.y = w.y;
    w = vector2d_from_angle(self->rotation.z - GFC_HALF_PI);
    right.x = w.x;
    right.y = w.y;

    forward = vector3d_multiply(forward, vector3d(0.5, 0.5, 0.5));
    right = vector3d_multiply(right, vector3d(0.5, 0.5, 0.5));
    forward = vector3d_multiply(forward, vector3d(factor, factor, factor));
    right = vector3d_multiply(right, vector3d(factor, factor, factor));
    if (keys[SDL_SCANCODE_W])
    {   
        vector3d_add(self->position,self->position,forward);
    }
    if (keys[SDL_SCANCODE_S])
    {
        vector3d_add(self->position,self->position,-forward);        
    }
    if (keys[SDL_SCANCODE_D])
    {
        vector3d_add(self->position,self->position,right);
    }
    if (keys[SDL_SCANCODE_A])    
    {
        vector3d_add(self->position,self->position,-right);
    }
    if (keys[SDL_SCANCODE_SPACE])self->position.z += 0.5 * factor;
    if (keys[SDL_SCANCODE_Z])self->position.z -= 0.5 * factor;
    
    if (keys[SDL_SCANCODE_UP])self->rotation.x -= 0.0050 * factor;
    if (keys[SDL_SCANCODE_DOWN])self->rotation.x += 0.0050 * factor;
    if (keys[SDL_SCANCODE_RIGHT])self->rotation.z -= 0.0050 * factor;
    if (keys[SDL_SCANCODE_LEFT])self->rotation.z += 0.0050 * factor;
    
    if (mouse.x != 0)self->rotation.z -= (mouse.x * 0.001);
    if (mouse.y != 0)self->rotation.x += (mouse.y * 0.001);

    if (keys[SDL_SCANCODE_F3])
    {
        thirdPersonMode = !thirdPersonMode;
        self->hidden = !self->hidden;
    }
}

void player_update(Entity *self)
{
    Vector3D forward = {0};
    Vector3D position;
    Vector3D rotation;
    Vector2D w;
    
    if (!self)return;
    
    vector3d_copy(position,self->position);
    vector3d_copy(rotation,self->rotation);
    if (thirdPersonMode)
    {
        position.z += 100;
        rotation.x += M_PI*0.125;
        w = vector2d_from_angle(self->rotation.z);
        forward.x = w.x * 100;
        forward.y = w.y * 100;
        vector3d_add(position,position,-forward);
    }
    gf3d_camera_set_position(position);
    gf3d_camera_set_rotation(rotation);
}

Entity* getPlayer() {
    Entity* ent = NULL;
    ent = Player_manager.main_player;
    if (!ent) {
        slog("UGH OHHHH, no player for you!");
    }
    return ent;
}

Vector3D player_position_get() {
    Entity* ent = NULL;
    ent = Player_manager.main_player;
    if (!ent) {
        slog("UGH OHHHH, no player for you!");
        return vector3d(0,0,0);
    }
    return ent->position;
}

void player_edit_gold(Uint32 gold_a) {
    Entity* ent = NULL;
    ent = Player_manager.main_player;
    if (!ent) {
        slog("UGH OHHHH, no player for you!");
        return;
    }
    Player_data* p_data = (Player_data*)ent->customData;
    if (!p_data) {
        slog("no more money");
        return;
    }
    p_data->gold += gold_a;
    slog("Gold: %d ", p_data->gold);
    slog("Added Gold: %d ", gold_a);
    ent->customData = p_data;
}

void player_heal(Uint32 heal_a) {
    Entity* ent = NULL;
    ent = Player_manager.main_player;
    if (!ent) {
        slog("UGH OHHHH, no player for you!");
        return;
    }
    ent->health = min((heal_a + ent->health),ent->maxhealth);
}

void player_boost() {
    Entity* ent = NULL;
    ent = Player_manager.main_player;
    if (!ent) {
        slog("UGH OHHHH, no player for you!");
        return;
    }
    ent->speed_f = 3;
}

/*eol@eof*/
