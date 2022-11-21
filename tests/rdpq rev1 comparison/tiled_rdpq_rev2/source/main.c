#include <libdragon.h>
#include "include/tiled.h"
#include "include/mem_pool.h"

static sprite_t *tiles_sprite;

int main()
{   
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    dfs_init(DFS_DEFAULT_LOCATION);

    const size_t zone_size = 32768;

    MemZone zone;

    mem_zone_init(&zone, zone_size);

    


    tiles_sprite = sprite_load("rom:/tile.sprite");

    Tiled *tilemap = tiled_init(&zone, tiles_sprite, "/maps/test_map.csv", new_size(20, 15), new_size(16,16));

    rdp_init();
    controller_init();

    // rdpq_debug_start();
    
    Rect screen_rect = {
        .pos = new_position(0.f,0.f),
        .size = new_size(320,240)
    };

    Position view_position = {
        .x = 0.f,
        .y = 0.f
    };
    
    while(1)
    {
        surface_t *disp = display_lock();
        if(!disp) continue;

        rdp_attach(disp);

        tiled_render_rdp(tilemap, screen_rect, view_position);

        rdp_detach_show(disp);
    }

    return 0;
}