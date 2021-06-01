#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

/* Include Freetype2 they have a weird system */
#include <ft2build.h>
#include FT_FREETYPE_H

#include <fontconfig/fontconfig.h>

typedef struct xuib_face_holder {
	FT_Face *faces;
	FT_Library library;
	uint8_t length;
} xuib_face_holder;

typedef struct xuib_patterns_holder {
	FcPattern **patterns;
	uint8_t length;
} xuib_patterns_holder;

bool
xuib_font_init(void)
{
	FcBool status;

	status = FcInit();
	if (status == FcFalse) {
		fprintf(stderr, "Could not init fontconfig\n");
		return false;
	}

	return true;
}

FcPattern *
xuib_font_query(const char *name)
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

FT_Face
xuib_load_face(FcPattern *pat)
{
	FcResult result;
	FT_Library library;
	FT_Init_FreeType(&library);

	FcValue fc_file, fc_index;
	result = FcPatternGet(pat, FC_FILE, 0, &fc_file);
	if (result != FcResultMatch) {
		fprintf(stderr, "font has not file location");
	}
	result = FcPatternGet(pat, FC_INDEX, 0, &fc_index);
	if (result != FcResultMatch) {
		fprintf(stderr, "font has no index, using 0 by default");
		fc_index.type = FcTypeInteger;
		fc_index.u.i = 0;
	}

	FT_Face face;
	FT_New_Face(
			library,
			(const char *) fc_file.u.s,
			fc_index.u.i,
			&face);

	FcPatternDestroy(pat);

	return face;
}

void
xuib_font_load(const char *name)
{
	FcPattern *pat;
	FT_Face face;

	pat = xuib_font_query(name);

	face = xuib_load_face(pat);
}

void
xuib_font_done(void)
{
	FcFini();
}
