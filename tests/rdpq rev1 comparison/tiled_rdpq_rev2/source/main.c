#include <libdragon.h>
#include "include/tiled.h"
#include "include/mem_pool.h"

static sprite_t *tiles_sprite;
static sprite_t *text_sprite;


void debug_text_rdp(surface_t * disp, int num, int x, int y)
{   
    int digit_ones = num % 10;
    num /= 10;
    int digit_tens = num % 10;

    rdpq_set_mode_copy(true);
    
    surface_t text_surf = sprite_get_pixels(text_sprite);
    rdpq_tex_load_sub(TILE0, &text_surf, 0, 0, 0, 80, 16);

    rdpq_texture_rectangle(TILE0, x, y, x + 8, y + 8, digit_tens * 8, 0, 1.f, 1.f);
    x += 8;
    rdpq_texture_rectangle(TILE0, x, y, x + 8, y + 8, digit_ones * 8, 0, 1.f, 1.f);

}

int main()
{   
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    dfs_init(DFS_DEFAULT_LOCATION);

    const size_t zone_size = 32768;

    MemZone zone;

    mem_zone_init(&zone, zone_size);

    


    tiles_sprite = sprite_load("rom:/tile.sprite");
    text_sprite = sprite_load("rom:/numbers_rdp.sprite");

    Tiled *tilemap = tiled_init(&zone, tiles_sprite, "/maps/test_map.csv", new_size(50, 50), new_size(16,16));

    rdp_init();
    controller_init();

    rdpq_debug_start();

    timer_init();

    Position view_position = {
        .x = 0.f,
        .y = 0.f
    };
    
    /*color_t bk_color = {
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 255
    };*/

    int num_layers = 1;

    Rect screen_rect = {
        .pos = new_position(0.f,0.f),
        .size = new_size(320,240)
    };

    uint32_t timer_0 = 0;
    uint32_t timer_1 = 1;
    float delta_time = 0;

    int timer = 0;

    while(1)
    {
        surface_t *disp = display_lock();
        if(!disp) continue;
        timer_1 = get_ticks_ms();
        if(timer % 10 == 0){ 
            if(delta_time > 0.f){delta_time = 1000.f / (float)(timer_1 - timer_0);}
            else{delta_time = 60.f;}

            if(delta_time > 60.f) delta_time = 60;
        }

        timer_0 = get_ticks_ms();

        timer++;

        if(timer > 10) timer = 0;


        controller_scan();
           
        struct controller_data controller = get_keys_down();
        struct controller_data d_pad = get_keys_pressed();

        if(controller.c[0].A){
            num_layers++;
        }
        if(controller.c[0].B){
            num_layers--;
        }

        if(d_pad.c[0].left) screen_rect.pos.x -= 1;
        if(d_pad.c[0].right) screen_rect.pos.x += 1;
        if(d_pad.c[0].up) screen_rect.pos.y -= 1;
        if(d_pad.c[0].down) screen_rect.pos.y += 1;

        if(num_layers < 0) num_layers = 0;

        Rect screen_rect_per_layer = screen_rect;

        rdp_attach(disp);

        // rdpq_set_mode_fill(bk_color);
        // rdpq_fill_rectangle(0, 0, 320, 240);

        for(int i = 0; i < num_layers; ++i){
            tiled_render_rdp(tilemap, screen_rect_per_layer, view_position);
            screen_rect_per_layer.pos.x += 5;
            screen_rect_per_layer.pos.y += 5;
        }

        debug_text_rdp(disp, (int)delta_time, 20, 20);
        debug_text_rdp(disp, num_layers, 20, 36);

        rdp_detach_show(disp);
    }

    return 0;
}