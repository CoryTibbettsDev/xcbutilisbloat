#ifndef xcbutilisbloat_h
#define xcbutilisbloat_h

#include <stdbool.h>

#include <xcb/xcb.h>

/* font.c */
typedef struct xuib_face_holder_t xuib_face_holder_t;
typedef struct xuib_patterns_holder xuib_patterns_holder;

/* util.c */
void xuib_test_void_cookie(xcb_void_cookie_t cookie, xcb_connection_t *c, char *errMessage);
/* font.c */
bool xuib_font_init(void);
void xuib_font_load(const char *name);
void xuib_font_done(void);

#endif /* xcbutilisbloat_h */
