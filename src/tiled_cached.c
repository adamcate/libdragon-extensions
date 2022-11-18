#include "../include/tiled_cached.h"

#include <string.h>

#include "../include/csv_reader.h"

TiledCached *tiled_cached_init(MemZone *memory_pool, sprite_t *sprite, const char *map_path,
							   Size map_size, Size tile_size) {
	TiledCached *tiled_map = NULL;
	if (memory_pool)
		tiled_map = mem_zone_alloc(memory_pool, sizeof(TiledCached));
	else
		tiled_map = malloc(sizeof(TiledCached));
	tiled_map->map_size = map_size;
	tiled_map->tile_size = tile_size;
	tiled_map->sprite = sprite;
	tiled_map->format = sprite_get_format(sprite);

	// allocate map
	char *map = malloc(map_size.width * map_size.height);
	memset(map, -1, map_size.width * map_size.height);

	// load map from csv
	csv_reader_from_chars(map_path, map_size.width * map_size.height, map);

	// naive lookup
	for (size_t i = 0; i < 255; ++i) {
		size_t counter = 0;
		for (size_t y = 0; y < map_size.height; y++) {
			for (size_t x = 0; x < map_size.width; x++) {
				size_t tile = (y * (int)map_size.width) + x;
				if (map[tile] == i)
					++counter;
			}
		}
		tiled_map->tiles[i].count = counter;
		if (counter > 0) {
			tiled_map->tiles[i].position = malloc(sizeof(PositionInt) * counter);
			memset(tiled_map->tiles[i].position, 0, sizeof(PositionInt) * counter);

			tiled_map->tiles[i].position[0].x = 0;
			size_t curr = 0;
			for (int y = 0; y < map_size.height; y++) {
				for (int x = 0; x < map_size.width; x++) {
					size_t tile = (y * (int)map_size.width) + x;
					if (map[tile] == i) {
						tiled_map->tiles[i].position[curr].x = x;
						tiled_map->tiles[i].position[curr].y = y;
						++curr;
					}
				}
			}
		}
	}

	free(map);

	return tiled_map;
}

// Render a cached Tiled map
void tiled_cached_render(TiledCached *tiled, Rect screen_rect) {

	surface_t tiled_surf = sprite_get_pixels(tiled->sprite);	// The surface pointing to the pixels of the tiled sprite 

	int tex_width = tiled->sprite->width / tiled->sprite->hslices;	// The width of the slice to load
	int tex_height = tiled->sprite->height / tiled->sprite->vslices;// The height of the slice to load

	int s_0;	// leftmost s coordinate
	int t_0;	// leftmost t coordinate
	int s_1;	// rightmost s coordinate
	int t_1;	// rightmost t coordinate

	rdpq_mode_copy(true);

	for (size_t i = 0; i < 255; ++i) {
		if (tiled->tiles[i].count > 0) {
			// calculate the s t coordinates of the slice of the parent sprite to use
			s_0 = (i % tiled->sprite->hslices) * tex_width;
			t_0 = (i % tiled->sprite->hslices) * tex_height;
			s_1 = s_0 + tex_width - 1;
			t_1 = s_1 + tex_height - 1;
			// load the segment into memory
			rdpq_load_sub(TILE0, &tiled_surf, 0, s_0, t_0, s_1, t_1);

			// draw the segment at every cached coordinate instance
			for (size_t j = 0; j < tiled->tiles[i].count; ++j) {
				rdpq_texture_rectangle(TILE0, tiled->tiles[i].position[j].x * tiled->tile_size.width, 
					tiled->tiles[i].position[j].y * tiled->tile_size.height,
					tiled->tiles[i].position[j].x * tiled->tile_size.width + tiled->tile_size.width,
					tiled->tiles[i].position[j].y * tiled->tile_size.height + tiled->tile_size.height,
					s_0, t_0, 1.f, 1.f);
			}
		}
	}
}

void tiled_cached_destroy(TiledCached *tiled) {
	free(tiled);
}
