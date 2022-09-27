#ifndef GUARD_VILLAGES_CURSOR_H
#define GUARD_VILLAGES_CURSOR_H

#include <ace/types.h>
#include <ace/utils/extview.h>

/**
 * Creates cursor on supplied view.
 * @param pView        Parent view.
 * @param szPath       Path to 2bpp interleaved bitmap used as cursor.
 *                     Bitmap has to have two unused lines on top & bottom
 *                     of cursor image.
 * @todo Passing hotspot coord as params.
 * @todo Support for 4bpp sprites.
 */
void cursorCreate(
	tView *pView,
	char *szPath
);

/**
 * Frees stuff allocated by copCreate().
 * This function doesn't remove created copBlock as it will be destroyed along
 * with copperlist.
 */
void cursorDestroy(void);

/**
 * Updates cursor position.
 */
void cursorUpdate(void);

#endif // GUARD_VILLAGES_CURSOR_H
