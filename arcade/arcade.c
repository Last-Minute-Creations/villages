#include "arcade.h"
#include <ace/generic/screen.h>
#include <ace/utils/extview.h>
#include <ace/utils/palette.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/system.h>
#include <ace/managers/key.h>
#include <ace/managers/state.h>

#include "global.h"
#include "cursor.h"
#include "player.h"
#include "logic.h"
#include "hud.h"
#include "pawn.h"

#include "gamestates/menu/menu.h"

#define TILE_BOUND_X 100
#define TILE_BOUND_Y 100
#define TILE_SIZE_SHIFT 5
#define TILE_SIZE (1 << TILE_SIZE_SHIFT)

/* Globals */
tPlayer *g_pCurrPlayer;
tTileBufferManager *g_pTileBuffer;
tUwCoordYX g_sLastTileCoord;

/* Statics */
// Ekran
static tView *s_pView;
static tVPort *s_pMainVPort;
static tVPort *s_pHudVPort;

static tBitMap* s_pTileSetBitMap;
static tSimpleBufferManager* s_pHudBuffer;

// Stan gry
UBYTE g_ubTurnStep;
UBYTE g_ubScrollSpeed = 2; // TODO: wjebac do konfiga

// Indeks kursora - obecny i poprzedni
UBYTE g_ubPointerIdx;
UBYTE g_ubPrevPointerIdx;

// Pamiec dla sprawdzania na hoverze
UBYTE g_pTileIsPlacable[9];
UBYTE g_ubPrevLogicDataIdx;

/* Functions */

// ustawia kolory kursora na dwa wskazane indeksy z palety
void setPointerColors(UBYTE *pColorIdx) {
	s_pMainVPort->pPalette[19] = s_pMainVPort->pPalette[pColorIdx[0]];
	s_pMainVPort->pPalette[18] = s_pMainVPort->pPalette[pColorIdx[1]];
}

void gsArcadeCreate(void) {
	logBlockBegin("gsArcadeCreate()");

	s_pView = viewCreate(
		NULL,
		TAG_VIEW_GLOBAL_CLUT, 1,
		TAG_DONE
	);
	s_pMainVPort = vPortCreate(
		NULL,
		TAG_VPORT_VIEW, s_pView,
		TAG_VPORT_WIDTH, SCREEN_PAL_WIDTH,
		TAG_VPORT_HEIGHT, SCREEN_PAL_HEIGHT - 32,
		TAG_VPORT_BPP, GAME_BPP,
		TAG_DONE
	);

	s_pTileSetBitMap = bitmapCreateFromFile("data/_old/tiles/selur_v1.bm", FALSE);
	g_pTileBuffer = tileBufferCreate(
		NULL,
		TAG_TILEBUFFER_VPORT, s_pMainVPort,
		TAG_TILEBUFFER_BOUND_TILE_X, TILE_BOUND_X,
		TAG_TILEBUFFER_BOUND_TILE_Y, TILE_BOUND_Y,
		TAG_TILEBUFFER_TILE_SHIFT, TILE_SIZE_SHIFT,
		TAG_TILEBUFFER_TILESET, s_pTileSetBitMap,
		TAG_TILEBUFFER_CALLBACK_TILE_DRAW, pawnRedrawCallback,
		TAG_TILEBUFFER_REDRAW_QUEUE_LENGTH, 100,
		TAG_DONE
	);

	s_pHudVPort = vPortCreate(
		NULL,
		TAG_VPORT_VIEW, s_pView,
		TAG_VPORT_WIDTH, SCREEN_PAL_WIDTH,
		TAG_VPORT_HEIGHT, HUD_VPORT_HEIGHT,
		TAG_VPORT_BPP, GAME_BPP,
		TAG_DONE
	);

	s_pHudBuffer = simpleBufferCreate(
		NULL,
		TAG_SIMPLEBUFFER_VPORT, s_pHudVPort,
		TAG_SIMPLEBUFFER_BOUND_WIDTH, SCREEN_PAL_WIDTH,
		TAG_SIMPLEBUFFER_BOUND_HEIGHT, HUD_VPORT_HEIGHT * g_sGameConfig.ubPlayerCount,
		TAG_SIMPLEBUFFER_USE_X_SCROLLING, FALSE,
		TAG_DONE
	);
	// palety
	paletteLoad("data/_old/palettes/selur_v1.plt", s_pMainVPort->pPalette, 32);

	cursorCreate(s_pView, 0, "data/_old/hand.bm", 0);

	// Gracze, pionki, logika
	pawnCreate();
	playerListCreate();
	g_pCurrPlayer = g_sGameConfig.pPlayerFirst;
	setPointerColors(g_pPlayerColors[g_pCurrPlayer->ubIdx]);
	tileStackCreate();
	logicListCreate();
	hudCreate(s_pView);
	g_ubPointerIdx = POINTER_NW;
	g_ubPrevPointerIdx = POINTER_NW;
	g_ubTurnStep = TURN_TILE;

	// kafel startowy
	g_pTileBuffer->pTileData[TILE_BOUND_X / 2][TILE_BOUND_Y / 2] = 4 + uwRandMinMax(0, 3) * TILE_TYPES;
	// Poczatkowe przygotowanie ekranu
	cameraCenterAt(
		g_pTileBuffer->pCamera,
		(TILE_BOUND_X * TILE_SIZE) / 2,
		(TILE_BOUND_Y * TILE_SIZE) / 2
	);
	tileBufferRedrawAll(g_pTileBuffer);

	copBlockDisableSprites(s_pView->pCopList, 0xFE);
	systemSetDmaBit(DMAB_SPRITE, TRUE);
	viewLoad(s_pView);

	logBlockEnd("gsArcadeCreate()");

	systemUnuse();
}

