#include "game.h"

#include <ace/types.h>

#include <ace/managers/state.h>
#include <ace/managers/key.h>
#include <ace/managers/blit.h>

#include <ace/utils/extview.h>

#include "config.h"

#include "gamestates/map.h"

const UWORD WINDOW_SCREEN_WIDTH = 320;
const UWORD WINDOW_SCREEN_HEIGHT = 256;
const UWORD WINDOW_SCREEN_BPP = 5;

tStateManager *g_pGameStateManager = 0;
tState *g_pGameStates[GAME_STATE_COUNT] = {0};

#define GENERIC_MAIN_LOOP_CONDITION g_pGameStateManager->pCurrent
#include <ace/generic/main.h>

void genericCreate() {
	logBlockBegin("genericCreate()");

	keyCreate();

	createGameStates();
	statePush(g_pGameStateManager, g_pGameStates[GAME_STATE_MAP]);

	systemUnuse();

	logBlockEnd("genericCreate()");
}

void genericProcess() {
	keyProcess();
	stateProcess(g_pGameStateManager);
}

void genericDestroy() {
	logBlockBegin("genericDestroy()");

	systemUse();

	destroyGameStates();
	keyDestroy();

	logBlockEnd("genericDestroy()");
}

void createGameStates() {
	g_pGameStateManager = stateManagerCreate();

	g_pGameStates[GAME_STATE_MAP] = stateCreate(gsMapCreate, gsMapLoop, gsMapDestroy, 0, 0, 0);
}

void destroyGameStates() {
	stateManagerDestroy(g_pGameStateManager);

	UBYTE ubStateIndex = GAME_STATE_COUNT;
	while (ubStateIndex--) {
		stateDestroy(g_pGameStates[ubStateIndex]);
	}
}
