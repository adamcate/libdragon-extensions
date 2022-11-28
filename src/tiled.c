#include "../include/tiled.h"

#include <string.h>
#include "../include/csv_reader.h"
#include "../include/memory_alloc.h"
#include <math.h>

#define FILE_BUFFER_SIZE 100

#define CHECK_BOUNDS()                                                                             \
	if (!is_intersecting(screen_rect, tiled->map_rect))                                            \
		return;

#define SET_VARS()                                                                                 \
	int initial_x = (screen_rect.pos.x - tiled->offset.x) / tiled->tile_size.width;                \
	int initial_y = (screen_rect.pos.y - tiled->offset.y) / tiled->tile_size.height;               \
	size_t final_x = ((screen_rect.pos.x + screen_rect.size.width - tiled->offset.x) /             \
					  tiled->tile_size.width) +                                                    \
					 1;                                                                            \
	size_t final_y = ((screen_rect.pos.y + screen_rect.size.height - tiled->offset.y) /            \
					  tiled->tile_size.height) +                                                   \
					 1;                                                                            \
	if (initial_x < 0)                                                                             \
		initial_x = 0;                                                                             \
	if (initial_y < 0)                                                                             \
		initial_y = 0;                                                                             \
	if (final_x > tiled->map_size.width)                                                           \
		final_x = tiled->map_size.width;                                                           \
	if (final_y > tiled->map_size.height)                                                          \
		final_y = tiled->map_size.height;

#define BEGIN_LOOP()                                                                               \
	for (size_t y = initial_y; y < final_y; y++) {                                                 \
		for (size_t x = initial_x; x < final_x; x++) {                                             \
			size_t tile = (y * (int)tiled->map_size.width) + x;                                    \
			if (tiled->map[tile] == -1)                                                            \
				continue;                                                                          \
			int screen_x = x * tiled->tile_size.width - screen_rect.pos.x + tiled->offset.x +      \
						   view_position.x;                                                        \
			int screen_y = y * tiled->tile_size.height - screen_rect.pos.y + tiled->offset.y +     \
						   view_position.y;

#define END_LOOP()                                                                                 \
	}                                                                                              \
	}

Tiled *tiled_init(MemZone *memory_pool, sprite_t *sprite, const char *map_path, Size map_size,
				  Size tile_size) {
	Tiled *tiled_map = MEM_ALLOC(sizeof(Tiled), memory_pool);
	tiled_map->map_size = map_size;
	tiled_map->tile_size = tile_size;
	tiled_map->sprite = sprite;
	tiled_map->format = sprite_get_format(sprite);
	tiled_map->offset = new_position_zero();
	tiled_map->map_rect = new_rect(tiled_map->offset, new_size(map_size.width * tile_size.width,
															   map_size.height * tile_size.height));

	// allocate map
	tiled_map->map = MEM_ALLOC(map_size.width * map_size.height, memory_pool);
	memset(tiled_map->map, -1, map_size.width * map_size.height);

	// read map from file
	csv_reader_from_chars(map_path, map_size.width * map_size.height, tiled_map->map);

	return tiled_map;
}

void tiled_set_render_offset(Tiled *tiled, Position offset) {
	tiled->offset = offset;
	tiled->map_rect.pos = offset;
}

void tiled_render(surface_t *disp, Tiled *tiled, Rect screen_rect, Position view_position) {
	CHECK_BOUNDS()

	SET_VARS()

	BEGIN_LOOP()

	graphics_draw_sprite_trans_stride(disp, screen_x, screen_y, tiled->sprite, tiled->map[tile]);

	END_LOOP()
}

