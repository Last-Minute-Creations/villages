#include "menu.h"

#include <ace/generic/screen.h>

#include <ace/managers/game.h>
#include <ace/managers/key.h>
#include <ace/managers/mouse.h>
#include <ace/managers/timer.h>
#include <ace/managers/log.h>
#include <ace/managers/viewport/simplebuffer.h>
#include <ace/managers/blit.h>
#include <ace/managers/system.h>

#include <ace/utils/font.h>
#include <ace/utils/palette.h>

#include "config.h"
#include "global.h"
#include "cursor.h"

#include "gamestates/arcade/arcade.h"

/* Globals */
static tView *s_pMenuView;
static tVPort *s_pMenuVPort;
static tSimpleBufferManager *s_pMenuBuffer;
static tBitMap *s_pSplashBitMap;
static tBitMap *s_pGrassBitMap;
static tState s_sStateMenuSplash;
static tState s_sStateMenuLobby;

/* Functions */
void gsMenuCreate(void) {
	logBlockBegin("gsMenuCreate()");

	s_pMenuView = viewCreate(
		NULL,
		TAG_VIEW_GLOBAL_CLUT, 1,
		TAG_DONE
	);
	s_pMenuVPort = vPortCreate(
		NULL,
		TAG_VPORT_VIEW, s_pMenuView,
		TAG_VPORT_BPP, GAME_BPP,
		TAG_DONE
	);
	s_pMenuBuffer = simpleBufferCreate(
		NULL,
		TAG_SIMPLEBUFFER_VPORT, s_pMenuVPort,
		TAG_DONE
	);

	paletteLoad("data/palettes/selur_v1.plt", s_pMenuVPort->pPalette, 32);

	cursorCreate(s_pMenuView, 0, "data/hand.bm", 0);

	s_pGrassBitMap = bitmapCreateFromFile("data/bitmaps/grass.bm", FALSE);

	// Splash
	s_pSplashBitMap = bitmapCreateFromFile("data/bitmaps/splash.bm", FALSE);

	fontDrawStr(g_pFont, s_pSplashBitMap, SCREEN_PAL_WIDTH >> 1, 180, "Click to start", COLOR_DRKGREEN, FONT_CENTER | FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);
	fontDrawStr(g_pFont, s_pSplashBitMap, SCREEN_PAL_WIDTH >> 1, 215, "All rights reserved 2014", COLOR_DRKGREEN, FONT_CENTER | FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);
	fontDrawStr(g_pFont, s_pSplashBitMap, SCREEN_PAL_WIDTH >> 1, 230, "KaiN, Proxy, Selur, JnR", COLOR_DRKGREEN, FONT_CENTER | FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);

	copBlockDisableSprites(s_pMenuView->pCopList, 0xFE);
	systemSetDmaBit(DMAB_SPRITE, TRUE);
	viewLoad(s_pMenuView);

	logBlockEnd("gsMenuCreate()");

	systemUnuse();

	statePush(g_pGameStateManager, &s_sStateMenuSplash);
}

void gsMenuSplashSetup(void) {
	// blitCopyAligned(
		// s_pSplashBitMap, 0, 0,
		// s_pMenuBuffer->pBack, 0, 0,
		// SCREEN_PAL_WIDTH, SCREEN_PAL_HEIGHT
	// );
	blitCopy(
		s_pSplashBitMap, 0, 0,
		s_pMenuBuffer->pBack, 0, 0,
		SCREEN_PAL_WIDTH, SCREEN_PAL_HEIGHT, MINTERM_COPY
	);
	// FIXME: Pointer from open fire
	// mouseSetPointer(g_sMouseManager.pBlankCursor, 1, 16, 0, 0);
}

void gsMenuSplashLoop(void) {
	cursorUpdate();

	if (keyUse(KEY_ESCAPE)) {
		statePopAll(g_pGameStateManager);
		return;
	}

	if (mouseUse(MOUSE_PORT_1, MOUSE_LMB)) {
		// TODO: Use paletteDim
		// extViewFadeOut(s_pMenuView);
		stateChange(g_pGameStateManager, &s_sStateMenuLobby);
		// extViewFadeIn(s_pMenuView);
	}

	viewProcessManagers(s_pMenuView);
	copProcessBlocks();
	vPortWaitForEnd(s_pMenuVPort);
}

