#include "game.h"

#include <ace/managers/rand.h>
#include <ace/managers/key.h>
#include <ace/managers/mouse.h>

#include <ace/utils/palette.h>

#include "gamestates/menu/menu.h"
#include "cursor.h"

#define GENERIC_MAIN_LOOP_CONDITION g_pStateManager->pCurrent
#include <ace/generic/main.h>

/* Types */

/* Globals */

tView *g_pView;
tRandManager *g_pRandManager;
tFont *g_pFont;
tTextBitMap *g_pTextBitMap;
tStateManager *g_pStateManager;
tState *g_pStates[GAME_STATE_COUNT] = {
	[GAME_STATE_MENU] = &g_sMenuState,
	[GAME_STATE_BOARD] = &g_sMenuState,
};
UWORD g_pPalette[GAME_COLOR_COUNT] = {0};

/* Statics */

/* Functions */

void genericCreate(void) {
	g_pRandManager = randCreate(1234, 5678);
	keyCreate();
	mouseCreate(MOUSE_PORT_1);

	g_pView = viewCreate(
		NULL,
		TAG_VIEW_GLOBAL_PALETTE, 1,
		TAG_DONE
	);

	copBlockDisableSprites(g_pView->pCopList, 0xFE);
	systemSetDmaBit(DMAB_SPRITE, TRUE);

	cursorCreate(g_pView, "data/cursors/hand.bm");
	g_pFont = fontCreate("data/fonts/arpegius-15.fnt");
	g_pTextBitMap = fontCreateTextBitMap(GAME_SCREEN_WIDTH, g_pFont->uwHeight);
	paletteLoad("data/palettes/global.plt", g_pPalette, GAME_COLOR_COUNT);

	g_pStateManager = stateManagerCreate();
	statePush(g_pStateManager, g_pStates[GAME_STATE_BOARD]);
}

void genericProcess(void) {
	mouseProcess();
	keyProcess();

	stateProcess(g_pStateManager);
}

void genericDestroy(void) {
	stateManagerDestroy(g_pStateManager);

	fontDestroyTextBitMap(g_pTextBitMap);
	fontDestroy(g_pFont);
	cursorDestroy();

	viewDestroy(g_pView);

	mouseDestroy();
	keyDestroy();
	randDestroy(g_pRandManager);
}
