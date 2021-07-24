#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

/* Include Freetype2 they have a weird system */
#include <ft2build.h>
#include FT_FREETYPE_H

#include <fontconfig/fontconfig.h>

#include <xcb/xcb.h>
#include <xcb/render.h>

#include "xcbutilisbloat.h"

struct xuib_font_holder_t {
	FT_Face face;
};

FT_Library library;

bool
xuib_font_init(void)
{
	FcBool status;
	FT_Error error;

	status = FcInit();
	if (status == FcFalse) {
		fprintf(stderr, "Could not init fontconfig\n");
		return false;
	}

	error = FT_Init_FreeType(&library);
	if (error) {
		fprintf(stderr, "Could not init freetype2\n");
		return false;
	}

	return true;
}

static FcPattern *
font_query(const char *name)
{
	FcBool status;
	FcPattern *pat, *match_pat;
	FcResult result;

	pat = FcNameParse((FcChar8 *) name);

	if (!pat) {
		fprintf(stderr, "Invalid font name\n");
	}

	/* Fill in unspecified info */
	FcDefaultSubstitute(pat);
	status = FcConfigSubstitute(NULL, pat, FcMatchPattern);
	if (status == FcFalse) {
		fprintf(stderr, "Could not perform font config substitution\n");
		return NULL;
	}

	match_pat = FcFontMatch(NULL, pat, &result);
	FcPatternDestroy(pat);

	if (result == FcResultMatch) {
		return match_pat;
	} else if (result == FcResultNoMatch) {
		fprintf(stderr, "There was not a match\n");
	} else {
		fprintf(stderr, "The match was not good enough\n");
	}
	return NULL;
}

xuib_font_holder_t *
xuib_load_font(const char *name)
{
	FcPattern *pat;
	FcResult result;
	FcValue fc_file, fc_index;

	FT_Face face;
	FT_Error error;

	xuib_font_holder_t *holder;

	pat = font_query(name);

	result = FcPatternGet(pat, FC_FILE, 0, &fc_file);
	if (result != FcResultMatch) {
		fprintf(stderr, "font has no file location\n");
	}
	result = FcPatternGet(pat, FC_INDEX, 0, &fc_index);
	if (result != FcResultMatch) {
		fprintf(stderr, "font has no index, using 0 by default\n");
		fc_index.type = FcTypeInteger;
		fc_index.u.i = 0;
	}

	error = FT_New_Face(
			library,
			(const char *) fc_file.u.s,
			fc_index.u.i,
			&face);
	if (error == FT_Err_Unknown_File_Format) {
		fprintf(stderr, "FT_New_Face error: font format not supported\n");
	} else if (error == FT_Err_Cannot_Open_Resource) {
		fprintf(stderr, "FT_New_Face error: could not open resources\n");
	} else if (error) {
		fprintf(stderr, "FT_New_Face error\n");
	}

	error = FT_Set_Char_Size(
			face,
			0,
			18 * 64,
			90, 90);
	if (error) {
		fprintf(stderr, "FT_Set_Char_Size error\n");
	}

	holder = malloc(sizeof(xuib_font_holder_t));

	holder->face = face;

	FcPatternDestroy(pat);

	return holder;
}

