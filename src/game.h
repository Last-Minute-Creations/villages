#ifndef GUARD_VILLAGES_GAME_H
#define GUARD_VILLAGES_GAME_H

#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/state.h>

#define GAME_STATE_MAP 0
#define GAME_STATE_COUNT 1

extern const UWORD WINDOW_SCREEN_WIDTH;
extern const UWORD WINDOW_SCREEN_HEIGHT;
extern const UWORD WINDOW_SCREEN_BPP;

extern tStateManager *g_pGameStateManager;
extern tState *g_pGameStates[];

void createGameStates();
void destroyGameStates();

#endif
