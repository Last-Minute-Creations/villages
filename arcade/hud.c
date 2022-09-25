#include "hud.h"

#include <ace/generic/screen.h>

#include "global.h"
#include "arcade.h"

/* Globals */
static struct BitMap *g_pPawnsLarge;
tHudManager g_sHudManager;

/* Functions */
void hudCreate(tView *pView) {
	UBYTE i;

	g_sHudManager.pView = pView;
	g_sHudManager.pMainVPort = pView->pFirstVPort;
	g_sHudManager.pHudVPort = g_sHudManager.pMainVPort->pNext;
	g_sHudManager.ubExpanded = 0;
	g_sHudManager.pCamera = (tCameraManager *) vPortGetManager(g_sHudManager.pHudVPort, VPM_CAMERA);
	g_sHudManager.pSimpleBuffer = (tSimpleBufferManager *) vPortGetManager(g_sHudManager.pHudVPort, VPM_SCROLL);

	g_pPawnsLarge = bitmapCreateFromFile("data/_old/bitmaps/pawnsLarge.bm", FALSE);

	g_sHudManager.pPlayerCache = memAllocFast(sizeof(g_sHudManager.pPlayerCache[0]) * g_sGameConfig.ubPlayerCount);
	for (i = 0; i < g_sGameConfig.ubPlayerCount; ++i) {
		g_sHudManager.pPlayerCache[i].ubBgDrawn = FALSE;
	}

	hudRedrawAll();
	// TODO: zastapic kodem menadgera skrolingu
	cameraSetCoord(g_sHudManager.pCamera , 0, 32 * g_pCurrPlayer->ubIdx);
}

void hudDestroy(void) {
	memFree(g_sHudManager.pPlayerCache, sizeof(g_sHudManager.pPlayerCache[0]) * g_sGameConfig.ubPlayerCount);
	bitmapDestroy(g_pPawnsLarge);
}

void hudRedrawAll(void) {
	UBYTE i;
	tPlayer *pPlayer;
	pPlayer = g_sGameConfig.pPlayerFirst;
	for (i = g_sGameConfig.ubPlayerCount; i--;) {
		hudRedrawPlayer(pPlayer);
		pPlayer = pPlayer->pNext;
	}
}

