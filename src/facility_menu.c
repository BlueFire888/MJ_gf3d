#include <stdio.h>

#include "simple_logger.h"

#include "gfc_types.h"
#include "gfc_input.h"

#include "gf2d_windows.h"
#include "gf2d_elements.h"
#include "gf2d_element_actor.h"
#include "gf2d_element_list.h"
#include "gf2d_element_label.h"
#include "gf2d_element_button.h"
#include "gf2d_draw.h"
#include "gf2d_mouse.h"
#include "gf2d_windows_common.h"
#include "gf2d_message_buffer.h"

#include "config_def.h"
#include "station_def.h"
#include "resources.h"
#include "player.h"
#include "planet_menu.h"
#include "station.h"
#include "station_extension_menu.h"
#include "facility_buy_menu.h"
#include "work_menu.h"
#include "facility_menu.h"

typedef struct
{
    StationFacility *facility;
    int lastUpdated;
}FacilityMenuData;

void facility_menu_set_list(Window *win);

int facility_menu_free(Window *win)
{
    FacilityMenuData *data;
    if ((!win)||(!win->data))return 0;
    data = win->data;
    gf2d_window_close_child(win,win->child);
    gf2d_window_close_child(win->parent,win);
    free(data);
    return 0;
}

int facility_menu_draw(Window *win)
{
    if ((!win)||(!win->data))return 0;
    return 0;
}

