#include "../include/sprite_batch.h"
#include "../include/memory_alloc.h"

SpriteBatch *sprite_batch_init(MemZone *memory_pool, sprite_t *sprite, size_t qty, Size size,
							   Position render_offset) {
	SpriteBatch *batch = MEM_ALLOC(sizeof(SpriteBatch), memory_pool);
	batch->positions = MEM_ALLOC(sizeof(Position) * qty, memory_pool);
	batch->sprite = sprite;
	batch->format = sprite_get_format(sprite);
	batch->qty = qty;
	batch->size = size;
	batch->render_offset = render_offset;

	return batch;
}

void sprite_batch_draw(SpriteBatch *sprite_batch, int offset, Rect screen_rect) {

	int tex_width = sprite_batch->sprite->width / sprite_batch->sprite->hslices;	// The width of the slice to load
	int tex_height = sprite_batch->sprite->height / sprite_batch->sprite->vslices;	// The height of the slice to load

	int s_0 = (offset % sprite_batch->sprite->hslices) * tex_width;	// The leftmost s coordinate
	int t_0 = (offset / sprite_batch->sprite->hslices) * tex_height;	// The leftmost t coordinate
	int s_1 = s_0 + tex_width - 1;	// Rightmost s coordinate
	int t_1 = t_0 + tex_height - 1;	// Rightmost t coordinate

	surface_t sprite_surf = sprite_get_pixels(sprite_batch->sprite);	// Set sprite_surf to point to the pixels of the sprite

	rdpq_set_mode_copy(true);	// Switch to faster copy mode, since we aren't using mirroring

	if(sprite_batch->format == FMT_CI4 || sprite_batch->format == FMT_CI8){	// if the image is paletted, load its color palette
		rdpq_mode_tlut(TLUT_RGBA16);
		rdpq_tex_load_tlut(sprite_get_palette(sprite_batch->sprite), 0, 16);
	}

	rdpq_tex_load_sub(TILE0, &sprite_surf, 0, s_0, t_0, s_1, t_1);

	Rect rect;
	rect.size = sprite_batch->size;

	for (size_t i = 0; i < sprite_batch->qty; ++i) {
		rect.pos = sprite_batch->positions[i];
		if (is_intersecting(rect, screen_rect)) {
			rdpq_texture_rectangle(TILE0, rect.pos.x - sprite_batch->render_offset.x, 
							rect.pos.y - sprite_batch->render_offset.y,
							rect.pos.x - sprite_batch->render_offset.x + tex_width, 
							rect.pos.y - sprite_batch->render_offset.y + tex_height,s_0,t_0,1.f,1.f);
		}
	}
}

void sprite_batch_destroy(SpriteBatch *sprite_batch) {
	free(sprite_batch->positions);
	free(sprite_batch);
}
