#include "../include/animated_sprite.h"

#include <math.h>


Animation *animation_init(MemZone *memory_pool, sprite_t *sprite, Position top_left, Size size, Position render_offset, int num_frames, float *frame_durations){
	Animation *anim = mem_zone_alloc(memory_pool, sizeof(Animation));
	debugf("successfully allocated anim ");
	anim->frame_durations = mem_zone_alloc(memory_pool, sizeof(float) * num_frames);
	anim->anim_sprite = sprite;
	debugf("successfully grabbed surface pixels ");

	for(int i = 0; i < num_frames; ++i) anim->frame_durations[i] = frame_durations[i];
	
	anim->top_left = top_left;
	anim->frame_size = size;
	anim->num_frames = num_frames;
	anim->render_offset = render_offset;

	return anim;
}

AnimatedSprite *animated_sprite_init(MemZone *memory_pool, Animation *anim_init, size_t offset_start, size_t offset_end,
									 float anim_speed) {
	AnimatedSprite *anim = mem_zone_alloc(memory_pool, sizeof(AnimatedSprite));
	debugf("successfully allocated animated sprite ");
	
	anim->current_anim = anim_init;
	anim->offset_start = offset_start;
	anim->offset_end = offset_end;
	anim->anim_speed = anim_speed;

	anim->_current_offset = offset_start;
	anim->_last_tick = get_ticks_ms();
	anim->_ticks = 0;

	return anim;
}

void animated_sprite_pause(AnimatedSprite *anim){
	anim->is_paused = true;
}

void animated_sprite_resume(AnimatedSprite *anim){
	anim->is_paused = false;
	anim->_last_tick = get_ticks_ms();
}

void animated_sprite_set_animation(AnimatedSprite *anim, Animation *animation){
	anim->current_anim = animation;
	anim->_last_tick = get_ticks_ms();
}

void animated_sprite_tick(AnimatedSprite *anim, float anim_rate) {

	if(anim->is_paused) return;

	if(anim->_current_offset > anim->current_anim->num_frames - 1) anim->_current_offset = anim->current_anim->num_frames - 1;

	int max_offset = (anim->offset_end > anim->current_anim->num_frames - 1) ? anim->current_anim->num_frames - 1 : anim->offset_end;
	const size_t total_frames = max_offset - anim->offset_start + 1;

	unsigned long current_tick = get_ticks_ms();
	unsigned long diff = current_tick - anim->_last_tick;
	anim->_last_tick = current_tick;

	anim->_ticks += diff * anim_rate * anim->current_anim->frame_durations[anim->_current_offset];
	anim->_current_offset = (size_t)(anim->_ticks / anim->anim_speed) % total_frames;
	anim->_current_offset += anim->offset_start;
}

void animated_sprite_draw(AnimatedSprite *anim, Position pos, Rect screen_rect) {
	if (is_intersecting(new_rect(pos, anim->current_anim->frame_size), screen_rect)) { // determine if the sprite would be visible on screen before drawing

		// determine how big a single TMEM load can be for this animation based on its format
		tex_format_t fmt = FMT_RGBA16;

		size_t max_texels;

		surface_t surf = sprite_get_pixels((*anim).current_anim->anim_sprite);

		switch(fmt){
			case FMT_RGBA32:
				max_texels = 4096 / 4;
				break;
			case FMT_RGBA16:
				max_texels = 4096 / 2;
				break;
				
			case FMT_CI8:
				max_texels = 4096 / 2;
				break;
			case FMT_CI4:
				max_texels = 4096;
				break;
				
			default:
				return;
		}

		rdpq_set_mode_copy(true);

		// if the animation frame doesn't fit completely within TMEM, draw it in slices, otherwise do it in one go
		if(anim->current_anim->frame_size.width * anim->current_anim->frame_size.height > max_texels){

			/*const int tex_load_width = (anim->current_anim->frame_size.height >= anim->current_anim->frame_size.width) ? anim->current_anim->frame_size.width : max_texels / anim->current_anim->frame_size.height;
			const int tex_load_height = max_texels / tex_load_width;*/

			const int tex_load_width = 40;
			const int tex_load_height = 40;
			
			const int num_loads_x = ceilf((float)anim->current_anim->frame_size.width / tex_load_width);
			const int num_loads_y = ceilf((float)anim->current_anim->frame_size.height / tex_load_height);

			int s_0;
			int t_0;
			int s_1;
			int t_1;

			int x_0;
			int y_0;
			int x_1;
			int y_1;

			for(int tex_load_x = 0; tex_load_x < num_loads_x; ++tex_load_x){
				for(int tex_load_y = 0; tex_load_y < num_loads_y; ++tex_load_y){

					s_0 = anim->current_anim->top_left.x + anim->_current_offset * anim->current_anim->frame_size.width + tex_load_width * tex_load_x;
					t_0 = anim->current_anim->top_left.y + tex_load_height * tex_load_y;
					s_1 = s_0 + tex_load_width;
					t_1 = t_0 + tex_load_height;

					x_0 = pos.x + tex_load_x * tex_load_width;
					y_0 = pos.y + tex_load_y * tex_load_height;
					x_1 = x_0 + tex_load_width;
					y_1 = y_0 + tex_load_height;

					if(s_1 >= anim->current_anim->top_left.x + anim->_current_offset * anim->current_anim->frame_size.width + anim->current_anim->frame_size.width)
						s_1 = anim->current_anim->top_left.x + anim->_current_offset * anim->current_anim->frame_size.width + anim->current_anim->frame_size.width - 1;
					if(t_1 >= anim->current_anim->top_left.y + anim->current_anim->frame_size.height)
						t_1 = anim->current_anim->top_left.y + anim->current_anim->frame_size.height - 1;
					
					if(x_1 >= pos.x + anim->current_anim->frame_size.width)
						x_1 = pos.x + anim->current_anim->frame_size.width - 1;
					if(y_1 >= pos.y + anim->current_anim->frame_size.height)
						y_1 = pos.y + anim->current_anim->frame_size.height - 1;

					rdpq_tex_load_sub(TILE0, &surf, 0, s_0, t_0, s_1, t_1);

					rdpq_texture_rectangle(TILE0, x_0, y_0, x_1, y_1, s_0, t_0, 1.f, 1.f);

				}
			}

		}else{

			int s_0 = anim->current_anim->top_left.x + anim->_current_offset * anim->current_anim->frame_size.width;
			int t_0 = anim->current_anim->top_left.y;
			int s_1 = s_0 + anim->current_anim->frame_size.width - 1;
			int t_1 = t_0 + anim->current_anim->frame_size.height - 1;
			
			int x_0 = pos.x + anim->current_anim->render_offset.x;
			int y_0 = pos.y + anim->current_anim->render_offset.y;
			int x_1 = x_0 + anim->current_anim->frame_size.width - 1;
			int y_1 = y_0 + anim->current_anim->frame_size.height - 1;

			rdpq_tex_load_sub(TILE0, &surf, 0, s_0, t_0, s_1, t_1);
			// debugf("successfully accessed surface\n");
			rdpq_texture_rectangle(TILE0, x_0, y_0, x_1, y_1, s_0, t_0, 1.f, 1.f);
		}
	}
}