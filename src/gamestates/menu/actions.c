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

#define ACTION_QUEUE_MAX_SIZE 8
#define ACTION_QUEUE_EMPTY -1

/* Globals */

/* Statics */

// Simple queue implementation, where with ony 1 item, head and tail are at the same index
static tActionPtr s_pActionsQueue[ACTION_QUEUE_MAX_SIZE] = {};
// Empty queue is marked as a special value outside of [0 to max size]
static BYTE s_bActionsQueueHeadIndex = ACTION_QUEUE_EMPTY;
static BYTE s_bActionsQueueTailIndex = ACTION_QUEUE_EMPTY;

static char *s_szSplashPrompt = "Click To Continue";

static tUwCoordYX s_sSplashPromptPos = {
	.uwX = GAME_SCREEN_WIDTH >> 1,
	.uwY = GAME_SCREEN_HEIGHT - 50,
};

/* Functions */

tActionState actionSplashStart(void) {
	logWrite("actionSplashStart");

	blitCopyAligned(
		g_pSplashBitMap, 0, 0,
		g_pMenuBuffer->pBack, 0, 0,
		GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT
	);

	return ACTION_STATE_ADVANCE;
}

tActionState actionSplashPromtDraw(void) {
	logWrite("actionSplashPromtDraw");

	if (!fadeRequestState(FADE_STATE_IN)) {
		return ACTION_STATE_KEEP_RUNNING;
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

	return ACTION_STATE_ADVANCE;
}

tActionState actionSplashCheckForNewActions(void) {
	if (keyUse(KEY_ESCAPE)) {
		actionsQueue(actionExitGame);
		return ACTION_STATE_ADVANCE;
	}

	if (mouseUse(MOUSE_PORT_1, MOUSE_LMB)) {
		actionsQueue(actionSplashStop);
		actionsQueue(actionMainMenuStart);
		actionsQueue(actionMainMenuCheckForNewActions);
		return ACTION_STATE_ADVANCE;
	}

	return ACTION_STATE_KEEP_RUNNING;
}

tActionState actionSplashStop(void) {
	logWrite("actionSplashStop");

	tUwCoordYX sTextSize = fontMeasureText(g_pFont, s_szSplashPrompt);
	sTextSize.uwX = ceilToFactor(sTextSize.uwX, 16);

	blitCopyAligned(
		g_pSplashBitMap, s_sSplashPromptPos.uwX - (sTextSize.uwX >> 1), s_sSplashPromptPos.uwY,
		g_pMenuBuffer->pBack, s_sSplashPromptPos.uwX - (sTextSize.uwX >> 1), s_sSplashPromptPos.uwY,
		sTextSize.uwX, sTextSize.uwY
	);

	return ACTION_STATE_ADVANCE;
}

tActionState actionMainMenuStart(void) {
	logWrite("actionMainMenuStart");

	if (!scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_CLOSED)) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	menuItemMainMenuDraw();

	return ACTION_STATE_ADVANCE;
}

tActionState actionMainMenuCheckForNewActions(void) {
	if (!scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_OPEN)) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	if (keyUse(KEY_ESCAPE)) {
		actionsQueue(actionHideRightScroll);
		actionsQueue(actionSplashPromtDraw);
		actionsQueue(actionSplashCheckForNewActions);

		return ACTION_STATE_ADVANCE;
	}

	switch (menuItemMainMenuProcess()) {
		case MENU_MAIN_ITEM_NEW_GAME:
			actionsQueue(actionMainMenuStart);
			actionsQueue(actionMainMenuCheckForNewActions);
			return ACTION_STATE_ADVANCE;
		case MENU_MAIN_ITEM_HOW_TO_PLAY:
			actionsQueue(actionHowToPlayMenuStart);
			actionsQueue(actionHowToPlayMenuCheckForNewActions);
			return ACTION_STATE_ADVANCE;
		case MENU_MAIN_ITEM_CREDITS:
			actionsQueue(actionCreditsMenuStart);
			actionsQueue(actionCreditsMenuCheckForNewActions);
			return ACTION_STATE_ADVANCE;
		case MENU_MAIN_ITEM_QUIT:
			actionsQueue(actionHideRightScroll);
			actionsQueue(actionExitGame);
			return ACTION_STATE_ADVANCE;
		default:
			break;
	}

	return ACTION_STATE_KEEP_RUNNING;
}

