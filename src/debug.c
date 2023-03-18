#include "debug.h"

#include <ace/types.h>
#include <ace/utils/custom.h>
#include <ace/managers/blit.h>

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

void debugDrawBox(tBitMap *pBuffer, tUwRect sBoxRect) {
	blitLine(
		pBuffer,
		sBoxRect.uwX,
		sBoxRect.uwY,
		sBoxRect.uwX + sBoxRect.uwWidth,
		sBoxRect.uwY,
		2, 0xAAAA, 0
	);

	blitLine(
		pBuffer,
		sBoxRect.uwX,
		sBoxRect.uwY,
		sBoxRect.uwX,
		sBoxRect.uwY + sBoxRect.uwHeight,
		2, 0xAAAA, 0
	);

	blitLine(
		pBuffer,
		sBoxRect.uwX + sBoxRect.uwWidth,
		sBoxRect.uwY + sBoxRect.uwHeight,
		sBoxRect.uwX + sBoxRect.uwWidth,
		sBoxRect.uwY,
		2, 0xAAAA, 0
	);

	blitLine(
		pBuffer,
		sBoxRect.uwX + sBoxRect.uwWidth,
		sBoxRect.uwY + sBoxRect.uwHeight,
		sBoxRect.uwX,
		sBoxRect.uwY + sBoxRect.uwHeight,
		2, 0xAAAA, 0
	);
}