void gsMenuLobbySetup(void) {
	gsMenuDrawBg();
	fontDrawStr(g_pFont, s_pMenuBuffer->pBack, 160, 50, "Players", COLOR_DRKGREEN, FONT_CENTER | FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);

	UBYTE ubPlayer = PLAYER_COUNT;
	while (ubPlayer--) {
		blitCopyAligned(
			g_pCharactersBitMap, (32 * ubPlayer), 32 * g_sGameConfig.pPlayersSelected[ubPlayer],
			s_pMenuBuffer->pBack, 32 + (32 * ubPlayer), 70,
			32, 32
		);
	}

	fontDrawStr(g_pFont, s_pMenuBuffer->pBack, 64, 120, "Game mode", COLOR_DRKGREEN, FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);
	fontDrawStr(g_pFont, s_pMenuBuffer->pBack, SCREEN_PAL_WIDTH - 115, 120, "[                 ]", COLOR_DRKGREEN, FONT_HCENTER | FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);
	fontDrawStr(g_pFont, s_pMenuBuffer->pBack, SCREEN_PAL_WIDTH - 115, 120, "classic", COLOR_DRKGREEN, FONT_HCENTER | FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);

	fontDrawStr(g_pFont, s_pMenuBuffer->pBack, 64, 140, "Pawn count", COLOR_DRKGREEN, FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);
	fontDrawStr(g_pFont, s_pMenuBuffer->pBack, SCREEN_PAL_WIDTH - 115, 140, "[                 ]", COLOR_DRKGREEN, FONT_HCENTER | FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);
	char szPawnCount[3];
	sprintf(szPawnCount, "%hhu", g_sGameConfig.ubPawnsPerPlayer);
	fontDrawStr(g_pFont, s_pMenuBuffer->pBack, SCREEN_PAL_WIDTH - 115, 140, szPawnCount, COLOR_DRKGREEN, FONT_HCENTER | FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);

	fontDrawStr(g_pFont, s_pMenuBuffer->pBack, 64, 160, "Tile set", COLOR_DRKGREEN, FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);
	fontDrawStr(g_pFont, s_pMenuBuffer->pBack, SCREEN_PAL_WIDTH - 115, 160, "[                 ]", COLOR_DRKGREEN, FONT_HCENTER | FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);
	fontDrawStr(g_pFont, s_pMenuBuffer->pBack, SCREEN_PAL_WIDTH - 115, 160, "goblins", COLOR_DRKGREEN, FONT_HCENTER | FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);

	fontDrawStr(g_pFont, s_pMenuBuffer->pBack, SCREEN_PAL_WIDTH / 2, 200, "[       play       ]", COLOR_DRKGREEN, FONT_HCENTER | FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);
}

void gsMenuLobbyLoop(void) {
	cursorUpdate();

	if (keyUse(KEY_ESCAPE)) {
		// TODO: Use paletteDim
		// extViewFadeOut(s_pMenuView);
		stateChange(g_pGameStateManager, &s_sStateMenuSplash);
		// extViewFadeIn(s_pMenuView);

		return;
	}

	if (mouseUse(MOUSE_PORT_1, MOUSE_LMB)) {
		UBYTE ubPlayer = PLAYER_COUNT;
		while (ubPlayer--) {
			if (mouseInRect(MOUSE_PORT_1, (tUwRect) {
				.uwX = 32 + (32 * ubPlayer),
				.uwY = 70,
				.uwWidth = 32,
				.uwHeight = 32
			})) { // Character
				gsMenuCharacterAction(ubPlayer);

				return;
			}
		}

		if (mouseInRect(MOUSE_PORT_1, (tUwRect) {
			.uwX = SCREEN_PAL_WIDTH - 115 - (95 / 2),
			.uwY = 140,
			.uwWidth = 95,
			.uwHeight = 15
		}) && (g_sGameConfig.ubPawnsPerPlayer < 16)) { // Pawns
			blitCopyAligned(
				s_pMenuBuffer->pBack, 176, 140 % 32,
				s_pMenuBuffer->pBack, 176, 140,
				64, 16
			);

			g_sGameConfig.ubPawnsPerPlayer += 1;
			char szPawnCount[3];
			sprintf(szPawnCount, "%hhu", g_sGameConfig.ubPawnsPerPlayer);
			fontDrawStr(g_pFont, s_pMenuBuffer->pBack, SCREEN_PAL_WIDTH - 115, 140, szPawnCount, COLOR_DRKGREEN, FONT_HCENTER | FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);
		}
		else if (mouseInRect(MOUSE_PORT_1, (tUwRect) {
			.uwX = (SCREEN_PAL_WIDTH >> 1) - (109 / 2),
			.uwY = 200,
			.uwWidth = 109,
			.uwHeight = 15
		})) { // Play
			gsMenuPlayAction();

			return;
		}
	}

	if (mouseUse(MOUSE_PORT_1, MOUSE_RMB)) {
		if (mouseInRect(MOUSE_PORT_1, (tUwRect) {
			.uwX = SCREEN_PAL_WIDTH - 115 - (95 / 2),
			.uwY = 140,
			.uwWidth = 95,
			.uwHeight = 15
		}) && (7 < g_sGameConfig.ubPawnsPerPlayer)) { // Pawns
			blitCopyAligned(
				s_pMenuBuffer->pBack, 176, 140 % 32,
				s_pMenuBuffer->pBack, 176, 140,
				64, 16
			);

			char szPawnCount[3];
			g_sGameConfig.ubPawnsPerPlayer -= 1;
			sprintf(szPawnCount, "%hhu", g_sGameConfig.ubPawnsPerPlayer);
			fontDrawStr(g_pFont, s_pMenuBuffer->pBack, SCREEN_PAL_WIDTH - 115, 140, szPawnCount, COLOR_DRKGREEN, FONT_HCENTER | FONT_COOKIE | FONT_SHADOW, g_pTextBitMap);
		}
	}

	viewProcessManagers(s_pMenuView);
	copProcessBlocks();
	vPortWaitForEnd(s_pMenuVPort);
}

