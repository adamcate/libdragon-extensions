#include "../include/layer_templates.h"
#include "tiled.h"

Layer layer_init_tiled(MemZone *memory_pool, sprite_t *sprite, const char *map_path, Size map_size,
					   Size tile_size, layer_priority_t render_priority, int ID) {
	Tiled *tiled = tiled_init(memory_pool, sprite, map_path, map_size, tile_size);
	Layer layer = new_layer(&layer_render_tiled, render_priority, ID, 1.f, 1.f, tiled);

	return layer;
}

void layer_render_tiled(Layer *instance, Position view_position, Rect screen_rect) {
	tiled_render_rdp(((Tiled *)(instance->layer_data)), screen_rect, view_position);
}