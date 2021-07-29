#ifndef xcbutilisbloat_h
#define xcbutilisbloat_h

#include <stdbool.h>

#include <xcb/xcb.h>
#include <xcb/render.h>

/* font.c */
typedef struct xuib_font_holder_t xuib_font_holder_t;

typedef enum xuib_pict_standard_t {
	XUIB_PICT_STANDARD_ARGB_32,
	XUIB_PICT_STANDARD_RGB_24,
	XUIB_PICT_STANDARD_A_8,
	XUIB_PICT_STANDARD_A_4,
	XUIB_PICT_STANDARD_A_1
} xuib_pict_standard_t;

typedef enum xuib_pict_format_t {
	XUIB_PICT_FORMAT_ID         = (1 << 0),
	XUIB_PICT_FORMAT_TYPE       = (1 << 1),
	XUIB_PICT_FORMAT_DEPTH      = (1 << 2),
	XUIB_PICT_FORMAT_RED        = (1 << 3),
	XUIB_PICT_FORMAT_RED_MASK   = (1 << 4),
	XUIB_PICT_FORMAT_GREEN      = (1 << 5),
	XUIB_PICT_FORMAT_GREEN_MASK = (1 << 6),
	XUIB_PICT_FORMAT_BLUE       = (1 << 7),
	XUIB_PICT_FORMAT_BLUE_MASK  = (1 << 8),
	XUIB_PICT_FORMAT_ALPHA      = (1 << 9),
	XUIB_PICT_FORMAT_ALPHA_MASK = (1 << 10),
	XUIB_PICT_FORMAT_COLORMAP   = (1 << 11)
} xuib_pict_format_t;

/* util.c */
void xuib_test_void_cookie(
		xcb_connection_t *c,
		xcb_void_cookie_t cookie,
		char *message);

/* font.c */
bool xuib_font_init(void);

xuib_font_holder_t *xuib_load_font(const char *name);

void
xuib_draw_text(
		xcb_connection_t *c,
		xcb_screen_t *s,
		xcb_window_t window,
		xuib_font_holder_t *holder,
		uint16_t x, uint16_t y,
		uint16_t width, uint16_t height,
		char *text);

void xuib_font_done(void);

#endif /* xcbutilisbloat_h */