tActionState actionCreditsMenuStart(void) {
	logWrite("actionCreditsMenuStart");

	if (!scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_HIDDEN)) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	if (!scrollsRequestState(SCROLL_LEFT, SCROLL_STATE_CLOSED)) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	// Handle right scroll
	{
		menuItemBackMenuDraw();
	}

	// Handle left scroll
	{
		UBYTE ubMenuItemsCount = 5;
		UWORD uwScrollContentHeight = ((g_pFont->uwHeight + MENU_ITEMS_SPACING) * ubMenuItemsCount) + MENU_ITEMS_SPACING;
		scrollsSetContentHeight(SCROLL_LEFT, uwScrollContentHeight);
		scrollsDrawContentBaground(SCROLL_LEFT);

		fontDrawStr(
			g_pFont, g_ppScrollsContentBitMap[SCROLL_LEFT],
			scrollsGetContentWidth(SCROLL_LEFT) / 2,
			MENU_ITEMS_SPACING + (SCROLL_EDGE_HEIGHT / 2) - (uwScrollContentHeight / 2) + ((g_pFont->uwHeight + MENU_ITEMS_SPACING) * 0),
			"Code: Proxy",
			MENU_ITEMS_TEXT_COLOR_INDEX,
			FONT_HCENTER | FONT_COOKIE,
			g_pTextBitMap
		);
		fontDrawStr(
			g_pFont, g_ppScrollsContentBitMap[SCROLL_LEFT],
			scrollsGetContentWidth(SCROLL_LEFT) / 2,
			MENU_ITEMS_SPACING + (SCROLL_EDGE_HEIGHT / 2) - (uwScrollContentHeight / 2) + ((g_pFont->uwHeight + MENU_ITEMS_SPACING) * 1),
			"GFX: Softiron",
			MENU_ITEMS_TEXT_COLOR_INDEX,
			FONT_HCENTER | FONT_COOKIE,
			g_pTextBitMap
		);
		fontDrawStr(
			g_pFont, g_ppScrollsContentBitMap[SCROLL_LEFT],
			scrollsGetContentWidth(SCROLL_LEFT) / 2,
			MENU_ITEMS_SPACING + (SCROLL_EDGE_HEIGHT / 2) - (uwScrollContentHeight / 2) + ((g_pFont->uwHeight + MENU_ITEMS_SPACING) * 2),
			"SFX: Luc3k",
			MENU_ITEMS_TEXT_COLOR_INDEX,
			FONT_HCENTER | FONT_COOKIE,
			g_pTextBitMap
		);
		fontDrawStr(
			g_pFont, g_ppScrollsContentBitMap[SCROLL_LEFT],
			scrollsGetContentWidth(SCROLL_LEFT) / 2,
			MENU_ITEMS_SPACING + (SCROLL_EDGE_HEIGHT / 2) - (uwScrollContentHeight / 2) + ((g_pFont->uwHeight + MENU_ITEMS_SPACING) * 4),
			"THX: KaiN",
			MENU_ITEMS_TEXT_COLOR_INDEX,
			FONT_HCENTER | FONT_COOKIE,
			g_pTextBitMap
		);
	}

	return ACTION_STATE_ADVANCE;
}

tActionState actionCreditsMenuCheckForNewActions(void) {
	UBYTE ubLeftScrollState = scrollsRequestState(SCROLL_LEFT, SCROLL_STATE_OPEN);
	UBYTE ubRightScrollState = scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_OPEN);
	if (!ubLeftScrollState || !ubRightScrollState) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	if (keyUse(KEY_ESCAPE)) {
		actionsQueue(actionCreditsMenuStop);
		actionsQueue(actionMainMenuStart);
		actionsQueue(actionMainMenuCheckForNewActions);

		return ACTION_STATE_ADVANCE;
	}

	switch (menuItemBackMenuProcess()) {
		case MENU_BACK_ITEM_BACK:
			actionsQueue(actionCreditsMenuStop);
			actionsQueue(actionMainMenuStart);
			actionsQueue(actionMainMenuCheckForNewActions);
			return ACTION_STATE_ADVANCE;
		default:
			break;
	}

	return ACTION_STATE_KEEP_RUNNING;
}

