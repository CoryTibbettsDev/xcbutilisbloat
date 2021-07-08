#ifndef xcbutilisbloat_h
#define xcbutilisbloat_h

#include <stdbool.h>

#include <xcb/xcb.h>
#include <xcb/render.h>

/* font.c */
typedef struct xuib_font_holder_t xuib_font_holder_t;

/* util.c */
void xuib_test_void_cookie(
		xcb_void_cookie_t cookie,
		xcb_connection_t *c,
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
