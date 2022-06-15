#ifndef GUARD_GAMESTATE_MENU_MENU_H
#define GUARD_GAMESTATE_MENU_MENU_H

#include <clib/exec_protos.h> // Amiga typedefs

#include "config.h"
#include "global.h"

#include <ace/managers/game.h>
#include <ace/managers/mouse.h>
#include <ace/managers/timer.h>
#include <ace/managers/log.h>

#include <ace/managers/viewport/simplebuffer.h>

#include <ace/utils/extview.h>
#include <ace/utils/font.h>
#include <ace/utils/bitmap.h>
#include <ace/utils/palette.h>

#include "gamestates/arcade/arcade.h"

/* Types */

/* Globals */

/* Functions */
void gsMenuCreate(void);

void gsMenuSplashSetup(void);

void gsMenuSplashLoop(void);

void gsMenuLobbySetup(void);

void gsMenuLobbyLoop(void);

void gsMenuDestroy(void);

void gsMenuDrawBg(void);

void gsMenuCharacterAction(
	IN UBYTE ubPlayer
);

void gsMenuPlayAction(void);

void gsMenuHowToAction(void);

#endif
