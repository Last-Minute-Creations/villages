#include "gamestates/menu/menu.h"

#include <ace/managers/game.h>
#include <ace/managers/key.h>
#include <ace/managers/mouse.h>
#include <ace/managers/timer.h>
#include <ace/managers/log.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/blit.h>
#include <ace/managers/system.h>
#include <ace/utils/palette.h>

#include "game.h"
#include "cursor.h"
#include "debug.h"
#include "fade.h"
#include "utils.h"

#include "gamestates/menu/actions.h"
#include "gamestates/menu/scrolls.h"

/* Types */

/* Globals */

tVPort *g_pMenuVPort;
tSimpleBufferManager *g_pMenuBuffer;
tBitMap *g_pSplashBitMap;

/* Statics */

/* Functions */

void gsMenuCreate(void) {
	logBlockBegin("gsMenuCreate()");

	viewLoad(0);

	g_pMenuVPort = vPortCreate(
		NULL,
		TAG_VPORT_VIEW, g_pView,
		TAG_VPORT_BPP, GAME_BPP,
		TAG_DONE
	);
	g_pMenuBuffer = simpleBufferCreate(
		NULL,
		TAG_SIMPLEBUFFER_VPORT, g_pMenuVPort,
		TAG_SIMPLEBUFFER_USE_X_SCROLLING, FALSE,
		TAG_DONE
	);

	g_pSplashBitMap = bitmapCreateFromFile("data/bitmaps/splash.bm", TRUE);
	scrollsCreate();

	viewLoad(g_pView);

	actionsInit();

	logBlockEnd("gsMenuCreate()");

	systemUnuse();
}

void gsMenuLoop(void) {
	debugSetColor(g_pMenuVPort->pPalette[0]);

	if (keyUse(KEY_TAB)) {
		debugToggle();
	}

	cursorUpdate();

	debugSetColor(0x800);
	actionsProcess();
	debugSetColor(0x008);

	viewProcessManagers(g_pView);
	copProcessBlocks();

	debugSetColor(g_pMenuVPort->pPalette[0]);
	vPortWaitForEnd(g_pMenuVPort);
}

void gsMenuDestroy(void) {
	systemUse();

	logBlockBegin("gsMenuDestroy()");

	scrollsDestroy();
	bitmapDestroy(g_pSplashBitMap);

	viewDestroy(g_pView);

	logBlockEnd("gsMenuDestroy()");
}

tState g_sMenuState = {
	.cbCreate = gsMenuCreate,
	.cbLoop = gsMenuLoop,
	.cbDestroy = gsMenuDestroy,
};
