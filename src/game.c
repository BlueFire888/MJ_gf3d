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

extern int __DEBUG;

int main(int argc,char *argv[])
{
    int done = 0;
    int a;
    
    Sprite *mouse = NULL;
    int mousex,mousey;
    //Uint32 then;
    float mouseFrame = 0;
    World *w;
    Entity *npc,*player,*npc2,*npc3,*npc4,*npc5;
    Particle particle[100];
    Matrix4 skyMat;
    Model *sky;
    char* inv_g[128], *p_health[4];
    Player_inven p_inven;

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
    
    
    npc = npc_new(vector3d(0,0,0));
    Sphere aggro_r = gfc_sphere(npc->position.x, npc->position.y, npc->position.z, 50);
    NPC_data c_data = { 0 };
    c_data.range = aggro_r;
    c_data.message = "Mission get: finish the game :(";
    c_data.talking = 0;
    c_data.type = NPC_dude;
    npc->customData = &c_data;

    npc2 = npc_new(vector3d(200, 0, 0));
    NPC_data c_data2 = { 0 };
    c_data2.range = gfc_sphere(npc2->position.x, npc2->position.y, npc2->position.z, 50);;
    c_data2.message = "Gold get!!!!";
    c_data2.talking = 0;
    c_data2.type = NPC_obj;
    c_data2.gold_c = 50;
    npc2->customData = &c_data2;

    npc3 = npc_new(vector3d(-200, 0, 0));
    NPC_data c_data3 = { 0 };
    c_data3.range = gfc_sphere(npc3->position.x, npc3->position.y, npc3->position.z, 50);;
    c_data3.message = "Obtained cool sword";
    c_data3.talking = 0;
    c_data3.type = NPC_chest;
    c_data3.item_name = "Cool Sword";
    npc3->customData = &c_data3;

    npc4 = npc_new(vector3d(-200, -200, 0));
    NPC_data c_data4 = { 0 };
    c_data4.range = gfc_sphere(npc4->position.x, npc4->position.y, npc4->position.z, 50);;
    c_data4.message = "Healed!";
    c_data4.talking = 0;
    c_data4.type = NPC_heal;
    c_data4.heal_ammount = 50;
    npc4->customData = &c_data4;

    npc5 = npc_new(vector3d(-200, -400, 0));
    NPC_data c_data5 = { 0 };
    c_data5.range = gfc_sphere(npc5->position.x, npc5->position.y, npc5->position.z, 50);;
    c_data5.message = "Zoom!!!";
    c_data5.talking = 0;
    c_data5.type = NPC_boost;
    npc5->customData = &c_data5;

    if (npc)npc->selected = 1;
    w = world_load("config/testworld.json");
    
    SDL_SetRelativeMouseMode(SDL_TRUE);
    slog_sync();
    gf3d_camera_set_scale(vector3d(1,1,1));
    player = player_new(vector3d(-50,0,0));
    Player_data p_data = { 0 };
    p_inven = p_data.p_inven;
    p_data.gold = 0;
    p_inven.inven_max = 64;
    p_inven.inven_count = 0;
    p_inven.inven = (Inven_item*) gfc_allocate_array(sizeof(Inven_item),64);

    if (!p_inven.inven) {
        slog("NO INVENTORY FOR YOU");
        return;
    }
    p_data.p_inven = p_inven;
    player->customData = &p_data;
    
    for (a = 0; a < 100; a++)
    {
        particle[a].position = vector3d(gfc_crandom() * 100,gfc_crandom() * 100,gfc_crandom() * 100);
        particle[a].color = gfc_color(0,0,0,1);
//        particle[a].color = gfc_color(gfc_random(),gfc_random(),gfc_random(),1);
        particle[a].size = 100 * gfc_random();
    }
    a = 0;
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

        c_data = *(NPC_data*)npc->customData;
        p_data = *(Player_data*)player->customData;
        p_inven = p_data.p_inven;
        
        sprintf(inv_g, "%d", p_data.gold);
        sprintf(p_health, "%d", player->health);

        gf3d_vgraphics_render_start();

            //3D draws
                gf3d_model_draw_sky(sky,skyMat,gfc_color(1,1,1,1));
                world_draw(w);
                entity_draw_all();
                
               // for (a = 0; a < 100; a++)
               // {
               //     //gf3d_particle_draw(&particle[a]);
               // }
            //2D draws
                //gf2d_draw_rect_filled(gfc_rect(10 ,10,1000,32),gfc_color8(128,128,128,255));
                //gf2d_font_draw_line_tag("Press ALT+F4 to exit",FT_H1,gfc_color(1,1,1,1), vector2d(10,10));
                
               // gf2d_draw_rect(gfc_rect(10 ,10,1000,32),gfc_color8(255,255,255,255));
                
                gf2d_draw_rect_filled(gfc_rect(10, 10, 500, 120), gfc_color8(128, 128, 128, 255));
                gf2d_font_draw_line_tag("Health:", FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 10));
                gf2d_font_draw_line_tag(p_health, FT_H2, gfc_color(1, 1, 1, 1), vector2d(70, 10)); 
                gf2d_font_draw_line_tag("Gold:", FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 30));
                gf2d_font_draw_line_tag(inv_g, FT_H2, gfc_color(1, 1, 1, 1), vector2d(70, 30));

                gf2d_draw_rect(gfc_rect(10, 10, 500, 120), gfc_color8(255, 255, 255, 255));
                
                if (c_data.talking) {

                    gf2d_font_draw_line_tag(c_data.message, FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 50));
                } else if (c_data2.talking) {

                    gf2d_font_draw_line_tag(c_data2.message, FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 50));
                }
                else if (c_data3.talking) {
                    gf2d_font_draw_line_tag(c_data3.message, FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 50));
                }
                else if (c_data4.talking) {
                    gf2d_font_draw_line_tag(c_data4.message, FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 50));
                }
                else if (c_data5.talking) {
                    gf2d_font_draw_line_tag(c_data5.message, FT_H2, gfc_color(1, 1, 1, 1), vector2d(10, 50));
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
