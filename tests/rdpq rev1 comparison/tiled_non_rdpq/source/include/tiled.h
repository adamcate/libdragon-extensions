#pragma once

#include <libdragon.h>
#include "mem_pool.h"
#include "rect.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Struct that holds a Tiled map.
 */
typedef struct {
	/// Map data.
	char *map;
	/// Size of the map in tiles.
	Size map_size;
	/// Size of each tile.
	Size tile_size;
	/// Sprite used to render.
	sprite_t *sprite;
	/// Texture format of the sprite
	tex_format_t format;
	/// Offset to render the map.
	Position offset;
	/// Map position and size in pixels
	Rect map_rect;
} Tiled;

/**
 * @brief Allocates, loads, and initializes the Tiled map.
 *
 * @param memory_pool
 *        MemZone to use to allocate. If NULL will use 'malloc', in that case remember to call
 * 'tiled_destroy' to free the memory allocated.
 * @param sprite
 *        Sprite used to render.
 * @param map_path
 *        Path to the map file (eg.: "/maps/my_map.csv"). Has to be a CSV file.
 * @param map_size
 *        Size of the map in tiles.
 * @param tile_size
 *        Size of each tile.
 *
 * @return The new Tiled.
 */
Tiled *tiled_init(MemZone *memory_pool, sprite_t *sprite, const char *map_path, Size map_size,
				  Size tile_size);

/**
 * @brief Sets the offset for rendering this map. Useful when rendering multiple maps.
 *
 * @param tiled
 *        Tiled to set the offset.
 * @param offset
 *        Position relative to the top left of all maps.
 */
void tiled_set_render_offset(Tiled *tiled, Position offset);

/**
 * @brief Render a Tiled map using software rendering. Use this method for constant timing, or maps
 * that have lots of different tiles.
 *
 * @param disp
 *        Display Constext to render to.
 * @param tiled
 *        Tiled to render.
 * @param screen_rect
 *        Rect of the current screen. Used to cull tiles outside of the screen.
 * @param view_position
 *        Position on where to draw the map on screen. Can be used for split-screen.
 */
void tiled_render(surface_t* disp, Tiled *tiled, Rect screen_rect, Position view_position);

/**
 * @brief Render a Tiled map using hardware rendering. Use this when there's not much texture
 * swapping.
 *
 * @param tiled
 *        Tiled to render.
 * @param screen_rect
 *        Rect of the current screen. Used to cull tiles outside of the screen.
 * @param view_position
 *        Position on where to draw the map on screen. Can be used for split-screen.
 */
void tiled_render_rdp(Tiled *tiled, Rect screen_rect, Position view_position);

/**
 * @brief Render a Tiled map using hardware rendering. Use this when the tileset can fit completely
 * into TMEM.
 *
 * @param tiled
 *        Tiled to render.
 * @param screen_rect
 *        Rect of the current screen. Used to cull tiles outside of the screen.
 * @param view_position
 *        Position on where to draw the map on screen. Can be used for split-screen.
 */
void tiled_render_fast(Tiled *tiled, Rect screen_rect, Position view_position);

/**
 * @brief Render a Tiled map using hardware rendering. Use this when the tileset can fit completely
 * into TMEM and your editor tileset has mirrored data (see sample_assets folder for example)
 *
 * @param tiled
 *        Tiled to render.
 * @param screen_rect
 *        Rect of the current screen. Used to cull tiles outside of the screen.
 * @param view_position
 *        Position on where to draw the map on screen. Can be used for split-screen.
 */
void tiled_render_fast_mirror(Tiled *tiled, Rect screen_rect, Position view_position);

/**
 * @brief Destroy a Tiled created when not using a memory pool. Do not call this function if using a
 * memory pool.
 *
 * @param tiled
 *        Tiled to destroy.
 */
void tiled_destroy(Tiled *tiled);

#ifdef __cplusplus
}
#endif