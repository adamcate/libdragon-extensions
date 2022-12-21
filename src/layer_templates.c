#include "../include/layer_templates.h"
#include "tiled.h"

fnRenderLayer layer_render_tiled(Layer *instance) {
	tiled_render_rdp(((Tiled *)(instance->layer_data)), instance->screen_rect,
					 instance->view_position);
}
