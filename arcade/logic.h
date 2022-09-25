#ifndef GUARD_GAMESTATE_ARCADE_LOGIC_H
#define GUARD_GAMESTATE_ARCADE_LOGIC_H

#include "gamestates/arcade/tile.h"

/* Types */
#define LOGIC_VOID 0
#define LOGIC_GRASS 1
#define LOGIC_VILLAGE 2
#define LOGIC_ROAD 3
#define LOGIC_BONUS 4
#define LOGIC_SWAMP 5
#define LOGIC_COUNT 5

#define LOGIC_TILE_DIRS 9

typedef struct {
	tUwCoordYX sTileCoord;
	UBYTE ubLogicDataIdx;
} tLogicListItem;

typedef struct {
	tLogicListItem *pItems;
	UWORD uwPos;
	UWORD uwLength;
	UBYTE ubClosed;
	UBYTE ubType;
	UBYTE ubPointAdd;
	UBYTE ubPointMult;
	UBYTE pVisitedLogicIdx[LOGIC_TILE_DIRS];
} tLogicList;

/* Globals */
extern tLogicList g_sLogicList;

/* Functions */
void logicListCreate(void);

void logicListDestroy(void);

void logicCountPoints(void);

void logicSumWalkerResults(void);

void logicAssignPoints(
	UBYTE *pPlayerPawnCounts
);

UBYTE logicIsStructureOccupied(
	UWORD uwTileX,
	UWORD uwTileY,
	UBYTE ubLogicDataId
);

void logicListMakeDistinct(void);

void logicWalkMakeStep(
	UWORD uwTileX,
	UWORD uwTileY,
	UBYTE ubLogicDataIdx
);

void logicWalkRoad(
	UWORD uwTileX,
	UWORD uwTileY,
	UBYTE ubLogicDataIdx
);

void logicWalkVillage(
	UWORD uwTileX,
	UWORD uwTileY,
	UBYTE ubLogicDataIdx
);

void logicWalkSwamp(
	UWORD uwTileX,
	UWORD uwTileY,
	UBYTE ubLogicDataIdx
);

#endif
