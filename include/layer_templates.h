#pragma once

#include "layer.h"
#include "tiled.h"
#include "size.h"

#ifdef __cplusplus
extern "C" {
#endif

Layer layer_init_tiled(MemZone *memory_pool, sprite_t *sprite, const char *map_path, Size map_size,
					   Size tile_size, layer_priority_t render_priority);

fnRenderLayer layer_tiled_render(Layer *instance);

fnRenderLayer layer_image_render(Layer *instance);

#ifdef __cplusplus
}
#endif