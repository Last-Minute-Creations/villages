#ifndef GUARD_GAMESTATE_ARCADE_HUD_H
#define GUARD_GAMESTATE_ARCADE_HUD_H

#include <ace/utils/extview.h>
#include <ace/utils/font.h>

#include "config.h"
#include "gamestates/arcade/arcade.h"
#include "gamestates/arcade/tile.h"

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
	struct BitMap *pRectBfr;
	tView *pExtView;
	tVPort *pMainVPort;
	tVPort *pHudVPort;
	tHudPlayerCache *pPlayerCache;
} tHudManager;

/* Globals */
extern tHudManager g_sHudManager;

/* Functions */
void hudCreate(
	IN tView *pExtView
);

void hudDestroy(void);

void hudRedrawAll(void);

void hudRedrawPlayer(
	IN tPlayer *pPlayer
);

void hudDrawLargePawn(
	IN tPlayer *pPlayer
);

void hudShowPlayer(
	IN tPlayer *pPlayer
);

void hudExpand(void);

void hudMinimize(void);

void hudShow(void);

void hudHide(void);

void hudSummary(void);

#endif
