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

/* Types */

/* Globals */

/* Statics */

static tView *s_pMenuView;
static tVPort *s_pMenuVPort;
static tSimpleBufferManager *s_pMenuBuffer;
static tStateManager s_sMenuStateManager;

static tState s_sMenuStateSplash;

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
		// TAG_VPORT_PALETTE_PTR, g_pPalette,
		// TAG_VPORT_PALETTE_SIZE, GAME_COLOR_COUNT,
		TAG_DONE
	);
	s_pMenuBuffer = simpleBufferCreate(
		NULL,
		TAG_SIMPLEBUFFER_VPORT, s_pMenuVPort,
		TAG_DONE
	);

	cursorCreate(s_pMenuView, 0, "data/cursors/hand.bm", 0);

	copBlockDisableSprites(s_pMenuView->pCopList, 0xFE);
	systemSetDmaBit(DMAB_SPRITE, TRUE);
	viewLoad(s_pMenuView);

	statePush(&s_sMenuStateManager, &s_sMenuStateSplash);

	logBlockEnd("gsMenuCreate()");

	systemUnuse();
}

void gsMenuLoop(void) {
	if (keyUse(KEY_TAB)) {
		debugToggle();
	}

	debugSetColor(0x800);
	cursorUpdate();

	stateProcess(&s_sMenuStateManager);

	viewProcessManagers(s_pMenuView);
	copProcessBlocks();

	debugSetColor(s_pMenuVPort->pPalette[0]);
	vPortWaitForEnd(s_pMenuVPort);
}

void gsMenuSplashCreate(void) {
	logBlockBegin("gsMenuSplashCreate()");

	systemUse();

	bitmapLoadFromFile(s_pMenuBuffer->pBack, "data/debug/splash.bm", 0, 0);

	fadeMorphTo(FADE_TARGET_IN);

	systemUnuse();

	logBlockEnd("gsMenuSplashCreate()");
}

void gsMenuSplashLoop(void) {
	fadeProcess();
	if (fadeGetState() == FADE_STATE_MORPHING) {
		paletteDim(s_pMenuVPort->pPalette, (UWORD *) g_pCustom->color, GAME_COLOR_COUNT, fadeGetLevel());
	}

	if (keyUse(KEY_ESCAPE)) {
		statePop(g_pGameStateManager);
		return;
	}

	if (keyUse(KEY_UP)) {
		logWrite("Up!");
		fadeMorphTo(FADE_TARGET_IN);
	}

	if (keyUse(KEY_DOWN)) {
		logWrite("Down!");
		fadeMorphTo(FADE_TARGET_OUT);
	}

	// if (mouseUse(MOUSE_PORT_1, MOUSE_LMB)) {
	// 	stateChange(&s_sMenuStateManager, &s_sMenuStateLobby);
	// }
}

void gsMenuDestroy(void) {
	systemUse();

	logBlockBegin("gsMenuDestroy()");

	cursorDestroy();

	viewDestroy(s_pMenuView);

	logBlockEnd("gsMenuDestroy()");
}

tState g_sMenuState = {
	.cbCreate = gsMenuCreate,
	.cbLoop = gsMenuLoop,
	.cbDestroy = gsMenuDestroy,
};

static tState s_sMenuStateSplash = {
	.cbCreate = gsMenuSplashCreate,
	.cbLoop = gsMenuSplashLoop,
};
