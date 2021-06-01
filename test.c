#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h> /* For sleep() */

#include <xcb/xcb.h>

#include "xcbutilisbloat.h"

xcb_connection_t *connection;

int main()
{
	int screenNum;
	/* Open the connection to the X server */
	connection = xcb_connect(NULL, &screenNum);
	if (xcb_connection_has_error(connection)) {
		fprintf(stderr, "Couldn't connect to Xserver\n");
		exit(EXIT_FAILURE);
	}
	// Get current screen
	const xcb_setup_t *setup = xcb_get_setup(connection);
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);
	// Find screen at index number of iterator
	for (int i = 0; i < screenNum; i++) {
		xcb_screen_next(&iter);
	}
	xcb_screen_t *screen = iter.data;
	if (!screen) {
		fprintf(stderr, "Could not get screen\n");
		xcb_disconnect(connection);
		return -1;
	}

	const char *name = "monospace:size=14";
	bool status;
	status = xuib_font_init();
	if (!status) {
		return -1;
	}

	xuib_font_load(name);

	xcb_window_t window = xcb_generate_id(connection);
	// Set window masks (properties of the window ) and their values
	xcb_void_cookie_t windowCookie = xcb_create_window_checked(
			connection, /* Connection */
			screen->root_depth, /* depth */
			window, /* window Id */
			screen->root, /* parent window */
			100, 200, /* x, y */
			400, 300, /* width, height */
			5, /* border_width */
			XCB_WINDOW_CLASS_INPUT_OUTPUT, /* class */
			screen->root_visual, /* visual */
			0, NULL); /* properties of window and their values */
	xuib_test_void_cookie(windowCookie, connection, "Could not create window");

	// Show window, seems it needs to be done before drawing/changing other properties
	xcb_void_cookie_t mapCookie = xcb_map_window(connection, window);
	xuib_test_void_cookie(mapCookie, connection, "Could not map window");

	xcb_flush(connection);
	sleep(2);
	xuib_font_done();
}
