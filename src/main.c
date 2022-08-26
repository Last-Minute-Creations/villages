#include <ace/generic/main.h>
#include <ace/managers/rand.h>
#include <ace/managers/key.h>
#include <ace/managers/joy.h>
#include <ace/managers/mouse.h>

#include "global.h"
#include "gamestates/menu/menu.h"


void genericCreate(void) {
	randInit(0);
	keyCreate();
	joyOpen();
	mouseCreate(MOUSE_PORT_1);
	globalCreate();

	g_pGameStateManager = stateManagerCreate();
	stateChange(g_pGameStateManager, &g_sStateMenu);
}

void genericProcess(void) {
	mouseProcess();
	keyProcess();
	joyProcess();

	stateProcess(g_pGameStateManager);
}

void genericDestroy(void) {
	stateManagerDestroy(g_pGameStateManager);

	globalDestroy();
	mouseDestroy();
	joyClose();
	keyDestroy();
}