void facility_menu_refresh_view(Window *win)
{
    SJson *def;
    TextLine buffer,buffer2;
    int workTime = 0;
    const char *str,*name;
    Element *e;
    Element *cost_list;
    List *resources;
    FacilityMenuData *data;
    if ((!win)||(!win->data))return;
    data = win->data;
    
    if (!data->facility)
    {
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"item_name"),"Empty Slot");
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"item_description"),"This slot is free for new facility installation");
        gf2d_element_actor_set_actor(gf2d_window_get_element_by_name(win,"item_picture"),NULL);
        gf2d_element_list_free_items(gf2d_window_get_element_by_name(win,"features"));
        gf2d_element_list_free_items(gf2d_window_get_element_by_name(win,"upkeep"));
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"productivity"),"Productivity: 0");
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"staff"),"Staff: 0 / 0");
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"energy"),"Energy Use: 0");
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"active"),"Active: No");
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"mission"),"Action: ---");
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"damage"),"Damage: ---");
        gf2d_element_set_color(gf2d_window_get_element_by_name(win,"damage"),GFC_COLOR_WHITE);
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"storage"),"Storage Capacity: 0");
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"housing"),"Housing: 0");
        gf2d_element_set_hidden(gf2d_window_get_element_by_name(win,"sell"),1);
        gf2d_element_set_hidden(gf2d_window_get_element_by_name(win,"buy"),0);
        return;
    }
    station_facility_check(data->facility);
    gf2d_element_set_hidden(gf2d_window_get_element_by_name(win,"sell"),0);
    planet_menu_set_camera_at_site(win->parent,data->facility->position);
    if ((!data->facility->inactive)&&(!data->facility->disabled))
    {
        gf2d_element_set_color(gf2d_window_get_element_by_name(win,"active"),GFC_COLOR_WHITE);
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"active"),"Active: Yes");
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"disable_label"),"Disable");
    }
    else
    {
        gf2d_element_set_color(gf2d_window_get_element_by_name(win,"active"),GFC_COLOR_RED);
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"active"),"Active: No");
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"disable_label"),"Enable");
    }
    
    name = station_facility_get_display_name(data->facility->name);
    gfc_line_sprintf(buffer,"Staff: %i / %i",data->facility->staffAssigned,data->facility->staffPositions);
    if (data->facility->staffAssigned < data->facility->staffRequired)
    {
        gf2d_element_set_color(gf2d_window_get_element_by_name(win,"staff"),GFC_COLOR_RED);
    }
    else
    {
        gf2d_element_set_color(gf2d_window_get_element_by_name(win,"staff"),GFC_COLOR_WHITE);
    }
    gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"staff"),buffer);

    if (data->facility->mission)
    {
        gfc_line_sprintf(buffer,"Action: %s",data->facility->mission->title);
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"mission"),buffer);
    }
    else if ((!data->facility->inactive))
    {
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"mission"),"Action: Working");        
    }
    else
    {
        gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"mission"),"Action: ---");        
    }
    if (data->facility->damage >= 0)
    {
        gfc_line_sprintf(buffer,"Damage: %.0f%%",data->facility->damage * 100);
    }
    else
    {
        if (data->facility->mission)
        {
            gfc_line_sprintf(buffer,"Construction Until Day %i",data->facility->mission->dayFinished);
        }
        else
        {
            gfc_line_sprintf(buffer,"Under Construction");
        }
    }
    gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"damage"),buffer);
    if (data->facility->damage > 0)
    {
        gf2d_element_set_color(gf2d_window_get_element_by_name(win,"damage"),GFC_COLOR_RED);
    }
    else
    {
        gf2d_element_set_color(gf2d_window_get_element_by_name(win,"damage"),GFC_COLOR_WHITE);
    }
    gfc_line_sprintf(buffer,"productivity: %i %%",(int)(data->facility->productivity * 100));
    gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"productivity"),buffer);
    
    if (data->facility->energyOutput > 0 )
    {
        if (data->facility->productivity < 1)
        {
            gfc_line_sprintf(buffer,"Energy Ouput: %i",(int)(data->facility->energyOutput*data->facility->productivity));
            gf2d_element_set_color(gf2d_window_get_element_by_name(win,"energy"),GFC_COLOR_RED);
        }
        else
        {
            gfc_line_sprintf(buffer,"Energy Ouput: %i",(int)(data->facility->energyOutput));
            gf2d_element_set_color(gf2d_window_get_element_by_name(win,"energy"),GFC_COLOR_WHITE);
        }
    }
    else if (data->facility->energyDraw > 0 )
        gfc_line_sprintf(buffer,"Energy Draw: %i",data->facility->energyDraw);
    else
        gfc_line_sprintf(buffer,"Energy Use: 0");
    gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"energy"),buffer);
    
    gfc_line_sprintf(buffer,"%s %i",name,data->facility->id);
    gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"item_name"),buffer);
    def = config_def_get_by_parameter("facilities","displayName",name);
    if (!def)return;
    str = sj_object_get_value_as_string(def,"description");
    gf2d_element_label_set_text(gf2d_window_get_element_by_name(win,"item_description"),str);
    str = sj_object_get_value_as_string(def,"icon");
    if (str)
    {
        gf2d_element_actor_set_actor(gf2d_window_get_element_by_name(win,"item_picture"),str);
    }
    //upkeep
    resources = station_facility_get_resource_cost(sj_object_get_value_as_string(def,"name"),"upkeep");
    e = gf2d_window_get_element_by_name(win,"upkeep");
    gf2d_element_list_free_items(e);
    if (resources)
    {
        cost_list = resource_list_element_new(win,"upkeep_list", vector2d(0,0),resources,NULL,NULL);
        gf2d_element_list_add_item(e,cost_list);
        resources_list_free(resources);
    }
    //features
    resources = station_facility_get_resource_cost(sj_object_get_value_as_string(def,"name"),"produces");
    e = gf2d_window_get_element_by_name(win,"features_list");
    gf2d_element_list_free_items(e);
    if (resources)
    {
        cost_list = resource_list_element_new(win,"produces_list", vector2d(0,0),resources,NULL,NULL);        
        gf2d_element_list_add_item(e,cost_list);
        resources_list_free(resources);
        gfc_line_sprintf(buffer2,"Next Production Day: ---");
        if ((!data->facility->inactive)&&(!data->facility->disabled))
        {
            workTime = station_facility_get_work_time(data->facility->name);
            get_date_of(buffer2,data->facility->lastProduction + workTime);
            gfc_line_sprintf(buffer,"Next Production on: %s",buffer2);
        }
        gf2d_element_list_add_item(e,gf2d_label_new_simple(win,0,buffer2,FT_H6,GFC_COLOR_CYAN));
    }
    if (data->facility->storage > 0)
    {
        gfc_line_sprintf(buffer,"Storage Capacity: %i",(int)data->facility->storage);
        gf2d_element_list_add_item(e,gf2d_label_new_simple(win,0,buffer,FT_H6,GFC_COLOR_WHITE));
    }
    if (data->facility->housing > 0)
    {
        gfc_line_sprintf(buffer,"Housing: %i",(int)data->facility->housing);
        gf2d_element_list_add_item(e,gf2d_label_new_simple(win,0,buffer,FT_H6,GFC_COLOR_WHITE));
    }
}

void facility_menu_yes(void *Data)
{
    Window *win;
    TextLine buffer;
    FacilityMenuData *data;
    win = Data;
    if (!win)return;
    win->child = NULL;
    data = win->data;
    if (!data)return;
    
    gfc_line_sprintf(buffer,"%i",data->facility->id);
    data->facility->mission = mission_begin(
        "Facility Removal",
        NULL,
        "removal",
        "facility",
        station_facility_get_display_name(data->facility->name),
        data->facility->id,
        player_get_day(),
        station_facility_get_build_time(data->facility->name)/2,
        0);//TODO make this staffed

    facility_menu_refresh_view(win);
}

void facility_menu_no(void *Data)
{
    Window *win;
    win = Data;
    if (!win)return;
    win->child = NULL;
}

