#include "menu.h"

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

/* Types */

/* Globals */

tSimpleBufferManager *g_pMenuBuffer;
tStateManager g_sMenuStateManager;
tBitMap *g_pSplashBitMap;

tState g_sMenuStateSplash;
tState g_sMenuStateMainMenu;

/* Statics */

static tView *s_pMenuView;
static tVPort *s_pMenuVPort;

/* Functions */

void gsMenuCreate(void) {
	logBlockBegin("gsMenuCreate()");

	s_pMenuView = viewCreate(
		NULL,
		TAG_VIEW_GLOBAL_CLUT, 1,
		TAG_DONE
	);
	s_pMenuVPort = vPortCreate(
		NULL,
		TAG_VPORT_VIEW, s_pMenuView,
		TAG_VPORT_BPP, GAME_BPP,
		TAG_DONE
	);
	g_pMenuBuffer = simpleBufferCreate(
		NULL,
		TAG_SIMPLEBUFFER_VPORT, s_pMenuVPort,
		TAG_DONE
	);

	cursorCreate(s_pMenuView, 0, "data/cursors/hand.bm", 0);
	g_pSplashBitMap = bitmapCreateFromFile("data/debug/splash.bm", TRUE);

	copBlockDisableSprites(s_pMenuView->pCopList, 0xFE);
	systemSetDmaBit(DMAB_SPRITE, TRUE);
	viewLoad(s_pMenuView);

	statePush(&g_sMenuStateManager, &g_sMenuStateSplash);

	logBlockEnd("gsMenuCreate()");

	systemUnuse();
}

void gsMenuLoop(void) {
	if (keyUse(KEY_TAB)) {
		debugToggle();
	}

	debugSetColor(0x800);
	cursorUpdate();

	stateProcess(&g_sMenuStateManager);

	viewProcessManagers(s_pMenuView);
	copProcessBlocks();

	debugSetColor(s_pMenuVPort->pPalette[0]);
	vPortWaitForEnd(s_pMenuVPort);
}

void gsMenuDestroy(void) {
	systemUse();

	logBlockBegin("gsMenuDestroy()");

	statePopAll(&g_sMenuStateManager);

	bitmapDestroy(g_pSplashBitMap);

	cursorDestroy();

	viewDestroy(s_pMenuView);

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

void gsMenuCheckForActions() {
	if (keyUse(KEY_ESCAPE)) {
		gsMenuSetQueuedMenuAction(MENU_ACTION_EXIT, TRUE);
	}
	else if (mouseUse(MOUSE_PORT_1, MOUSE_LMB)) {
		gsMenuSetQueuedMenuAction(MENU_ACTION_SHOW_MAIN_MENU, FALSE);
	}
}

void gsMenuSplashLoop(void) {
	fadeProcess();
	if (fadeGetState() == FADE_STATE_MORPHING) {
		paletteDim(g_pPalette, (UWORD *) g_pCustom->color, GAME_COLOR_COUNT, fadeGetLevel());
	}
	else {
		if (gsMenuGetQueuedMenuAction() != MENU_ACTION_NONE) {
			gsMenuCallQueuedMenuAction();
		}
		else {
			gsMenuCheckForActions();
		}
	}
}

void gsMenuSplashDestroy() {
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
