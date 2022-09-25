#include "debug.h"

#include <ace/utils/custom.h>

/* Types */

/* Globals */

/* Statics */

UBYTE s_isDebug = 0;

/* Functions */

void debugToggle(void) {
	s_isDebug = !s_isDebug;
}

void debugSetColor(UWORD uwColor) {
	if(s_isDebug) {
		g_pCustom->color[0] = uwColor;
	}
}
