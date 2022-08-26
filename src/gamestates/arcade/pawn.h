#ifndef GUARD_GAMESTATE_ARCADE_PAWN_H
#define GUARD_GAMESTATE_ARCADE_PAWN_H


#include <ace/utils/bitmap.h>
#include <ace/managers/blit.h>

#include "config.h"
#include "gamestates/arcade/arcade.h"
#include "gamestates/arcade/logic.h"

/* Types */

/* Globals */
extern struct BitMap *g_pPawnsBitMap;
extern UBYTE *g_pPawnMask;

/* Functions */
void pawnCreate(void);

void pawnDestroy(void);

UBYTE pawnIsPlaceable(
	UWORD uwTileX,
	UWORD uwTileY,
	UBYTE ubLogicDataIdx
);

void pawnPlace(
	UWORD uwTileX,
	UWORD uwTileY,
	UBYTE ubLogicDataIdx,
	tPlayer *pPlayer
);

void pawnRemove(
	tPawn *pPawn
);

void pawnRedrawCallback(
	UWORD uwTileX,
	UWORD uwTileY,
	struct BitMap *pBmDest,
	UWORD uwBitMapX,
	UWORD uwBitMapY
);

#endif
