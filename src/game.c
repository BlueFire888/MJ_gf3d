#include <SDL.h>            

#include "simple_logger.h"
#include "gfc_input.h"
#include "gfc_vector.h"
#include "gfc_matrix.h"

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

#include "entity.h"
#include "agumon.h"
#include "player.h"
#include "world.h"
#include "NPC.h"
#include "Items.h"

extern int __DEBUG;

int main(int argc,char *argv[])
{
    int done = 0;
    int a,b=0;
    
    Sprite *mouse = NULL;
    int mousex,mousey;
    //Uint32 then;
    float mouseFrame = 0;
    World *w;
    Entity *npc = NULL,*player = NULL,*npc2 = NULL,*npc3 = NULL,*npc4 = NULL,*npc5 = NULL;
    Particle particle[100];
    Matrix4 skyMat;
    Model *sky;
    char* outval[128];

    for (a = 1; a < argc;a++)
    {
        if (strcmp(argv[a],"--debug") == 0)
        {
            __DEBUG = 1;
        }
    }
    
    init_logger("gf3d.log",0);    
    gfc_input_init("config/input.cfg");
    slog("gf3d begin");
    gf3d_vgraphics_init("config/setup.cfg");
    gf2d_font_init("config/font.cfg");
    gf2d_draw_manager_init(1000);
    
    slog_sync();
    
    entity_system_init(1024);
    
    mouse = gf2d_sprite_load("images/pointer.png",32,32, 16);
    
    
    npc = new_npc_from_config("config/npc1.json");
    npc2 = new_npc_from_config("config/npc2.json");
    npc3 = new_npc_from_config("config/npc3.json");
    npc4 = new_npc_from_config("config/npc4.json");
    npc5 = new_npc_from_config("config/npc5.json");

    w = world_load("config/testworld.json");
    
    SDL_SetRelativeMouseMode(SDL_TRUE);
    slog_sync();
    gf3d_camera_set_scale(vector3d(1,1,1));
    player = player_new(vector3d(-50,0,0));
    if (!player) {
        return;
    }
    Player_data p_data = { 0 };
    Player_inven p_inven = p_data.p_inven;
    p_data.gold = 0;
    p_inven.inven_max = 64;
    p_inven.inven_count = 0;

    //create inventory and populate it with NO_ITEMs
    p_inven.inven = (Inven_item*)gfc_allocate_array(sizeof(Inven_item), 64);
    if (!p_inven.inven) {
        slog("NO INVENTORY FOR YOU");
        return;
    }
    for (a = 0; a < 64; a++) {
        p_inven.inven[a] = (Inven_item)NO_ITEM;
    }
    //create the party's equipment and populate it with NO_ITEMS
    Player_Party p_party = p_data.party;
    p_party.equipment = (Party_equipment*)gfc_allocate_array(sizeof(Party_equipment), 4);
    if (!p_party.equipment) {
        slog("NO EQUIPMENT");
        return;
    }
    for (a = 0; a < 4; a++) {
        p_party.equipment[a].items_equipped = (Inven_item*)gfc_allocate_array(sizeof(Inven_item), 5);
        if (!p_party.equipment[a].items_equipped) {
            slog("NO EQUIPMENT for character ");
            return;
        }
    }
    for (a = 0; a< 4; a++) {
        for (b = 0; b < 5; b++) {
            p_party.equipment[a].items_equipped[b] = (Inven_item)NO_ITEM;
        }
    }
    //create the party's stats and make them all zero
    p_party.members = (Party_stats*)gfc_allocate_array(sizeof(Party_stats), 4);
    if (!p_party.members) {
        slog("NO STATS");
        return;
    }
    for (a = 0; a < 4; a++) {
        p_party.members[a] = (Party_stats){ 0 };
    }

    // assign newly made data back to the player we made
    p_data.p_inven = p_inven;
    p_data.party = p_party;
    player->customData = &p_data;
    sky = gf3d_model_load("models/sky.model");
    gfc_matrix_identity(skyMat);
    gfc_matrix_scale(skyMat,vector3d(100,100,100));
    
    // main game loop
    slog("gf3d main loop begin");
    while(!done)
    {
        gfc_input_update();
        gf2d_font_update();
        SDL_GetMouseState(&mousex,&mousey);

               
        mouseFrame += 0.01;
        if (mouseFrame >= 16)mouseFrame = 0;
        world_run_updates(w);
        entity_think_all();
        entity_update_all();
        gf3d_camera_update_view();
        gf3d_camera_get_view_mat4(gf3d_vgraphics_get_view_matrix());

        p_data = *(Player_data*)player->customData;
        p_inven = p_data.p_inven;
        p_party = p_data.party;
        Party_stats player_stats = p_party.members[0];
        Party_equipment* p_equipment = p_party.equipment;
        Inven_item* p_equiped_stats = p_equipment[0].items_equipped;

        sprintf(outval, "%d", p_data.gold);
        

        gf3d_vgraphics_render_start();

            //3D draws
                gf3d_model_draw_sky(sky,skyMat,gfc_color(1,1,1,1));
                world_draw(w);
                entity_draw_all();
                
                
                gf2d_draw_rect_filled(gfc_rect(10, 10, 250, 400), gfc_color8(122, 97, 71, 255));
                gf2d_font_draw_line_tag("Gold:", FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 10));
                gf2d_font_draw_line_tag(outval, FT_H2, gfc_color(1, 1, 1, 1), vector2d(70, 10));

                gf2d_font_draw_line_tag("Health:", FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 50));
                sprintf(outval, "%d", player_stats.health + p_equiped_stats->health + player->health);
                gf2d_font_draw_line_tag(outval, FT_H2, gfc_color(1, 1, 1, 1), vector2d(70, 50));
                
                gf2d_font_draw_line_tag("Mana:", FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 70));
                sprintf(outval, "%d", player_stats.mana + p_equiped_stats->mana);
                gf2d_font_draw_line_tag(outval, FT_H2, gfc_color(1, 1, 1, 1), vector2d(70, 70));
                
                gf2d_font_draw_line_tag("ATK:", FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 90));
                sprintf(outval, "%d", player_stats.attack + p_equiped_stats->attack);
                gf2d_font_draw_line_tag(outval, FT_H2, gfc_color(1, 1, 1, 1), vector2d(70, 90));
                
                gf2d_font_draw_line_tag("PHYS-DEF:", FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 110));
                sprintf(outval, "%d", player_stats.phys_defense + p_equiped_stats->phys_defense);
                gf2d_font_draw_line_tag(outval, FT_H2, gfc_color(1, 1, 1, 1), vector2d(100, 110));
                
                gf2d_font_draw_line_tag("ELEM-DEF:", FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 130));
                sprintf(outval, "%d", player_stats.magic_defense + p_equiped_stats->magic_defense);
                gf2d_font_draw_line_tag(outval, FT_H2, gfc_color(1, 1, 1, 1), vector2d(100, 130));
                
                gf2d_font_draw_line_tag("EVADE:", FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 150));
                sprintf(outval, "%d", player_stats.evasion + p_equiped_stats->evasion);
                gf2d_font_draw_line_tag(outval, FT_H2, gfc_color(1, 1, 1, 1), vector2d(70, 150));
                
                gf2d_font_draw_line_tag("SPD:", FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 170));
                sprintf(outval, "%d", player_stats.speed + p_equiped_stats->speed);
                gf2d_font_draw_line_tag(outval, FT_H2, gfc_color(1, 1, 1, 1), vector2d(70, 170));




         ///       gf2d_draw_rect(gfc_rect(10, 10, 250, 70), gfc_color8(255, 255, 255, 255));
                
                
                //sprintf(outval, "%d", c_data.talking);
                //slog(outval);
                if ( ((NPC_data*)(npc->customData))->talking ) {

                    gf2d_font_draw_line_tag( ((NPC_data*)(npc->customData))->message , FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 30));
                } else if ( ((NPC_data*)(npc2->customData))->talking ) {

                    gf2d_font_draw_line_tag( ((NPC_data*)(npc2->customData))->message , FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 30));
                }
                else if ( ((NPC_data*)(npc3->customData))->talking ) {
                    gf2d_font_draw_line_tag( ((NPC_data*)(npc3->customData))->message , FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 30));
                }
                else if ( ((NPC_data*)(npc4->customData))->talking ) {
                    gf2d_font_draw_line_tag( ((NPC_data*)(npc4->customData))->message , FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 30));
                }
                else if (((NPC_data*)(npc5->customData))->talking) {
                    gf2d_font_draw_line_tag( ((NPC_data*)(npc5->customData))->message , FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 30));
                }

                gf2d_sprite_draw(mouse,vector2d(mousex,mousey),vector2d(2,2),vector3d(8,8,0),gfc_color(0.3,.9,1,0.9),(Uint32)mouseFrame);
        gf3d_vgraphics_render_end();

        if (gfc_input_command_down("exit"))done = 1; // exit condition
    }    
    
    world_delete(w);
    
    vkDeviceWaitIdle(gf3d_vgraphics_get_default_logical_device());    
    //cleanup
    entity_system_close();
    slog("gf3d program end");
    slog_sync();
    return 0;
}

/*eol@eof*/
