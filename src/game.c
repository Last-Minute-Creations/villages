#include "game.h"

#include <ace/managers/rand.h>
#include <ace/managers/key.h>
#include <ace/managers/mouse.h>

#include <ace/utils/palette.h>

#include "gamestates/menu/menu.h"

#define GENERIC_MAIN_LOOP_CONDITION g_pGameStateManager->pCurrent
#include <ace/generic/main.h>

tFont *g_pFont;
tTextBitMap *g_pTextBitMap;
tStateManager *g_pGameStateManager;
tState *g_pGameStates[GAME_STATE_COUNT] = {
	[GAME_STATE_MENU] = &g_sMenuState,
	[GAME_STATE_BOARD] = &g_sMenuState,
};
UWORD g_pPalette[GAME_COLOR_COUNT] = {0};

void genericCreate(void) {
	randInit(0);
	keyCreate();
	mouseCreate(MOUSE_PORT_1);

	g_pFont = fontCreate("data/fonts/arpegius-15.fnt");
	g_pTextBitMap = fontCreateTextBitMap(GAME_SCREEN_WIDTH, g_pFont->uwHeight);
	paletteLoad("data/palettes/global.plt", g_pPalette, GAME_COLOR_COUNT);

	g_pGameStateManager = stateManagerCreate();
	statePush(g_pGameStateManager, g_pGameStates[GAME_STATE_BOARD]);
}

void genericProcess(void) {
	mouseProcess();
	keyProcess();

	stateProcess(g_pGameStateManager);
}

void genericDestroy(void) {
	stateManagerDestroy(g_pGameStateManager);

	fontDestroy(g_pFont);
	fontDestroyTextBitMap(g_pTextBitMap);

	mouseDestroy();
	keyDestroy();
}