void gsArcadeDestroy(void) {
	systemUse();

	logBlockBegin("gsArcadeDestroy()");

	viewLoad(0);

	viewDestroy(s_pView);
	bitmapDestroy(s_pTileSetBitMap);

	pawnDestroy();
	playerListDestroy();
	hudDestroy();
	logicListDestroy();
	tileStackDestroy();

	logBlockEnd("gsArcadeDestroy()");
}

void endTurn(void) {
	logWrite("EndTurn\n");
	if (g_sTileList.uwPos >= g_sTileList.uwLength - 1) {
		hudSummary();
		// FIXME
		// gameChangeLoop(gsArcadeResultsLoop);
		return;
	}
	else {
		logicCountPoints();

		// zmien gracza
		if (g_pCurrPlayer->pNext) {
			g_pCurrPlayer = g_pCurrPlayer->pNext;
		}
		else {
			g_pCurrPlayer = g_sGameConfig.pPlayerFirst;
		}

		// kafel do przodu
		++g_sTileList.uwPos;
		g_ubTurnStep = TURN_TILE;

		// odrysuj HUD kolejnego gracza
		setPointerColors(g_pPlayerColors[g_pCurrPlayer->ubIdx]);
		hudRedrawAll();
		hudShowPlayer(g_pCurrPlayer);
	}
}

static void arcadeProcessScroll(UWORD uwMouseX, UWORD uwMouseY){
	BYTE bScrollX, bScrollY;
	// X
	bScrollX = 0;
	if (uwMouseX == 0) {
		bScrollX = -1;
	}
	else if(uwMouseX == SCREEN_PAL_WIDTH - 1) {
		bScrollX = 1;
	}
	// Y
	bScrollY = 0;
	if (uwMouseY == 0) {
		bScrollY = -1;
	}
	else if(uwMouseY == SCREEN_PAL_HEIGHT - 1) {
		bScrollY = 1;
	}

	cameraMoveBy(g_pTileBuffer->pCamera, bScrollX * g_ubScrollSpeed, bScrollY * g_ubScrollSpeed);

	g_ubPointerIdx = g_pPointerDirs[bScrollY + 1][bScrollX + 1];
	if (g_ubPointerIdx != g_ubPrevPointerIdx) {
		// FIXME: do ustawienia kursora trzeba wyciagnac kod z openfire
		// mouseSetPointer(&g_pCursorData[g_ubPointerIdx * 36], 16, 16, g_pPointerSpots[g_ubPointerIdx][0], g_pPointerSpots[g_ubPointerIdx][1]);
	}
	g_ubPrevPointerIdx = g_ubPointerIdx;
}