void gsMenuDestroy(void) {
	systemUse();

	logBlockBegin("gsMenuDestroy()");

	bitmapDestroy(s_pGrassBitMap);
	bitmapDestroy(s_pSplashBitMap);

	cursorDestroy();

	viewDestroy(s_pMenuView);

	logBlockEnd("gsMenuDestroy()");
}

void gsMenuDrawBg(void) {
	UWORD uwX, uwY;
	// pierwszy wiersz
	for (uwX = 0; uwX < SCREEN_PAL_WIDTH; uwX += 32) {
		blitCopyAligned(
			s_pGrassBitMap, 0, 0,
			s_pMenuBuffer->pBack, uwX, 0,
			32, 32
		);
	}
	// blitowanie wierszami
	for (uwY = 0; uwY < SCREEN_PAL_HEIGHT; uwY += 32) {
		blitCopyAligned(
			s_pMenuBuffer->pBack, 0, 0,
			s_pMenuBuffer->pBack, 0, uwY,
			SCREEN_PAL_WIDTH, 32
		);
	}
}

void gsMenuCharacterAction(UBYTE ubPlayer) {
	if (g_sGameConfig.pPlayersSelected[ubPlayer]) {
		if (2 < g_sGameConfig.ubPlayerCount) {
			g_sGameConfig.pPlayersSelected[ubPlayer] = 0;
			--g_sGameConfig.ubPlayerCount;

			blitCopyAligned(
				g_pCharactersBitMap, (32 * ubPlayer), 0,
				s_pMenuBuffer->pBack, 32 + (32 * ubPlayer), 70,
				32, 32
			);
		}
	}
	else {
		g_sGameConfig.pPlayersSelected[ubPlayer] = 1;
		++g_sGameConfig.ubPlayerCount;

		blitCopyAligned(
			g_pCharactersBitMap, (32 * ubPlayer), 32,
			s_pMenuBuffer->pBack, 32 + (32 * ubPlayer), 70,
			32, 32
		);
	}
}

void gsMenuPlayAction(void) {
	statePop(g_pGameStateManager);
	stateChange(g_pGameStateManager, &g_sStateArcade);
}

void gsMenuHowToAction(void) {
	// gameLoadState(gsHowToCreate, gsHowToLoop, gsHowToDestroy);
}

tState g_sStateMenu = {
	.cbCreate = gsMenuCreate,
	.cbDestroy = gsMenuDestroy,
};

static tState s_sStateMenuSplash = {
	.cbCreate = gsMenuSplashSetup,
	.cbLoop = gsMenuSplashLoop,
};

static tState s_sStateMenuLobby = {
	.cbCreate = gsMenuLobbySetup,
	.cbLoop = gsMenuLobbyLoop,
};