xcb_render_pictforminfo_t *
find_pict_format(
		const xcb_render_query_pict_formats_reply_t *formats,
		unsigned long mask,
		const xcb_render_pictforminfo_t *ptemplate,
		int count)
{
	xcb_render_pictforminfo_iterator_t i;
	if (!formats)
		return 0;
	for (i = xcb_render_query_pict_formats_formats_iterator(formats);
			i.rem;
			xcb_render_pictforminfo_next(&i))
	{
		if (mask & XUIB_PICT_FORMAT_ID)
			if (ptemplate->id != i.data->id)
				continue;
		if (mask & XUIB_PICT_FORMAT_TYPE)
			if (ptemplate->type != i.data->type)
				continue;
		if (mask & XUIB_PICT_FORMAT_DEPTH)
			if (ptemplate->depth != i.data->depth)
				continue;
		if (mask & XUIB_PICT_FORMAT_RED)
			if (ptemplate->direct.red_shift != i.data->direct.red_shift)
				continue;
		if (mask & XUIB_PICT_FORMAT_RED_MASK)
			if (ptemplate->direct.red_mask != i.data->direct.red_mask)
				continue;
		if (mask & XUIB_PICT_FORMAT_GREEN)
			if (ptemplate->direct.green_shift != i.data->direct.green_shift)
				continue;
		if (mask & XUIB_PICT_FORMAT_GREEN_MASK)
			if (ptemplate->direct.green_mask != i.data->direct.green_mask)
				continue;
		if (mask & XUIB_PICT_FORMAT_BLUE)
			if (ptemplate->direct.blue_shift != i.data->direct.blue_shift)
				continue;
		if (mask & XUIB_PICT_FORMAT_BLUE_MASK)
			if (ptemplate->direct.blue_mask != i.data->direct.blue_mask)
				continue;
		if (mask & XUIB_PICT_FORMAT_ALPHA)
			if (ptemplate->direct.alpha_shift != i.data->direct.alpha_shift)
				continue;
		if (mask & XUIB_PICT_FORMAT_ALPHA_MASK)
			if (ptemplate->direct.alpha_mask != i.data->direct.alpha_mask)
				continue;
		if (mask & XUIB_PICT_FORMAT_COLORMAP)
			if (ptemplate->colormap != i.data->colormap)
				continue;
		if (count-- == 0)
			return i.data;
	}
	return 0;
}