void hudRedrawPlayer(const tPlayer *pPlayer) {
	char szBfr[128];
	UBYTE ubBgFresh = 0;
	tBitMap *pHudBitMap = g_sHudManager.pSimpleBuffer->pBack;
	UBYTE ubPlayerOffs = pPlayer->ubIdx * 32;

	if (!g_sHudManager.pPlayerCache[pPlayer->ubIdx].ubBgDrawn) {
		// czyszczenie tla
		blitRect(
			pHudBitMap,
			0, ubPlayerOffs,
			g_sHudManager.pHudVPort->uwWidth, 30,
			g_pPlayerColors[pPlayer->ubIdx][1]
		);
		ubBgFresh = 1;
		// odrysowanie ryja
		blitCopy(
			g_pCharactersBitMap, ubPlayerOffs + 2, 32 + 2,
			pHudBitMap, 1, ubPlayerOffs + 1,
			28, 28, MINTERM_COPY
		);
		g_sHudManager.pPlayerCache[pPlayer->ubIdx].ubBgDrawn = 1;
		// wymuszenie odrysowania reszty
		g_sHudManager.pPlayerCache[pPlayer->ubIdx].ubTileErased = 0;
		g_sHudManager.pPlayerCache[pPlayer->ubIdx].ubPrevPawns = 0xFF;
		g_sHudManager.pPlayerCache[pPlayer->ubIdx].wPrevScore = -1;
	}

	if (g_sHudManager.pPlayerCache[pPlayer->ubIdx].wPrevScore != pPlayer->uwScore) {
		// odrysowanie tla score'a
		if (!ubBgFresh) {
			blitRect(
				pHudBitMap,
				35, ubPlayerOffs + 7,
				130, 15,
				g_pPlayerColors[pPlayer->ubIdx][1]
			);
		}
		// odrysowanie licznika score'a
		sprintf(szBfr, "Score: %hu", pPlayer->uwScore);
		fontDrawStr(g_pFont, pHudBitMap, 35, ubPlayerOffs + 7, szBfr, COLOR_DRKGREEN, FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);
		g_sHudManager.pPlayerCache[pPlayer->ubIdx].wPrevScore = pPlayer->uwScore;
	}

	// odrysowanie tla licznika pionow
	if (g_sHudManager.pPlayerCache[pPlayer->ubIdx].ubPrevPawns != pPlayer->ubPawnsLeft) {
		if (!ubBgFresh) {
			blitRect(
				pHudBitMap,
				165, ubPlayerOffs + 7,
				125, 15,
				g_pPlayerColors[pPlayer->ubIdx][1]
			);
		}
		// odrysowanie licznika pionow
		sprintf(szBfr, "Goblins: %hhu", pPlayer->ubPawnsLeft);
		fontDrawStr(g_pFont, pHudBitMap, 165, ubPlayerOffs + 7, szBfr, COLOR_DRKGREEN, FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);
		g_sHudManager.pPlayerCache[pPlayer->ubIdx].ubPrevPawns = pPlayer->ubPawnsLeft;
	}

	// odrysowanie kafla
	if (pPlayer == g_pCurrPlayer) {
		if (g_ubTurnStep == TURN_PAWN) {
			blitCopy(
				g_pPawnsLarge, 0, 28 * pPlayer->ubIdx,
				pHudBitMap, 291, pPlayer->ubIdx * 32 + 1, 28, 28,
				MINTERM_COPY
			);
		}
		else if (g_ubTurnStep == TURN_TILE) {
			blitCopy(
				g_pTileBuffer->pTileSet, 2, 2 + g_sTileList.pTileIdx[g_sTileList.uwPos] * g_pTileBuffer->ubTileSize,
				pHudBitMap, 291, ubPlayerOffs + 1, 28, 28,
				MINTERM_COPY
			);
		}
	}
	else if (!g_sHudManager.pPlayerCache[pPlayer->ubIdx].ubTileErased || !ubBgFresh) {
		blitRect(
			pHudBitMap,
			291, ubPlayerOffs + 1,
			28, 28,
			g_pPlayerColors[pPlayer->ubIdx][1]
		);
	}
	else {
		g_sHudManager.pPlayerCache[pPlayer->ubIdx].ubTileErased = 1;
	}
}

void hudShowPlayer(const tPlayer *pPlayer) {
	UBYTE steps[20] = {0, 3, 5, 7, 10, 12, 15, 17, 19, 21, 23, 24, 26, 27, 29, 30, 30, 31, 32, 32};
	UBYTE ubStart = g_sHudManager.pCamera->uPos.uwY;
	UBYTE ubEnd = 32 * pPlayer->ubIdx;
	UBYTE i;

	for (i = 0; i != 20; ++i) {
		cameraSetCoord(g_sHudManager.pCamera, g_sHudManager.pCamera->uPos.uwX, ubStart + ((ubEnd - ubStart) * steps[i]) / 32);
		vPortProcessManagers(g_sHudManager.pHudVPort);
		copProcessBlocks();
		vPortWaitForEnd(g_sHudManager.pHudVPort);
	}
}

void hudAnimate(UBYTE ubStart, UBYTE ubStop) {
	UBYTE stepRatios[42] = {0, 0, 10, 20, 30, 40, 50, 60, 69, 79, 88, 98, 107, 116, 125, 133, 142, 150, 158, 166, 173, 180, 187, 194, 200, 206, 212, 217, 222, 227, 232, 236, 239, 243, 245, 248, 250, 252, 253, 254, 255, 255};
	UBYTE ubStep = ubStop <= ubStart ? -1 : 1;

	for (UBYTE i = ubStart; i < ubStop; i += ubStep) {
		UBYTE ubStepValue = ((((g_sGameConfig.ubPlayerCount - 1) * HUD_VPORT_HEIGHT) * stepRatios[i]) / 256);
		g_sHudManager.pMainVPort->uwHeight = 223 - ubStepValue;
		g_sHudManager.pHudVPort->uwOffsY = g_sHudManager.pMainVPort->uwHeight + 2;
		g_sHudManager.pHudVPort->uwHeight = SCREEN_PAL_HEIGHT - g_sHudManager.pHudVPort->uwOffsY;

		if (ubStepValue < (g_pCurrPlayer->ubIdx * 32)) {
			g_sHudManager.pHudVPort->uwOffsY = (g_pCurrPlayer->ubIdx * HUD_VPORT_HEIGHT) - ubStepValue;
		}
		else {
			g_sHudManager.pHudVPort->uwOffsY = 0;
		}

		viewProcessManagers(g_sHudManager.pView);
		copProcessBlocks();
		vPortWaitForEnd(g_sHudManager.pHudVPort);
	}
}

