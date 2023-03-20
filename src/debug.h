#ifndef GUARD_DEBUG_H
#define GUARD_DEBUG_H

#include <ace/types.h>
#include <ace/utils/bitmap.h>

/* Types */

#define DEBUG_COLOR_IDLE 0x000
#define DEBUG_COLOR_GAME_LOGIC 0x800
#define DEBUG_COLOR_MANAGER_LOGIC 0x080

/* Globals */

/* Functions */

void debugToggle(void);

void debugSetColor(UWORD uwColor);

void debugDrawBox(tBitMap *pBuffer, tUwRect sBoxRect);

#endif // GUARD_DEBUG_H