void tiled_render_rdp(Tiled *tiled, Rect screen_rect, Position view_position) {


	// new implementation

	// check and set the tile index bounds of the drawn region
	int initial_x = (screen_rect.pos.x - tiled->offset.x) / tiled->tile_size.width;
	int initial_y = (screen_rect.pos.y - tiled->offset.y) / tiled->tile_size.height;  
	         
	int final_x = ((screen_rect.pos.x + screen_rect.size.width - tiled->offset.x) /             
					  tiled->tile_size.width) +                                                    
					 1;                                                                            
	int final_y = ((screen_rect.pos.y + screen_rect.size.height - tiled->offset.y) /            
					  tiled->tile_size.height) +                                                   
					 1;
                                                                            
	if (initial_x < 0)                
		initial_x = 0;          
	if (initial_y < 0)     
		initial_y = 0;    
	if (final_x > tiled->map_size.width) 
		final_x = tiled->map_size.width;
	if (final_y > tiled->map_size.height)
		final_y = tiled->map_size.height;

	int tile_width = tiled->sprite->width / tiled->sprite->hslices;		// The width of a single tile
	int tile_height = tiled->sprite->height / tiled->sprite->vslices;	// The height of a single tile

	int tile_index = 0;

	rdpq_set_mode_copy(true);
	surface_t tiled_surf = sprite_get_pixels(tiled->sprite);	// The surface pointing to the pixels of the tiled sprite 
	

	for(int tmem_load = 0; tmem_load < ceilf((float)tiled->sprite->hslices / 4); ++tmem_load)
	{
		// load in as a linear array of tiles 16x16 tiles for now, so only iterate along the x direction of tiles
		int s_0 = 16 * tmem_load;
		int t_0 = 0;
		int s_1 = 16 * tmem_load + 16;
		int t_1 = 16;

		rdpq_tex_load_sub(TILE0, &tiled_surf, 0, s_0, t_0, s_1, t_1);

		for(int tile; tile < 4; ++tile)
		{
			for (size_t y = initial_y; y < final_y; y++) 
			{                                              
				for (size_t x = initial_x; x < final_x; x++) 
				{                                             
					size_t tile = (y * (int)tiled->map_size.width) + x;                                    
					if (tiled->map[tile] == -1)                                                            
						continue;                                                                          
					int screen_x = x * tiled->tile_size.width - screen_rect.pos.x + tiled->offset.x +      
						   view_position.x;                                                        
					int screen_y = y * tiled->tile_size.height - screen_rect.pos.y + tiled->offset.y + 
						   view_position.y;

					rdpq_texture_rectangle(TILE0, screen_x, screen_y, screen_x + 16, screen_y + 16, s_0, t_0, 1.f, 1.f);
				}
			}
		}
	}

}


void tiled_render_fast(Tiled *tiled, Rect screen_rect, Position view_position) {

	CHECK_BOUNDS()

	surface_t tiled_surf = sprite_get_pixels(tiled->sprite);

	rdpq_tex_load(TILE0, &tiled_surf, 0);

	rdpq_set_mode_copy(true);

	SET_VARS()

	BEGIN_LOOP()

	int s = (tiled->map[tile] % tiled->sprite->hslices) * tiled->tile_size.width;
	int t = (tiled->map[tile] / tiled->sprite->hslices) * tiled->tile_size.height;

	rdpq_texture_rectangle(TILE0, screen_x, screen_y, screen_x + tiled->tile_size.width - 1,
				screen_y + tiled->tile_size.width - 1, s, t, 1.f, 1.f);

	END_LOOP()
}


void tiled_render_fast_mirror(Tiled *tiled, Rect screen_rect, Position view_position) {
	CHECK_BOUNDS()

	rdp_sync(SYNC_PIPE);
	rdp_load_texture(0, 0, MIRROR_XY, tiled->sprite);

	/*SET_VARS()

	BEGIN_LOOP()

	int s = (tiled->map[tile] % (tiled->sprite->hslices * 2)) * tiled->tile_size.width;
	int t = (tiled->map[tile] / (tiled->sprite->hslices * 2)) * tiled->tile_size.height;
	rdp_draw_textured_rectangle_scaled_text_coord(
		0, screen_x, screen_y, screen_x + tiled->tile_size.width - 1,
		screen_y + tiled->tile_size.height - 1, 1, 1, s, t, MIRROR_DISABLED);

	END_LOOP()*/
}

void tiled_destroy(Tiled *tiled) {
	free(tiled->map);
	free(tiled);
}
