#include "cursor.h"
#include <ace/macros.h>
#include <ace/managers/mouse.h>
#include <ace/managers/log.h>
#include <ace/utils/bitmap.h>

#include "game.h"

static tBitMap *s_pCrosshair;

void cursorCreate(tView *pView, FUBYTE fubSpriteIdx, char *szPath, UWORD uwRawCopPos) {
	mouseSetBounds(MOUSE_PORT_1, 0, 0, WINDOW_SCREEN_WIDTH, WINDOW_SCREEN_HEIGHT);
	s_pCrosshair = bitmapCreateFromFile(szPath, 0);
	UWORD *pSpriteBfr = (UWORD*) ((void*) s_pCrosshair->Planes[0]);
	cursorUpdate();
	ULONG ulSprAddr = (ULONG) ((UBYTE*)pSpriteBfr);

	if(pView->pCopList->ubMode == COPPER_MODE_RAW) {
		tCopCmd *pCrossList = &pView->pCopList->pBackBfr->pList[uwRawCopPos];

		copSetMove(&pCrossList[0].sMove, &g_pSprFetch[fubSpriteIdx].uwHi, ulSprAddr >> 16);
		copSetMove(&pCrossList[1].sMove, &g_pSprFetch[fubSpriteIdx].uwLo, ulSprAddr & 0xFFFF);

		UBYTE *pSrc = (void *) &pView->pCopList->pBackBfr->pList[uwRawCopPos];
		UBYTE *pDst = (void *) &pView->pCopList->pFrontBfr->pList[uwRawCopPos];
		UBYTE ulCopySize = 2 * sizeof(tCopCmd);
		while (ulCopySize--) {
			logWrite("before: %d", *pDst);
			*pDst++ = *pSrc++;
			logWrite("after: %d", *pDst);
		}

		// CopyMemQuick(
		// 	&pView->pCopList->pBackBfr->pList[uwRawCopPos],
		// 	&pView->pCopList->pFrontBfr->pList[uwRawCopPos],
		// 	2 * sizeof(tCopCmd)
		// );
	}
	else {
		tCopBlock *pBlock = copBlockCreate(pView->pCopList, 2, 0, 0);

		copMove(pView->pCopList, pBlock, &g_pSprFetch[fubSpriteIdx].uwHi, ulSprAddr >> 16);
		copMove(pView->pCopList, pBlock, &g_pSprFetch[fubSpriteIdx].uwLo, ulSprAddr & 0xFFFF);
	}
}

void cursorDestroy(void) {
	bitmapDestroy(s_pCrosshair);
	// CopBlock will be freed with whole copperlist
}

void cursorUpdate(void) {
	// Destination angle from mouse
	UWORD uwMouseX = mouseGetX(MOUSE_PORT_1);
	UWORD uwMouseY = mouseGetY(MOUSE_PORT_1);

	const UWORD uwCrossHeight = 11;
	UWORD uwVStart = 0x2B - 4 + uwMouseY;
	UWORD uwVStop = uwVStart + uwCrossHeight;

	UWORD *pSpriteBfr = (UWORD*) ((void*)s_pCrosshair->Planes[0]);
	pSpriteBfr[0] = (UWORD) ((uwVStart << 8) | (64 - (4 >> 1) + (uwMouseX >> 1)));
	pSpriteBfr[1] = (UWORD) (
		(uwVStop << 8) |
		(((uwVStart & (1 << 8)) >> 8) << 2) |
		(((uwVStop  & (1 << 8)) >> 8) << 1) |
		(uwMouseX & 1)
	);
}