tActionState actionCreditsMenuStop(void) {
	logWrite("actionCreditsMenuStop");

	UBYTE ubLeftScrollState = scrollsRequestState(SCROLL_LEFT, SCROLL_STATE_HIDDEN);
	UBYTE ubRightScrollState = scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_HIDDEN);
	if (!ubLeftScrollState || !ubRightScrollState) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	return ACTION_STATE_ADVANCE;
}

tActionState actionHowToPlayMenuStart(void) {
	logWrite("actionHowToPlayMenuStart");

	if (!scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_CLOSED)) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	menuItemBackMenuDraw();

	return ACTION_STATE_ADVANCE;
}

tActionState actionHowToPlayMenuCheckForNewActions(void) {
	if (!scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_OPEN)) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	if (keyUse(KEY_ESCAPE)) {
		actionsQueue(actionMainMenuStart);
		actionsQueue(actionMainMenuCheckForNewActions);

		return ACTION_STATE_ADVANCE;
	}

	switch (menuItemBackMenuProcess()) {
		case MENU_BACK_ITEM_BACK:
			actionsQueue(actionMainMenuStart);
			actionsQueue(actionMainMenuCheckForNewActions);
			return ACTION_STATE_ADVANCE;
		default:
			break;
	}

	return ACTION_STATE_KEEP_RUNNING;
}

tActionState actionHideRightScroll(void) {
	logWrite("actionHideRightScroll");

	if (!scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_HIDDEN)) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	return ACTION_STATE_ADVANCE;
}

tActionState actionCloseRightScroll(void) {
	logWrite("actionCloseRightScroll");

	if (!scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_CLOSED)) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	return ACTION_STATE_ADVANCE;
}

tActionState actionExitGame(void) {
	logWrite("actionExitGame");

	if (!fadeRequestState(FADE_STATE_OUT)) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	statePopAll(g_pStateManager);

	return ACTION_STATE_ADVANCE;
}

void actionsInit(void) {
	logWrite("actionsInit");

	actionsQueue(actionSplashStart);
	actionsQueue(actionSplashPromtDraw);
	actionsQueue(actionSplashCheckForNewActions);
}

void actionsProcess(void) {
	if (s_bActionsQueueHeadIndex == ACTION_QUEUE_EMPTY) {
		// On empty queue we have nothing to do
		return;
	}

	if (s_pActionsQueue[s_bActionsQueueHeadIndex]() == ACTION_STATE_KEEP_RUNNING) {
		// Action just told us to just keep running it, nothing more to do
		return;
	}

	// At this point, action told us to advance to next action in queue

	if (s_bActionsQueueHeadIndex == s_bActionsQueueTailIndex) {
		// As head and tail are at the same index, we have nothing left in queue
		s_bActionsQueueHeadIndex = ACTION_QUEUE_EMPTY;
		s_bActionsQueueTailIndex = ACTION_QUEUE_EMPTY;
	}
	else {
		// As head and tail are not at the same index, lets advance the index with wrapping value around the size bounds
		s_bActionsQueueHeadIndex = (s_bActionsQueueHeadIndex + 1) % ACTION_QUEUE_MAX_SIZE;
	}
}

void actionsQueue(tActionPtr pAction) {
	// As we want to put new action on queue, lets check what we can find on advanced index with wrapping value around the size bounds
	BYTE bNextTailIndex = (s_bActionsQueueTailIndex + 1) % ACTION_QUEUE_MAX_SIZE;

	if (bNextTailIndex == s_bActionsQueueHeadIndex) {
		// We've reached queue capacity, ignore the new action
		logWrite("ERR: Action queue overflow! Ignoring.");
		return;
	}

	s_bActionsQueueTailIndex = bNextTailIndex;
	s_pActionsQueue[s_bActionsQueueTailIndex] = pAction;

	// In case when queue was empty, tell the head index to start from the same place
	if (s_bActionsQueueHeadIndex == ACTION_QUEUE_EMPTY) {
		s_bActionsQueueHeadIndex = s_bActionsQueueTailIndex;
	}
}
