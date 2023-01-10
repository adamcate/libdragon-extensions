#include "../include/layer.h"
#include "../include/memory_alloc.h"

Layer new_layer(fnIRenderLayer render_callback, layer_priority_t priority, int ID,
				float parallax_factor_x, float parallax_factor_y, void *layer_data) {
	Layer layer;

	layer.render_callback = &render_callback;
	layer.render_priority = priority;
	layer.ID = ID;
	layer.parallax_factor_x = parallax_factor_x;
	layer.parallax_factor_y = parallax_factor_y;
	layer.layer_data = layer_data;

	return layer;
}

void layer_render(Layer *layer, Rect screen_rect, Position view_position) {
	layer->render_callback(layer, view_position, screen_rect);
}

LayerManager *layer_manager_init(MemZone *memory_pool, int slots_max) {
	LayerManager *manager = mem_zone_alloc(memory_pool, sizeof(LayerManager));
	manager->layer_array = mem_zone_alloc(memory_pool, sizeof(Layer) * slots_max);
	manager->layer_enabled = mem_zone_alloc(memory_pool, sizeof(bool) * slots_max);

	for (int index = 0; index < slots_max; ++index)
		manager->layer_enabled[index] = false;

	manager->slots_max = slots_max;
	manager->slots_occupied = 0;

	return manager;
}

void layer_manager_render(LayerManager *manager, Rect screen_rect, Position view_position) {
	for (int index = 0; index < manager->slots_max; ++index) {
		if (manager->layer_enabled[index])

			manager->layer_array[index].render_callback(&manager->layer_array[index], view_position,
														screen_rect);
	}
}

void layer_manager_add_layer(LayerManager *manager, Layer layer) {
	if (manager->slots_occupied >= manager->slots_max)
		return;

	manager->layer_array[manager->slots_occupied++] = layer;
}