xcb_render_pictforminfo_t *
get_pictforminfo(
		xcb_connection_t *c,
		xuib_pict_standard_t format)
{
	xcb_render_query_pict_formats_reply_t *formats;
	xcb_render_query_pict_formats_cookie_t formats_cookie;
	xcb_render_pictforminfo_t *pfi;

	formats_cookie = xcb_render_query_pict_formats(c);

	formats = xcb_render_query_pict_formats_reply(c, formats_cookie, 0);
	if (!formats) {
		fprintf(stderr, "Bad formats reply\n");
	}

	static const struct {
		xcb_render_pictforminfo_t temp;
		unsigned long mask;
	} standardFormats[] = {
		/* XUIB_PICT_STANDARD_ARGB_32 */
		{
			{
			0, /* id */
			XCB_RENDER_PICT_TYPE_DIRECT, /* type */
			32,	/* depth */
			{ 0 }, /* pad */
			{ /* direct */
				16,	/* direct.red */
				0xff, /* direct.red_mask */
				8, /* direct.green */
				0xff, /* direct.green_mask */
				0, /* direct.blue */
				0xff, /* direct.blue_mask */
				24, /* direct.alpha */
				0xff, /* direct.alpha_mask */
			},
			0, /* colormap */
			},
			XUIB_PICT_FORMAT_TYPE |
			XUIB_PICT_FORMAT_DEPTH |
			XUIB_PICT_FORMAT_RED |
			XUIB_PICT_FORMAT_RED_MASK |
			XUIB_PICT_FORMAT_GREEN |
			XUIB_PICT_FORMAT_GREEN_MASK |
			XUIB_PICT_FORMAT_BLUE |
			XUIB_PICT_FORMAT_BLUE_MASK |
			XUIB_PICT_FORMAT_ALPHA |
			XUIB_PICT_FORMAT_ALPHA_MASK,
		},
		/* XUIB_PICT_STANDARD_RGB_24 */
		{
			{
			0, /* id */
			XCB_RENDER_PICT_TYPE_DIRECT, /* type */
			24,	/* depth */
			{ 0 }, /* pad */
			{ /* direct */
				16, /* direct.red */
				0xff, /* direct.red_MASK */
				8, /* direct.green */
				0xff, /* direct.green_MASK */
				0, /* direct.blue */
				0xff, /* direct.blue_MASK */
				0, /* direct.alpha */
				0x00, /* direct.alpha_MASK */
			},
			0,			    /* colormap */
			},
			XUIB_PICT_FORMAT_TYPE |
			XUIB_PICT_FORMAT_DEPTH |
			XUIB_PICT_FORMAT_RED |
			XUIB_PICT_FORMAT_RED_MASK |
			XUIB_PICT_FORMAT_GREEN |
			XUIB_PICT_FORMAT_GREEN_MASK |
			XUIB_PICT_FORMAT_BLUE |
			XUIB_PICT_FORMAT_BLUE_MASK |
			XUIB_PICT_FORMAT_ALPHA_MASK,
		},
		/* XUIB_PICT_STANDARD_A_8 */
		{
			{
			0, /* id */
			XCB_RENDER_PICT_TYPE_DIRECT,    /* type */
			8, /* depth */
			{ 0 }, /* pad */
			{ /* direct */
				0, /* direct.red */
				0x00, /* direct.red_MASK */
				0, /* direct.green */
				0x00, /* direct.green_MASK */
				0, /* direct.blue */
				0x00, /* direct.blue_MASK */
				0, /* direct.alpha */
				0xff, /* direct.alpha_MASK */
			},
			0, /* colormap */
			},
			XUIB_PICT_FORMAT_TYPE |
			XUIB_PICT_FORMAT_DEPTH |
			XUIB_PICT_FORMAT_RED_MASK |
			XUIB_PICT_FORMAT_GREEN_MASK |
			XUIB_PICT_FORMAT_BLUE_MASK |
			XUIB_PICT_FORMAT_ALPHA |
			XUIB_PICT_FORMAT_ALPHA_MASK,
		},
		/* XUIB_PICT_STANDARD_A_4 */
		{
			{
			0, /* id */
			XCB_RENDER_PICT_TYPE_DIRECT, /* type */
			4, /* depth */
			{ 0 }, /* pad */
			{ /* direct */
				0, /* direct.red */
				0x00, /* direct.red_MASK */
				0, /* direct.green */
				0x00, /* direct.green_MASK */
				0, /* direct.blue */
				0x00, /* direct.blue_MASK */
				0, /* direct.alpha */
				0x0f, /* direct.alpha_MASK */
			},
			0,			    /* colormap */
			},
			XUIB_PICT_FORMAT_TYPE |
			XUIB_PICT_FORMAT_DEPTH |
			XUIB_PICT_FORMAT_RED_MASK |
			XUIB_PICT_FORMAT_GREEN_MASK |
			XUIB_PICT_FORMAT_BLUE_MASK |
			XUIB_PICT_FORMAT_ALPHA |
			XUIB_PICT_FORMAT_ALPHA_MASK,
		},
		/* XUIB_PICT_STANDARD_A_1 */
		{
			{
			0, /* id */
			XCB_RENDER_PICT_TYPE_DIRECT, /* type */
			1, /* depth */
			{ 0 }, /* pad */
			{ /* direct */
				0, /* direct.red */
				0x00, /* direct.red_MASK */
				0, /* direct.green */
				0x00, /* direct.green_MASK */
				0, /* direct.blue */
				0x00, /* direct.blue_MASK */
				0, /* direct.alpha */
				0x01, /* direct.alpha_MASK */
			},
			0, /* colormap */
			},
			XUIB_PICT_FORMAT_TYPE |
			XUIB_PICT_FORMAT_DEPTH |
			XUIB_PICT_FORMAT_RED_MASK |
			XUIB_PICT_FORMAT_GREEN_MASK |
			XUIB_PICT_FORMAT_BLUE_MASK |
			XUIB_PICT_FORMAT_ALPHA |
			XUIB_PICT_FORMAT_ALPHA_MASK,
		},
	};

	if (format < 0 || format >= sizeof(standardFormats) / sizeof(*standardFormats))
		return 0;

	pfi = find_pict_format(
			formats,
			standardFormats[format].mask,
			&standardFormats[format].temp,
			0);

	free(formats);

	return pfi;
}

xcb_render_picture_t
create_pen(xcb_connection_t *c, xcb_screen_t *s, xcb_window_t window)
{
	xcb_void_cookie_t cookie;

	xcb_render_pictforminfo_t *pfi = get_pictforminfo(c, XUIB_PICT_STANDARD_A_8);

	xcb_pixmap_t pm = xcb_generate_id(c);
	cookie = xcb_create_pixmap_checked(c, s->root_depth, pm, s->root, 1, 1);
	xuib_test_void_cookie(cookie, c, "Could not create pen pixmap");

	xcb_render_picture_t picture = xcb_generate_id(c);
	cookie = xcb_render_create_picture_checked(
			c,
			picture,
			pm,
			pfi->id,
			XCB_RENDER_CP_REPEAT,
			(uint32_t[]){ XCB_RENDER_REPEAT_NORMAL });
	xuib_test_void_cookie(cookie, c, "Could not create pen picture");

	xcb_render_color_t color = {
		.red = 0x00ff,
		.green = 0xff0f,
		.blue = 0xf0ff,
		.alpha = 0xffff
	};

	xcb_rectangle_t rect = {
		.x = 0,
		.y = 0,
		.width = 1,
		.height = 1
	};

	cookie = xcb_render_fill_rectangles_checked(
			c,
			XCB_RENDER_PICT_OP_OVER,
			picture,
			color, 1, &rect);
	xuib_test_void_cookie(cookie, c, "Could not fill pen rectangle");

	xcb_free_pixmap(c, pm);
	return picture;
}

