#include <libdragon.h>
#include "include/tiled.h"
#include "include/mem_pool.h"

const size_t zone_size = 32768;

static sprite_t *tiles_sprite;

int main()
{   
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 3, GAMMA_NONE, ANTIALIAS_RESAMPLE);
    dfs_init(DFS_DEFAULT_LOCATION);

    tiles_sprite = sprite_load("rom:/tiles.sprite");

    MemZone *memory_pool = NULL;
    mem_zone_init(memory_pool, zone_size);

    Tiled *tilemap = tiled_init(memory_pool, tiles_sprite, "/maps/test_map.csv", new_size(2, 2), new_size(16,16));

    rdp_init();
    controller_init();

    Rect screen_rect = {
        .pos = new_position(0.f,0.f),
        .size = new_size(320,240)
    };

    Position view_position = {
        .x = 160.f,
        .y = 120.f
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