#include "../include/layer.h"
#include "memory_alloc.h"

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