void
xuib_draw_text(
		xcb_connection_t *c,
		xcb_screen_t *s,
		xcb_window_t window,
		xuib_font_holder_t *holder,
		uint16_t x, uint16_t y,
		uint16_t width, uint16_t height,
		char *text)
{
	xcb_void_cookie_t cookie;

	xcb_render_pictforminfo_t *pfi = get_pictforminfo(c, XUIB_PICT_STANDARD_RGB_24);

	xcb_pixmap_t pm = xcb_generate_id(c);
	cookie = xcb_create_pixmap_checked(
			c,
			s->root_depth,
			pm,
			s->root,
			width,
			height);
	xuib_test_void_cookie(cookie, c, "Could not create draw_text pixmap");

	xcb_render_picture_t picture = xcb_generate_id(c);
	cookie = xcb_render_create_picture_checked(
			c,
			picture,
			pm,
			pfi->id,
			NULL,
			NULL);
	xuib_test_void_cookie(cookie, c, "Could not create draw_text picture");

	xcb_render_color_t rect_color = {
		.red = 0x00ff,
		.green = 0xff0f,
		.blue = 0xf0ff,
		.alpha = 0xffff
	};

	xcb_rectangle_t window_rect = {
		.x = 0,
		.y = 0,
		.width = 400,
		.height = 300
	};

	cookie = xcb_render_fill_rectangles_checked(
			c,
			XCB_RENDER_PICT_OP_OVER,
			picture,
			rect_color, 1, &window_rect);
	xuib_test_void_cookie(cookie, c, "can't fill rectangles");

	xcb_gcontext_t gc = xcb_generate_id(c);
	uint32_t masks = XCB_GC_FOREGROUND | XCB_GC_GRAPHICS_EXPOSURES;
	uint32_t values[2];
	values[0] = 0xe6e3c6 | 0xff000000;
	values[1] = 0;
	cookie = xcb_create_gc(c, gc, window, masks, values);
	xuib_test_void_cookie(cookie, c, "Could not create draw_text gc");
		
	// xcb_render_color_t color = {
	// 	.red = 0x00ff,
	// 	.green = 0xff0f,
	// 	.blue = 0xf0ff,
	// 	.alpha = 0xffff
	// };
	// // create a 1x1 pixel pen (on repeat mode) of a certain color
	// xcb_render_picture_t pen = create_pen(c, color);

	// cookie = xcb_render_composite_glyphs_32_checked(
	// 		c, /* XCB Connection */
	// 		op, /* unint8_t op */
	// 		src, /* xcb_render_picture_t src */
	// 		picture, /* xcb_render_picture_t dst */
	// 		mask_format, /* xcb_render_pictformat_t mask_format */
	// 		glyphset, /* xcb_render_glyphset_t glyphset */
	// 		src_x, src_y, /* int16_t src_x, int16_t src_y */
	// 		glyphsmode_len, /* uint32_t glyphsmode_len */
	// 		glyphcmds); /* const unint8_t *glyphcmds */
	// xuib_test_void_cookie(cookie, c, "Render composite glyphs failed");

	cookie = xcb_copy_area_checked(
			c, /* xcb_connection_t */
			pm, /* The Drawable we want to paste */
			window, /* The Drawable on which we copy the previous Drawable */
			gc,   /* A Graphic Context */
			0, /* Top left x coordinate of the region to copy */
			0, /* Top left y coordinate of the region to copy */
			0, /* Top left x coordinate of the region where to copy */
			0, /* Top left y coordinate of the region where to copy */
			400, /* Width of the region to copy */
			300); /* Height of the region to copy */
	xuib_test_void_cookie(cookie, c, "Could not copy area in draw_text");

	xcb_flush(c);
	xcb_free_pixmap(c, pm);
}

void
xuib_font_done(void)
{
	FcFini();
	FT_Done_FreeType(library);
}
