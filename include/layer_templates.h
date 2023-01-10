#pragma once

#include "layer.h"
#include "tiled.h"
#include "size.h"

#ifdef __cplusplus
extern "C" {
#endif

Layer layer_init_tiled(MemZone *memory_pool, sprite_t *sprite, const char *map_path, Size map_size,
					   Size tile_size, layer_priority_t render_priority, int ID);

void layer_render_tiled(Layer *instance, Position view_position, Rect screen_rect);

void layer_render_image(Layer *instance, Position view_position, Rect screen_rect);

#ifdef __cplusplus
}
#endif