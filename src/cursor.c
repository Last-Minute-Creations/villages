#include "cursor.h"

#include <ace/macros.h>
#include <ace/managers/mouse.h>
#include <ace/managers/log.h>
#include <ace/managers/sprite.h>
#include <ace/utils/bitmap.h>
#include <ace/generic/screen.h>

#define CURSOR_SPRITE_INDEX 0

static tBitMap *s_pCursorBitMap;
static tSprite *s_pCursorSprite;

void cursorCreate(tView *pView, char *szPath) {
	s_pCursorBitMap = bitmapCreateFromPath(szPath, 0);
	spriteManagerCreate(pView, 0);
	s_pCursorSprite = spriteAdd(CURSOR_SPRITE_INDEX, s_pCursorBitMap);
}

void cursorDestroy(void) {
	spriteRemove(s_pCursorSprite);
	spriteManagerDestroy();
	bitmapDestroy(s_pCursorBitMap);
}

void cursorUpdate(void) {
	s_pCursorSprite->wX = mouseGetX(MOUSE_PORT_1);
	s_pCursorSprite->wY = mouseGetY(MOUSE_PORT_1);

	spriteProcess(s_pCursorSprite);
}
