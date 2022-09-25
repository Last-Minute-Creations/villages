#ifndef GUARD_GAMESTATE_ARCADE_ARCADE_H
#define GUARD_GAMESTATE_ARCADE_ARCADE_H

#include <ace/managers/state.h>
#include <ace/managers/viewport/tilebuffer.h>

#include "global.h"

/* Types */
#define TURN_TILE 0
#define TURN_PAWN 1

extern tState g_sStateArcade;

extern tPlayer *g_pCurrPlayer;
extern tUwCoordYX g_sLastTileCoord;
extern tTileBufferManager *g_pTileBuffer;
extern UBYTE g_ubTurnStep;

#endif
