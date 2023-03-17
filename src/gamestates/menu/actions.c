#include "gamestates/menu/actions.h"

#include <ace/managers/key.h>
#include <ace/managers/mouse.h>
#include <ace/managers/blit.h>

#include "game.h"
#include "fade.h"
#include "utils.h"

#include "gamestates/menu/menu.h"
#include "gamestates/menu/scrolls.h"
#include "gamestates/menu/items.h"

/* Types */

/* Globals */

/* Statics */

static void (*s_pAction)(void);

static char *s_szSplashPrompt = "Click To Continue";
static tUwCoordYX s_sSplashPromptPos = {
	.uwX = GAME_SCREEN_WIDTH >> 1,
	.uwY = GAME_SCREEN_HEIGHT - 50,
};

/* Functions */

void actionExitGame(void) {
	logWrite("actionExitGame");

	if (!fadeRequestState(FADE_STATE_OUT)) {
		return;
	}

	statePopAll(g_pStateManager);
}

void actionsMainMenuClose(void) {
	logWrite("actionsMainMenuClose");

	if (!scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_HIDDEN)) {
		return;
	}

	actionsSetTo(actionSplashPromtDraw);
}

void actionMainMenuCheckForNewActions(void) {
	if (!scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_OPEN)) {
		return;
	}

	if (keyUse(KEY_ESCAPE)) {
		actionsSetTo(actionsMainMenuClose);
	}

	menuItemMainMenuProcess();
}

void actionMainMenuStart(void) {
	logWrite("actionMainMenuStart");

	if (!scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_CLOSED)) {
		return;
	}

	menuItemMainMenuDraw();

	actionsSetTo(actionMainMenuCheckForNewActions);
}

void actionSplashPromtUndraw(void) {
	logWrite("actionSplashPromtUndraw");

	tUwCoordYX sTextSize = fontMeasureText(g_pFont, s_szSplashPrompt);
	sTextSize.uwX = ceilToFactor(sTextSize.uwX, 16);

	blitCopyAligned(
		g_pSplashBitMap, s_sSplashPromptPos.uwX - (sTextSize.uwX >> 1), s_sSplashPromptPos.uwY,
		g_pMenuBuffer->pBack, s_sSplashPromptPos.uwX - (sTextSize.uwX >> 1), s_sSplashPromptPos.uwY,
		sTextSize.uwX, sTextSize.uwY
	);

	actionsSetTo(actionMainMenuStart);
}

void actionSplashCheckForNewActions(void) {
	if (keyUse(KEY_ESCAPE)) {
		actionsSetTo(actionExitGame);
	}

	if (mouseUse(MOUSE_PORT_1, MOUSE_LMB)) {
		actionsSetTo(actionSplashPromtUndraw);
	}
}

void actionSplashPromtDraw(void) {
	logWrite("actionSplashPromtDraw");

	if (!fadeRequestState(FADE_STATE_IN)) {
		return;
	}

	fontDrawStr(
		g_pFont,
		g_pMenuBuffer->pBack,
		s_sSplashPromptPos.uwX,
		s_sSplashPromptPos.uwY,
		s_szSplashPrompt,
		30,
		FONT_HCENTER | FONT_COOKIE | FONT_SHADOW,
		g_pTextBitMap
	);

	actionsSetTo(actionSplashCheckForNewActions);
}

void actionSplashStart(void) {
	logWrite("actionSplashStart");

	blitCopyAligned(
		g_pSplashBitMap, 0, 0,
		g_pMenuBuffer->pBack, 0, 0,
		GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT
	);

	actionsSetTo(actionSplashPromtDraw);
}

void actionsInit(void) {
	actionsSetTo(actionSplashStart);
}

void actionsProcess(void) {
	s_pAction();
}

void actionsSetTo(void (*pAction)(void)) {
	s_pAction = pAction;
}
