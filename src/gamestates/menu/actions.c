#include "gamestates/menu/actions.h"

#include <ace/managers/blit.h>

#include "game.h"
#include "fade.h"
#include "utils.h"

#include "gamestates/menu/menu.h"

/* Types */

/* Globals */

/* Statics */

static tMenuAction s_eQueuedMenuAction = MENU_ACTION_NONE;
static void (*s_pMenuActions[MENU_ACTION_COUNT])(void);

static char *s_szSplashPrompt = "Click To Continue";
static tUwCoordYX s_sSplashPromptPos = {
	.uwX = GAME_SCREEN_WIDTH >> 1,
	.uwY = GAME_SCREEN_HEIGHT - 50,
};

/* Functions */

tMenuAction gsMenuGetQueuedMenuAction() {
	return s_eQueuedMenuAction;
}

void gsMenuSetQueuedMenuAction(tMenuAction eMenuAction, UBYTE ubUseFadeOut) {
	s_eQueuedMenuAction = eMenuAction;

	if (ubUseFadeOut) {
		fadeMorphTo(FADE_TARGET_OUT);
	}
}

void gsMenuCallQueuedMenuAction(void) {
	tMenuAction eMenuAction = s_eQueuedMenuAction;

	gsMenuCallMenuAction(s_eQueuedMenuAction);

	if (s_eQueuedMenuAction == eMenuAction) {
		s_eQueuedMenuAction = MENU_ACTION_NONE;
	}
}

void gsMenuCallMenuAction(tMenuAction eMenuAction) {
	s_pMenuActions[eMenuAction]();
}

void gsMenuActionExit(void) {
	statePop(g_pStateManager);
}

void gsMenuActionShowSlpash(void) {
	stateChange(&g_sMenuStateManager, &g_sMenuStateSplash);
}

void gsMenuActionDrawSplashPromt(void) {
	fontDrawStr(
		g_pFont,
		g_pMenuBuffer->pBack,
		s_sSplashPromptPos.uwX,
		s_sSplashPromptPos.uwY,
		s_szSplashPrompt,
		30,
		FONT_CENTER | FONT_COOKIE | FONT_SHADOW,
		g_pTextBitMap
	);
}

void gsMenuActionUndrawSplashPromt(void) {
	tUwCoordYX sTextSize = fontMeasureText(g_pFont, s_szSplashPrompt);
	sTextSize.uwX = ceilToFactor(sTextSize.uwX, 16);
	sTextSize.uwY = ceilToFactor(sTextSize.uwY, 16);

	blitCopyAligned(
		g_pSplashBitMap, s_sSplashPromptPos.uwX - (sTextSize.uwX >> 1), s_sSplashPromptPos.uwY,
		g_pMenuBuffer->pBack, s_sSplashPromptPos.uwX - (sTextSize.uwX >> 1), s_sSplashPromptPos.uwY,
		sTextSize.uwX, sTextSize.uwY
	);
}

void gsMenuActionShowMainMenu(void) {
	stateChange(&g_sMenuStateManager, &g_sMenuStateSplash);
}

static void (*s_pMenuActions[MENU_ACTION_COUNT])(void) = {
	[MENU_ACTION_SHOW_SLPASH] = gsMenuActionShowSlpash,
	[MENU_ACTION_DRAW_SPLASH_PROMPT] = gsMenuActionDrawSplashPromt,
	[MENU_ACTION_UNDRAW_SPLASH_PROMPT] = gsMenuActionUndrawSplashPromt,
	[MENU_ACTION_SHOW_MAIN_MENU] = gsMenuActionShowMainMenu,
	[MENU_ACTION_EXIT] = gsMenuActionExit,
};
