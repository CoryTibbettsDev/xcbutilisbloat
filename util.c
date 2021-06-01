#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <xcb/xcb.h>

void
xuib_test_void_cookie(xcb_void_cookie_t cookie,
	xcb_connection_t *c,
	char *message)
{
	xcb_generic_error_t *error = xcb_request_check(c, cookie);
	if (error) {
		fprintf (stderr, "ERROR: %s: %"PRIu8"\n", message, error->error_code);
		xcb_disconnect(c);
		exit (-1);
	}
}
