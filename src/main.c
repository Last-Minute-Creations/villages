#include <ace/managers/rand.h>
#include <ace/managers/key.h>
#include <ace/managers/mouse.h>

#include "global.h"
#include "gamestates/menu/menu.h"

#define GENERIC_MAIN_LOOP_CONDITION g_pGameStateManager->pCurrent
#include <ace/generic/main.h>

tStateManager *g_pGameStateManager;

void genericCreate(void) {
	randInit(0);
	keyCreate();
	mouseCreate(MOUSE_PORT_1);
	globalCreate();

	g_pGameStateManager = stateManagerCreate();
	stateChange(g_pGameStateManager, &g_sStateMenu);
}

void genericProcess(void) {
	mouseProcess();
	keyProcess();

	stateProcess(g_pGameStateManager);
}

void genericDestroy(void) {
	stateManagerDestroy(g_pGameStateManager);

	globalDestroy();
	mouseDestroy();
	keyDestroy();
}
