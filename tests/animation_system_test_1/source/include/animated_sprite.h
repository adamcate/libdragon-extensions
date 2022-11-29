#pragma once

#include <libdragon.h>
#include "mem_pool.h"
#include "rect.h"

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Struct that holds an animation's data
 */
typedef struct{
	/// Contains the surface that points to the sprite data to be used
	sprite_t *anim_sprite;
	/// Sprite sheet coordinates of the top left of the animation
	Position top_left;
	/// The render offset of this animation
	Position render_offset;
	/// The size of a single frame
	Size frame_size;
	/// The number of frames in this animation
	uint8_t num_frames;
	/// Array holding the duration of each frame in this animation
	float *frame_durations;
} Animation;

/**
 * @brief Allocates and returns a new Animation object
 *
 * @param memory_pool
 *        MemZone to use when allocating memory.
 * @param sprite
 *        The sprite that will be used to render. Should have all the frames.
 * @param top_left
 *        The top left coordinate of the animation in the parent sprite sheet.
 * @param size
 *        Size of each frame.
 * @param num_frames
 *        The number of frames in this animation.
 * @param frame_durations
 *        An array of durations per frame, used when calling #animated_sprite_tick. This is in frames per tick (smaller numbers equal longer durations)
 * @return A pointer to the new Animation
 */
Animation *animation_init(MemZone *memory_pool, sprite_t *sprite, Position top_left, Size size, Position render_offset, int num_frames, float *frame_durations);

/**
 * @brief Struct with a sprite that can be animated.
 */
typedef struct{
	/// Pointer to the current animation
	Animation *current_anim;
	/// Numerical index of the current animation
	int anim_index;
	/// Starting offset
	int offset_start;
	/// Ending offset
	int offset_end;
	/// The current offset
	int _current_offset;
	/// Tick from last frame
	uint32_t _last_tick;
	/// Total sum of ticks
	uint32_t _ticks;
	float anim_speed;
	/// If true, update will have no effect
	bool is_paused;
	/// Flip the sprite horizontally
	bool flip_x;
	/// Flip the sprite vertically
	bool flip_y;
} AnimatedSprite;


/**
 * @brief Allocates and returns a new AnimatedSprite object.
 *
 * @param memory_pool
 *        MemZone to use when allocating memory.
 * @param sprite
 *        The sprite that will be used to render. Should have all the frames.
 * @param size
 *        Size of each frame.
 * @param render_offset
 *        Offset to be used when rendering.
 * @param offset_start
 *        Offset of the first frame.
 * @param offset_end
 *        Offset of the last frame.
 * @param anim_speed
 *        Speed of the animation in ms.
 *
 * @return A pointer to the new AnimatedSprite.
 */
AnimatedSprite *animated_sprite_init(MemZone *memory_pool, Animation *anim_init, size_t offset_start, size_t offset_end,
									 float anim_speed);

/**
 * @brief Updates the animation. Should be called on every frame.
 *
 * @param anim
 *        AnimatedSprite to tick.
 * @param anim_rate
 *        Rate that the animation should update this tick. Multiples 'anim_speed'.
 */
void animated_sprite_tick(AnimatedSprite *anim, float anim_rate);

/**
 * @brief Pause this sprite's animation. When paused, animated_sprite_tick will do nothing.
 *
 * @param anim
 *        AnimatedSprite to pause.
 */
void animated_sprite_pause(AnimatedSprite* anim);

/**
 * @brief Resume this sprite's animation if paused.
 *
 * @param anim
 *        AnimatedSprite to resume.
 */
void animated_sprite_resume(AnimatedSprite* anim);

/**
 * @brief Resume this sprite's animation if paused.
 *
 * @param anim
 *        AnimatedSprite to modify.
 * @param animation
 *        Animation to set.
 */
void animated_sprite_set_animation(AnimatedSprite *anim, Animation *animation);

/**
 * @brief Draw this AnimatedSprite. Uses faster RDP textured rectangles for rendering.
 *
 * @param anim
 *        AnimatedSprite to draw.
 * @param pos
 *        Position of the AnimatedSprite.
 * @param screen_rect
 *        Rect of the current screen. Used to check if the AnimatedSprite is on the screen.
 */
void animated_sprite_draw(AnimatedSprite *anim, Position pos, Rect screen_rect);

/**
 * @brief Draw this AnimatedSprite based on a transformation matrix. Uses 2 RDP textured triangles to simulate a transformed sprite.
 *
 * @param anim
 *        AnimatedSprite to draw.
 * @param pos
 *        Position of the AnimatedSprite.
 * @param screen_rect
 *        Rect of the current screen. Used to check if the AnimatedSprite is on the screen.
 * @param ix
 * 		  The ix component of the transformation matrix.
 * @param iy
 * 		  The iy component of the transformation matrix.
 * @param jx
 *        The jx component of the transformation matrix.
 * @param jy
 *        The jy component of the transformation matrix.
 */
// void animated_sprite_draw_transformed(AnimatedSprite *anim, Position pos, Rect screen_rect, float ix, float iy, float jx, float jy);

/**
 * @brief Draw this AnimatedSprite as rotated. Uses 2 RDP textured triangles to simulate a transformed sprite.
 *
 * @param anim
 *        AnimatedSprite to draw.
 * @param pos
 *        Position of the AnimatedSprite.
 * @param screen_rect
 *        Rect of the current screen. Used to check if the AnimatedSprite is on the screen.
 * @param angle
 *        The sprite's angle of rotation in degrees (0.f-360.f)
 */
// void animated_sprite_draw_rotate(AnimatedSprite *anim, Position pos, Rect screen_rect, float angle);

#ifdef __cplusplus
}
#endif