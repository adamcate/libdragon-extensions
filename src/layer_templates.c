#include "../include/layer_templates.h"
#include "../include/tiled.h"

Layer layer_init_tiled(MemZone *memory_pool, sprite_t *sprite, const char *map_path, Size map_size,
					   Size tile_size, layer_priority_t render_priority, int ID, float parallax_x,
					   float parallax_y) {
	Tiled *tiled = tiled_init(memory_pool, sprite, map_path, map_size, tile_size);
	Layer layer = new_layer(&layer_render_tiled, render_priority, ID, parallax_x, parallax_y,
							tiled);

	return layer;
}

void layer_render_tiled(Layer *instance, Position view_position, Rect screen_rect) {
	screen_rect.pos.x *= instance->parallax_factor_x;
	screen_rect.pos.y *= instance->parallax_factor_y;

	tiled_render_rdp(((Tiled *)(instance->layer_data)), screen_rect, view_position);
}