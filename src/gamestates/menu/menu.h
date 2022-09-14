#ifndef GUARD_GAMESTATE_MENU_MENU_H
#define GUARD_GAMESTATE_MENU_MENU_H

#include <ace/managers/state.h>

/* Types */

/* Globals */

extern tState g_sStateMenu;

/* Functions */
void gsMenuCreate(void);

void gsMenuSplashSetup(void);

void gsMenuSplashLoop(void);

void gsMenuLobbySetup(void);

void gsMenuLobbyLoop(void);

void gsMenuDestroy(void);

void gsMenuDrawBg(void);

void gsMenuCharacterAction(
	UBYTE ubPlayer
);

void gsMenuPlayAction(void);

void gsMenuHowToAction(void);

#endif
