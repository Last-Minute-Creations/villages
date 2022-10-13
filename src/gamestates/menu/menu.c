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
tStateManager g_sMenuStateManager;
tBitMap *g_pSplashBitMap;

tState g_sMenuStateSplash;
tState g_sMenuStateMainMenu;

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

	g_pSplashBitMap = bitmapCreateFromFile("data/debug/splash.bm", TRUE);
	scrollsCreate();

	viewLoad(g_pView);

	statePush(&g_sMenuStateManager, &g_sMenuStateSplash);

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
	stateProcess(&g_sMenuStateManager);
	debugSetColor(0x008);

	viewProcessManagers(g_pView);
	copProcessBlocks();

	debugSetColor(g_pMenuVPort->pPalette[0]);
	vPortWaitForEnd(g_pMenuVPort);
}

void gsMenuDestroy(void) {
	systemUse();

	logBlockBegin("gsMenuDestroy()");

	statePopAll(&g_sMenuStateManager);

	scrollsDestroy();
	bitmapDestroy(g_pSplashBitMap);

	viewDestroy(g_pView);

	logBlockEnd("gsMenuDestroy()");
}

void gsMenuSplashCreate(void) {
	logBlockBegin("gsMenuSplashCreate()");

	blitCopyAligned(
		g_pSplashBitMap, 0, 0,
		g_pMenuBuffer->pBack, 0, 0,
		GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT
	);

	fadeMorphTo(FADE_TARGET_IN);
	gsMenuSetQueuedMenuAction(MENU_ACTION_DRAW_SPLASH_PROMPT, FALSE);

	logBlockEnd("gsMenuSplashCreate()");
}

void gsMenuCheckForActions(void) {
	if (keyUse(KEY_ESCAPE)) {
		gsMenuSetQueuedMenuAction(MENU_ACTION_EXIT, TRUE);
	}
	else if (mouseUse(MOUSE_PORT_1, MOUSE_LMB)) {
		gsMenuSetQueuedMenuAction(MENU_ACTION_SHOW_MAIN_MENU, FALSE);
	}
}

BYTE bDirLeft = 1;
BYTE bDirRight = 1;

void gsMenuSplashLoop(void) {
	fadeProcess();

	if (fadeGetState() == FADE_STATE_MORPHING) {
		paletteDim(g_pPalette, (UWORD *) g_pCustom->color, GAME_COLOR_COUNT, fadeGetLevel());
		return;
	}

	if (keyUse(KEY_SPACE)) {
		scrollsProcess();
	}

	if (gsMenuGetQueuedMenuAction() != MENU_ACTION_NONE) {
		gsMenuCallQueuedMenuAction();
	}
	else {
		gsMenuCheckForActions();
	}

	if (bDirLeft) {
		if (scrollsRequestStage(SCROLL_LEFT, SCROLL_STAGE_OPEN)) {
			logWrite("Left log opened!");
			bDirLeft = 0;
		}
	}
	else {
		if (scrollsRequestStage(SCROLL_LEFT, SCROLL_STAGE_CLOSED)) {
			logWrite("Left log closed!");
			bDirLeft = 1;
		}
	}

	// if (bDirRight) {
	// 	if (scrollsRequestStage(SCROLL_RIGHT, SCROLL_STAGE_OPEN)) {
	// 		logWrite("Right log opened!");
	// 		bDirRight = 0;
	// 	}
	// }
	// else {
	// 	if (scrollsRequestStage(SCROLL_RIGHT, SCROLL_STAGE_CLOSED)) {
	// 		logWrite("Right log closed!");
	// 		bDirRight = 1;
	// 	}
	// }
}

void gsMenuSplashDestroy(void) {
	logBlockBegin("gsMenuSplashDestroy()");

	gsMenuCallMenuAction(MENU_ACTION_UNDRAW_SPLASH_PROMPT);

	logBlockEnd("gsMenuSplashDestroy()");
}

tState g_sMenuState = {
	.cbCreate = gsMenuCreate,
	.cbLoop = gsMenuLoop,
	.cbDestroy = gsMenuDestroy,
};

tState g_sMenuStateSplash = {
	.cbCreate = gsMenuSplashCreate,
	.cbLoop = gsMenuSplashLoop,
	.cbDestroy = gsMenuSplashDestroy,
};

tState g_sMenuStateMainMenu = {
	.cbCreate = gsMenuSplashCreate,
	.cbLoop = gsMenuSplashLoop,
	.cbDestroy = gsMenuSplashDestroy,
};
