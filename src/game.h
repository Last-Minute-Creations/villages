#ifndef GUARD_GAME_H
#define GUARD_GAME_H

#include <ace/generic/screen.h>
#include <ace/managers/state.h>
#include <ace/utils/font.h>
#include <ace/utils/extview.h>

/* Types */

#define GAME_SCREEN_WIDTH SCREEN_PAL_WIDTH
#define GAME_SCREEN_HEIGHT SCREEN_PAL_HEIGHT
#define GAME_BPP 5
#define GAME_COLOR_COUNT (1 << GAME_BPP)

#define GAME_STATE_MENU 0
#define GAME_STATE_BOARD 1
#define GAME_STATE_COUNT 2

/* Globals */

extern tView *g_pView;
extern tFont *g_pFont;
extern tTextBitMap *g_pTextBitMap;
extern tStateManager *g_pStateManager;
extern UWORD g_pPalette[GAME_COLOR_COUNT];
extern tState *g_pStates[GAME_STATE_COUNT];

/* Functions */

#endif