void gsArcadeLoop(void) {
	cursorUpdate();

	UWORD uwMouseX = mouseGetX(MOUSE_PORT_1);
	UWORD uwMouseY = mouseGetY(MOUSE_PORT_1);

	if (keyUse(KEY_ESCAPE)) {
		hudSummary();
		// FIXME
		// gameChangeLoop(gsArcadeResultsLoop);
		return;
	}

	if (g_sHudManager.ubExpanded) {
		if (keyUse(KEY_SPACE) || mouseUse(MOUSE_PORT_1, MOUSE_LMB)) {
			hudMinimize();
		}
		return;
	}
	else {
		static const tUwRect sHudRect = {
			.uwX = 0,
			.uwY = SCREEN_PAL_HEIGHT - HUD_VPORT_HEIGHT,
			.uwWidth = SCREEN_PAL_WIDTH,
			.uwHeight = HUD_VPORT_HEIGHT,
		};
		// TODO: Use const pointer in mouseInRect
		if (keyUse(KEY_SPACE) || (mouseCheck(MOUSE_PORT_1, MOUSE_LMB) && mouseInRect(MOUSE_PORT_1, sHudRect))) {
			mouseUse(MOUSE_PORT_1, MOUSE_LMB); // zuzycie klikniecia
			hudExpand();
		}
	}

	switch (g_ubTurnStep) {
		case TURN_TILE: {
			if (mouseUse(MOUSE_PORT_1, MOUSE_LMB)) {
				UWORD uwTileX = (g_pTileBuffer->pCamera->uPos.uwX + uwMouseX) / g_pTileBuffer->ubTileSize;
				UWORD uwTileY = (g_pTileBuffer->pCamera->uPos.uwY + uwMouseY) / g_pTileBuffer->ubTileSize;

				if (tileIsPlaceable(uwTileX, uwTileY, &g_pTileDefs[g_sTileList.pTileIdx[g_sTileList.uwPos]])) {
					tilePlace(uwTileX, uwTileY, &g_pTileDefs[g_sTileList.pTileIdx[g_sTileList.uwPos]]);

					if (g_pCurrPlayer->ubPawnsLeft) {
						logWrite("Placed tile @%ux%u", uwTileX, uwTileY);
						g_ubTurnStep = TURN_PAWN;
						hudRedrawPlayer(g_pCurrPlayer);
						g_ubPrevLogicDataIdx = 0xff;

						UBYTE i = 9;
						while (i--) {
							g_pTileIsPlacable[i] = pawnIsPlaceable(uwTileX, uwTileY, i);
						}
					}
					else {
						endTurn();
					}
					return;
				}
			}

			if (mouseUse(MOUSE_PORT_1, MOUSE_RMB)) {
				tileRotate();
			}
		} break;
		case TURN_PAWN: {
			UWORD uwTileX = (g_pTileBuffer->pCamera->uPos.uwX + uwMouseX) / g_pTileBuffer->ubTileSize;
			UWORD uwTileY = (g_pTileBuffer->pCamera->uPos.uwY + uwMouseY) / g_pTileBuffer->ubTileSize;

			if ((uwTileX == g_sLastTileCoord.uwX) && (uwTileY == g_sLastTileCoord.uwY)) {
				UBYTE ubTileChunkX = (((g_pTileBuffer->pCamera->uPos.uwX + uwMouseX) % g_pTileBuffer->ubTileSize) + (g_pTileBuffer->ubTileSize >> 2)) / (g_pTileBuffer->ubTileSize >> 1);
				UBYTE ubTileChunkY = (((g_pTileBuffer->pCamera->uPos.uwY + uwMouseY) % g_pTileBuffer->ubTileSize) + (g_pTileBuffer->ubTileSize >> 2)) / (g_pTileBuffer->ubTileSize >> 1);

				UBYTE ubLogicDataIdx = (3 * ubTileChunkY) + ubTileChunkX;
				if (mouseUse(MOUSE_PORT_1, MOUSE_LMB)) {
					if (g_pTileIsPlacable[ubLogicDataIdx]) {
						pawnPlace(uwTileX, uwTileY, ubLogicDataIdx, g_pCurrPlayer);
						// FIXME
						// mouseSetPointer(&g_pCursorData[g_ubPointerIdx * 36], 16, 16, g_pPointerSpots[g_ubPointerIdx][0], g_pPointerSpots[g_ubPointerIdx][1]);
						endTurn();
					}
				}
				else if (ubLogicDataIdx != g_ubPrevLogicDataIdx) {
					g_ubPrevLogicDataIdx = ubLogicDataIdx;
					if (g_pTileIsPlacable[ubLogicDataIdx]) {
						// FIXME
						// mouseSetPointer(g_pPawnCursorData, 16, 16, -5, -5);
					}
					else {
						// FIXME
						// mouseSetPointer(&g_pCursorData[g_ubPointerIdx * 36], 16, 16, g_pPointerSpots[g_ubPointerIdx][0], g_pPointerSpots[g_ubPointerIdx][1]);
					}
				}
			}
			else if (g_ubPrevLogicDataIdx != 0xff) {
				// FXIME
				// mouseSetPointer(&g_pCursorData[g_ubPointerIdx * 36], 16, 16, g_pPointerSpots[g_ubPointerIdx][0], g_pPointerSpots[g_ubPointerIdx][1]);
				g_ubPrevLogicDataIdx = 0xff;
			}

			if (keyUse(KEY_RETURN)) {
				endTurn();
			}
		} break;
	}

	arcadeProcessScroll(uwMouseX, uwMouseY);

	// odrysuj kafle na marginesach lub caly ekran jesli offset za duzy
	viewProcessManagers(s_pView);
	copProcessBlocks();
	vPortWaitForEnd(s_pHudVPort);
}

void gsArcadeResultsLoop(void) {
	if (mouseUse(MOUSE_PORT_1, MOUSE_LMB) || keyUse(KEY_ESCAPE) || keyUse(KEY_SPACE) || keyUse(KEY_RETURN)) {
		stateChange(g_pGameStateManager, &g_sStateMenu);
	}
}

tState g_sStateArcade = {
	.cbCreate = gsArcadeCreate,
	.cbDestroy = gsArcadeDestroy,
	.cbLoop = gsArcadeLoop,
};
