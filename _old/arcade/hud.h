#ifndef GUARD_GAMESTATE_ARCADE_HUD_H
#define GUARD_GAMESTATE_ARCADE_HUD_H

#include <ace/managers/viewport/simplebuffer.h>
#include <ace/utils/extview.h>
#include <ace/utils/font.h>

#include "gamestates/arcade/arcade.h"
#include "gamestates/arcade/tile.h"

/* Defines */

#define HUD_VPORT_HEIGHT 32

/* Types */

typedef struct {
	// redraw-specific
	UBYTE ubBgDrawn;
	UBYTE ubTileErased;
	// common with tPlayer
	UBYTE ubPrevPawns;
	WORD wPrevScore;
} tHudPlayerCache;

typedef struct {
	UBYTE ubExpanded;
	tView *pView;
	tVPort *pMainVPort;
	tVPort *pHudVPort;
	tSimpleBufferManager *pSimpleBuffer;
	tCameraManager *pCamera;
	tHudPlayerCache *pPlayerCache;
} tHudManager;

/* Globals */
extern tHudManager g_sHudManager;

/* Functions */
void hudCreate(
	tView *pView
);

void hudDestroy(void);

void hudRedrawAll(void);

void hudRedrawPlayer(
	const tPlayer *pPlayer
);

void hudDrawLargePawn(
	const tPlayer *pPlayer
);

void hudShowPlayer(
	const tPlayer *pPlayer
);

void hudExpand(void);

void hudMinimize(void);

void hudShow(void);

void hudHide(void);

void hudSummary(void);

#endif
