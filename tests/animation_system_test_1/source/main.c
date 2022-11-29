#include <libdragon.h>
#include "include/memory_alloc.h"
#include "include/mem_pool.h"
#include "include/rect.h"
#include "include/size.h"
#include "include/animated_sprite.h"

static sprite_t *spritesheet;
static sprite_t *alucard;

int main(){
    display_init(RESOLUTION_256x240, DEPTH_16_BPP, 3, GAMMA_CORRECT_DITHER, ANTIALIAS_RESAMPLE);

    dfs_init(DFS_DEFAULT_LOCATION);

    

    debug_init_usblog();

    rdp_init();
    controller_init();

    const size_t zone_size = 8*32768;

    MemZone zone;

    mem_zone_init(&zone, zone_size);
    
    spritesheet = sprite_load("rom:/big_sprite.sprite");
    alucard = sprite_load("rom:/alucard_rgb.sprite");

    float durations[4] = {0.1f, 0.2f, 0.2f, 0.1f};
    float durations2[16] = {1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f};

    Animation *test_anim = animation_init(&zone, spritesheet, new_position(0.f,0.f), new_size(100.f,100.f), new_position(0, 100), 4, durations);
    Animation *alucard_anim = animation_init(&zone, alucard, new_position(0.f,0.f), new_size(42, 49), new_position(0,0), 16, durations2);

    AnimatedSprite *test_sprite = animated_sprite_init(&zone, alucard_anim, 0, 15, 50.f);

    test_sprite->anim_speed += 1;

    Rect screen_rect = {
        .pos.x = 0,
        .pos.y = 0,
        .size.width = 256,
        .size.height = 240
    };

    color_t bk_color = {
        .r = 0,
        .g = 0,
        .b = 0,
        .a = 255
    };
    
    int toggle = 0;

    debugf("hello world\n");

    rdpq_mode_antialias(false);
    while(1){
        surface_t *disp = display_lock();
        if(!disp) continue;

        controller_scan();

        struct controller_data a = get_keys_down();

        rdp_attach(disp);

        rdpq_set_mode_fill(bk_color);
        rdpq_fill_rectangle(0,0,256,240);
        
        animated_sprite_tick(test_sprite, 1.5f);
        animated_sprite_draw(test_sprite, new_position(50.f,50.f), screen_rect);
        animated_sprite_draw(test_sprite, new_position(100,100), screen_rect);
        animated_sprite_draw(test_sprite, new_position(150,150), screen_rect);
        
        if(a.c[0].A){
            if(toggle) animated_sprite_set_animation(test_sprite, alucard_anim);
            else animated_sprite_set_animation(test_sprite, test_anim);

            toggle += 1;
            toggle %= 2;
        }
        
        rdp_detach_show(disp);
    }

    return 0;
}