int facility_menu_update(Window *win,List *updateList)
{
    int i,count;
    Element *e;
    StationData *station;
    FacilityMenuData* data;
    PlayerData *player;
    if ((!win)||(!win->data))return 0;
    if (!updateList)return 0;
    data = (FacilityMenuData*)win->data;
    if (!data)return 0;
    player = player_get_data();
    count = gfc_list_get_count(updateList);
    for (i = 0; i < count; i++)
    {
        e = gfc_list_get_nth(updateList,i);
        if (!e)continue;
        if (strcmp(e->name,"close")==0)
        {
            gf2d_window_free(win);
            return 1;
        }
        if (strcmp(e->name,"staff_assign")==0)
        {
            if (!data->facility)return 1;//nothing selected
            if (data->facility->damage < 0)
            {
                message_new("Cannot change staff assignments while under construction");
                return 1;
            }
            if (data->facility->staffAssigned < data->facility->staffPositions)
            {
                if (player->staff <= 0)
                {
                    message_new("Cannot assign any more staff.  Please hire more.");
                    return 1;
                }
                if (station_facility_change_staff(data->facility,1) == 0)
                {
                    player->staff--;
                    facility_menu_refresh_view(win);
                }
            }
            return 1;
        }
        if (strcmp(e->name,"staff_remove")==0)
        {
            if (!data->facility)return 1;//nothing selected
            if (data->facility->damage < 0)
            {
                message_new("Cannot change staff assignments while under construction");
                return 1;
            }
            if (data->facility->staffAssigned > 0)
            {
                if (station_facility_change_staff(data->facility,-1) == 0)
                {
                    player->staff++;
                    station_facility_check(data->facility);
                    facility_menu_refresh_view(win);
                }
            }
            return 1;
        }
        if (strcmp(e->name,"repair")==0)
        {
            if (!data->facility)return 1;
            if (data->facility->damage <= 0)
            {
                message_printf("Facility is not damaged");
                return 1;
            }
            if (data->facility->mission)
            {
                message_printf("Facility cannot be repaired while another mission is in progress");
                return 1;
            }
            if (win->child)return 1;
            win->child = work_menu(win,NULL,NULL,data->facility,"repair",NULL,vector2d(0,0),NULL,NULL);
            return 1;
        }
        if (strcmp(e->name,"sell")==0)
        {
            if (data->facility->mission)
            {
                message_printf("Facility cannot be removed while another mission is in progress");
                return 1;
            }
            if (!data->facility)return 1;
            if (win->child)return 1;
            win->child = work_menu(win,NULL,NULL,data->facility,"remove",NULL,vector2d(0,0),(gfc_work_func*)gf2d_window_free,win);
            return 1;
        }

        if (strcmp(e->name,"disable")==0)
        {
            if (!data->facility)return 1;//TODO: move this to station_facility.c
            if (data->facility->disabled)
            {
                if (strcmp(data->facility->name,"survey_site")==0)
                {
                    if (planet_site_surveyed(player_get_planet(),data->facility->position))
                    {
                        message_new("Survey is already complete!");
                        return 1;
                    }
                }
                data->facility->disabled = 0;
                station_facility_check(data->facility);
                if (data->facility->inactive)
                {
                    message_new("Cannot Enable Facility");
                    data->facility->disabled = 1;
                    return 1;
                }
                if (data->facility->energyDraw)
                {
                    station = player_get_station_data();
                    if (!station)return 1;//wtf
                    if (data->facility->energyDraw > station->energySupply)
                    {
                        message_printf("Facility %s cannot be enabled, not enough energy to power it.",data->facility->displayName);
                        data->facility->disabled = 1;
                        return 1;
                    }
                }
                data->facility->lastProduction = player_get_day();//starting from NOW
            }
            else
            {
                data->facility->disabled = 1;
                if (data->facility->mission)
                {
                    mission_cancel(data->facility->mission);
                }
            }
            facility_menu_refresh_view(win);
            return 1;
        }
    }
    if (gfc_input_command_released("cancel"))
    {
        gf2d_window_free(win);
        return 1;
    }
    return 0;
}

Window *facility_menu(Window *parent, StationFacility *facility)
{
    Window *win;
    FacilityMenuData* data;
    win = gf2d_window_load("menus/facility.menu");
    if (!win)
    {
        slog("failed to load facility menu");
        return NULL;
    }
    win->update = facility_menu_update;
    win->free_data = facility_menu_free;
    win->draw = facility_menu_draw;
    win->refresh = facility_menu_refresh_view;
    data = (FacilityMenuData*)gfc_allocate_array(sizeof(FacilityMenuData),1);
    win->data = data;
    win->parent = parent;
    data->facility = facility;
    facility_menu_refresh_view(win);
    message_buffer_bubble();
    return win;
}


/*eol@eof*/