void hudExpand(void) {
	// TODO: Use mose pointer from open fire
	// mouseSetPointer(&g_pCursorData[g_ubPointerIdx * 36], 16, 16, g_pPointerSpots[g_ubPointerIdx][0], g_pPointerSpots[g_ubPointerIdx][1]);
	g_sHudManager.ubExpanded = 1;

	hudAnimate(1, 41);
	// hudAnimate(40, 41); // no anim dbg
}

void hudMinimize(void) {
	g_sHudManager.ubExpanded = 0;

	hudAnimate(40, 0);
	// hudAnimate(1, 0); // no anim dbg
}

void hudSummary(void) {
	char pPlaceStrings[8][4] = {"1st", "2nd", "3rd", "4th", "5th", "6th", "7th", "8th"};
	UWORD *pPlayerScores;
	UBYTE *pPlayerNumbers;
	UBYTE ubPlayer, ubDone, ubPlace;
	UWORD uwTmp, uwPrevScore;
	tPlayer *pTmpCurr;

	// numery graczy i score'y do sortowania
	pPlayerScores = memAllocFast(g_sGameConfig.ubPlayerCount * sizeof(pPlayerScores[0]));
	pPlayerNumbers = memAllocFast(g_sGameConfig.ubPlayerCount * sizeof(pPlayerNumbers[0]));
	// wypelnij
	pTmpCurr = g_sGameConfig.pPlayerFirst;
	for (ubPlayer = 0; ubPlayer != g_sGameConfig.ubPlayerCount; ++ubPlayer) {
		pPlayerScores[ubPlayer] = pTmpCurr->uwScore;
		pPlayerNumbers[ubPlayer] = ubPlayer;
		pTmpCurr = pTmpCurr->pNext;
	}

	// przesortuj
	do {
		ubDone = 1;
		for (ubPlayer = g_sGameConfig.ubPlayerCount; --ubPlayer;) {
			if (pPlayerScores[ubPlayer] > pPlayerScores[ubPlayer-1]) {
				ubDone = 0;
				// swap punktow
				uwTmp = pPlayerScores[ubPlayer-1];
				pPlayerScores[ubPlayer-1] = pPlayerScores[ubPlayer];
				pPlayerScores[ubPlayer] = uwTmp;
				// swap graczy
				uwTmp = pPlayerNumbers[ubPlayer-1];
				pPlayerNumbers[ubPlayer-1] = pPlayerNumbers[ubPlayer];
				pPlayerNumbers[ubPlayer] = uwTmp;
			}
		}
	} while (!ubDone);

	// Odrysuj caly HUD bez zaznaczania aktywnego gracza
	pTmpCurr = g_pCurrPlayer;
	g_pCurrPlayer = 0; // zeby nie odrysowac nigdzie kafla
	hudRedrawAll();
	g_pCurrPlayer = pTmpCurr;

	// Wypisz numery zajetych miejsc
	uwPrevScore = 0;
	ubPlace = 0;
	for (ubPlayer = 0; ubPlayer != g_sGameConfig.ubPlayerCount; ++ubPlayer) {
		// Jesli gorszy wynik to nizsze miejsce - zaatwia ex aequo
		if (uwPrevScore > pPlayerScores[ubPlayer]) {
			++ubPlace;
		}
		fontDrawStr(
			g_pFont, g_sHudManager.pSimpleBuffer->pBack,
			280, pPlayerNumbers[ubPlayer]*32+7,
			pPlaceStrings[ubPlace], COLOR_DRKGREEN, FONT_COOKIE | FONT_SHADOW,
			g_pTextBitMap
		);
		uwPrevScore = pPlayerScores[ubPlayer];
	}

	memFree(pPlayerScores, g_sGameConfig.ubPlayerCount * sizeof(pPlayerScores[0]));
	memFree(pPlayerNumbers, g_sGameConfig.ubPlayerCount * sizeof(pPlayerNumbers[0]));

	hudExpand();
}
