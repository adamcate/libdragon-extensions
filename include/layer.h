#pragma once

#include "rect.h"
#include "size.h"
#include "position.h"
#include "position_int.h"
#include <stdbool.h>
#include "memory_alloc.h"
#include "mem_pool.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Layer_s Layer;
typedef struct LayerManager_s LayerManager;

typedef void (*fnRenderLayer)(Layer *instance);

typedef enum {
	/// Will be rendered in between layers with background and foreground priority
	PRIORITY_MIDDLE = 0,
	/// Will be rendered in the background
	PRIORITY_BACKGROUND = 1,
	/// Will be rendered in the foreground
	PRIORITY_FOREGROUND = 2
} layer_priority_t;

/**
 * @brief A basic layer object
 */
typedef struct Layer_s {
	/// The view position
	Position view_position;
	/// The rect of this screen
	Rect screen_rect;
	/// Whether or not this layer should be drawn
	bool enabled;
	/// The function to be called when rendering this layer.
	fnRenderLayer render_callback;
	/// The render priority of this layer.
	layer_priority_t priority;
	/// The data for this layer to be used by custom render callback implementation
	void *layer_data;
} Layer;

/**
 * @brief A basic layer manager
 */
typedef struct LayerManager_s {
	/// The array of layers to pull from when rendering.
	Layer *layer_array;
	/// Number of layers in this LayerManager
	int slots_max;
	/// Number of occupied layer slots
	int slots_occupied;
} LayerManager;

/**
 * @brief Initialize this Layer, allocating memory for it
 *
 * @param memory_pool The memory pool to use for this layer object
 * @param render_callback The function to be called during rendering.
 * @param update_callback The function to be called when updating properties.
 *
 * @return Layer * The Layer struct initialized
 */
Layer *layer_init(MemZone *memory_pool, fnRenderLayer render_callback, void *layer_data);

/**
 * @param layer The layer instance.
 *
 * @return void * A pointer to the data of this layer
 */
Layer *layer_get_data(Layer *layer);

/**
 * @brief Assign the render function callback of this layer
 *
 * @param layer The layer instance
 * @param render_callback The render callback to assign to this layer
 */
void layer_set_render_callback(Layer *layer, fnRenderLayer render_callback);

/**
 * @brief Call the render function of this layer
 *
 * @param layer The layer instance
 */
void layer_render(Layer *layer);

/**
 * @brief Free the memory associated with this layer. Do not use if part of a MemZone.
 *
 * @param layer The layer instance
 */
void layer_free(Layer *layer);

/**
 * @brief Initialize this Layer manager, allocating memory for it
 *
 * @param memory_pool The memory pool to use for this layer object. Use NULL to use malloc instead
 * @param num_layers The number of layers in this LayerManager
 * @param screen_rect The rectangle of this screen in pixels
 *
 * @return LayerManager * The LayerManager struct initialized
 */
LayerManager *layer_manager_init(MemZone *memory_pool, int num_layers, RectInt screen_rect);

Layer *layer_manager_add_layer(LayerManager *manager, Layer *layer);

void layer_manager_free(LayerManager *manager);

void layer_manager_render(LayerManager *manager);

#ifdef __cplusplus
}
#endif