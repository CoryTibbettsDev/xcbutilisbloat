#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#include <xcb/xcb.h>

#include <fontconfig/fontconfig.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "xcbutilisbloat.h"

xcb_connection_t *connection;

int main()
{
	xcb_connection_t *connection;
	int screen_number;
	xcb_void_cookie_t cookie;
	uint16_t width = 400;
	uint16_t height = 300;

	/* Open the connection to the X server */
	connection = xcb_connect(NULL, &screen_number);
	if (xcb_connection_has_error(connection)) {
		fprintf(stderr, "Couldn't connect to Xserver\n");
		exit(EXIT_FAILURE);
	}
	/* Get current screen */
	const xcb_setup_t *setup = xcb_get_setup(connection);
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
	/* Find screen at index number of iterator */
	for (int i = 0; i < screen_number; i++) {
		xcb_screen_next(&iter);
	}
	xcb_screen_t *screen = iter.data;
	if (!screen) {
		fprintf(stderr, "Could not get screen\n");
		xcb_disconnect(connection);
		return -1;
	}

	/* Set window masks (properties of the window ) and their values */
	uint32_t mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
	uint32_t value[2];
	value[0] = screen->black_pixel;
	value[1] = XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_KEY_PRESS;

	xcb_window_t window = xcb_generate_id(connection);
	/* Set window masks (properties of the window ) and their values */
	cookie = xcb_create_window_checked(
			connection, /* Connection */
			XCB_COPY_FROM_PARENT, /* depth */
			window, /* window Id */
			screen->root, /* parent window */
			0, 0, /* x, y */
			width, height, /* width, height */
			5, /* border_width */
			XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class */
			screen->root_visual, /* visual */
			mask, value); /* properties of window and their values */
	xuib_test_void_cookie(connection, cookie, "Could not create window");

	/* Begin Testing */
	const char *name = "monospace:size=18";
	bool status;
	status = xuib_font_init();
	if (!status)
		return -1;

	FT_Face *face;
	face = xuib_load_font(name);

	cookie = xcb_map_window(connection, window);
	xuib_test_void_cookie(connection, cookie, "Could not map window");

	xcb_flush(connection);

	xcb_generic_event_t *ev;
	xcb_key_press_event_t *kr;
	while (( ev = xcb_wait_for_event(connection)) ) {
		xcb_generic_error_t *err = (xcb_generic_error_t *)ev;
		switch (ev->response_type & ~0x80) {
			case XCB_EXPOSE:
				xuib_draw_text(
						connection,
						screen,
						window,
						face,
						0, 0,
						width, height,
						"hello");
				break;
			case XCB_KEY_PRESS:
				kr = (xcb_key_press_event_t *)ev;
				switch (kr->detail) {
					case 9: /* ESC */
					case 24: /* Q */
						xcb_disconnect(connection);
					}
				break;
			case 0:
				printf("Received X11 error %d\n", err->error_code);
				break;
			default:
				printf("default idk\n");
		}
		free(ev);
	}
	xuib_font_done();
}
