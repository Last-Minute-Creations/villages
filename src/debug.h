#ifndef GUARD_DEBUG_H
#define GUARD_DEBUG_H

#include <ace/types.h>
#include <ace/utils/bitmap.h>

/* Types */

/* Globals */

/* Functions */

void debugToggle(void);

void debugSetColor(UWORD uwColor);

void debugDrawBox(tBitMap *pBuffer, tUwRect sBoxRect);

#endif // GUARD_DEBUG_H
