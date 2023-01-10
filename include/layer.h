#pragma once

#include "rect.h"
#include "size.h"
#include "position.h"
#include "position_int.h"
#include "memory_alloc.h"
#include "mem_pool.h"

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { PRIORITY_BACKGROUND, PRIORITY_MIDDLE, PRIORITY_FOREGROUND } layer_priority_t;

typedef struct Layer_s Layer;
typedef struct LayerManager_s LayerManager;

typedef void (*fnIRenderLayer)(Layer *instance, Position view_position, Rect screen_rect);

typedef enum layer_priority_t { PRIORITY_BACKGROUND, PRIORITY_MIDDLE, PRIORITY_FOREGROUND };

typedef struct Layer_s {
	float parallax_factor_x;
	float parallax_factor_y;

	fnIRenderLayer render_callback;
	layer_priority_t render_priority;
	void *layer_data;
	int ID;
} Layer;

typedef struct LayerManager_s {
	Layer *layer_array;
	bool *layer_enabled;
	int slots_max;
	int slots_occupied;
} LayerManager;

Layer new_layer(fnIRenderLayer render_callback, layer_priority_t priority, int ID,
				float parallax_factor_x, float parallax_factor_y, void *layer_data);
void layer_render(Layer *layer, Rect screen_rect, Position view_position);

LayerManager *layer_manager_init(MemZone *memory_pool, int slots_max);
void layer_manager_free(LayerManager *manager);

void layer_manager_render(LayerManager *manager, Rect screen_rect, Position view_position);
void layer_manager_add_layer(LayerManager *manager, Layer layer);
void layer_manager_set_layer(LayerManager *manager, Layer layer, int index);
void layer_manager_remove_layer(LayerManager *manager, int index);

#ifdef __cplusplus
}
#endif