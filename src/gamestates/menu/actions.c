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

tActionState actionExitGame(void) {
	logWrite("actionExitGame");

	if (!fadeRequestState(FADE_STATE_OUT)) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	statePopAll(g_pStateManager);

	return ACTION_STATE_ADVANCE;
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

tActionState actionMainMenuCheckForNewActions(void) {
	if (!scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_OPEN)) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	if (keyUse(KEY_ESCAPE)) {
		actionsQueue(actionHideRightScroll);
		actionsQueue(actionSplashPromtDraw);

		return ACTION_STATE_ADVANCE;
	}

	switch (menuItemMainMenuProcess()) {
		case 0: // New game
			actionsQueue(actionCloseRightScroll);
			actionsQueue(actionMainMenuCheckForNewActions);
			return ACTION_STATE_ADVANCE;
		case 1: // How to play?
			actionsQueue(actionCloseRightScroll);
			actionsQueue(actionMainMenuCheckForNewActions);
			return ACTION_STATE_ADVANCE;
		case 2: // Credits
			actionsQueue(actionCloseRightScroll);
			actionsQueue(actionMainMenuCheckForNewActions);
			return ACTION_STATE_ADVANCE;
		case 3: // Quit
			actionsQueue(actionHideRightScroll);
			actionsQueue(actionExitGame);
			return ACTION_STATE_ADVANCE;
	}

	return ACTION_STATE_KEEP_RUNNING;
}

tActionState actionMainMenuStart(void) {
	logWrite("actionMainMenuStart");

	if (!scrollsRequestState(SCROLL_RIGHT, SCROLL_STATE_CLOSED)) {
		return ACTION_STATE_KEEP_RUNNING;
	}

	menuItemMainMenuDraw();

	return ACTION_STATE_ADVANCE;
}

tActionState actionSplashPromtUndraw(void) {
	logWrite("actionSplashPromtUndraw");

	tUwCoordYX sTextSize = fontMeasureText(g_pFont, s_szSplashPrompt);
	sTextSize.uwX = ceilToFactor(sTextSize.uwX, 16);

	blitCopyAligned(
		g_pSplashBitMap, s_sSplashPromptPos.uwX - (sTextSize.uwX >> 1), s_sSplashPromptPos.uwY,
		g_pMenuBuffer->pBack, s_sSplashPromptPos.uwX - (sTextSize.uwX >> 1), s_sSplashPromptPos.uwY,
		sTextSize.uwX, sTextSize.uwY
	);

	return ACTION_STATE_ADVANCE;
}

tActionState actionSplashCheckForNewActions(void) {
	if (keyUse(KEY_ESCAPE)) {
		actionsQueue(actionExitGame);
		return ACTION_STATE_ADVANCE;
	}

	if (mouseUse(MOUSE_PORT_1, MOUSE_LMB)) {
		actionsQueue(actionSplashPromtUndraw);
		actionsQueue(actionMainMenuStart);
		actionsQueue(actionMainMenuCheckForNewActions);
		return ACTION_STATE_ADVANCE;
	}

	return ACTION_STATE_KEEP_RUNNING;
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

tActionState actionSplashStart(void) {
	logWrite("actionSplashStart");

	blitCopyAligned(
		g_pSplashBitMap, 0, 0,
		g_pMenuBuffer->pBack, 0, 0,
		GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT
	);

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
