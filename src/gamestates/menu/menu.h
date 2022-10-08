#ifndef GUARD_GAMESTATE_MENU_MENU_H
#define GUARD_GAMESTATE_MENU_MENU_H

#include <ace/managers/state.h>
#include <ace/managers/viewport/simplebuffer.h>

/* Types */

/* Globals */

extern tVPort *g_pMenuVPort;
extern tSimpleBufferManager *g_pMenuBuffer;
extern tStateManager g_sMenuStateManager;
extern tBitMap *g_pSplashBitMap;

extern tState g_sMenuState;
extern tState g_sMenuStateSplash;
extern tState g_sMenuStateMainMenu;

/* Functions */

#endif // GUARD_GAMESTATE_MENU_MENU_H
