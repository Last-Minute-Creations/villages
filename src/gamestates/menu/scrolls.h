#ifndef GUARD_GAMESTATE_MENU_SCROLLS_H
#define GUARD_GAMESTATE_MENU_SCROLLS_H

#include <ace/utils/bitmap.h>

#include "game.h"

/* Types */

#define SCROLLS_MARGIN 16
#define SCROLLS_X_SPEED 4
#define SCROLLS_Y_SPEED 4

#define SCROLL_LEFT_WIDTH 144
#define SCROLL_RIGHT_WIDTH 128

#define SCROLL_EDGE_WIDTH 16
#define SCROLL_EDGE_HEIGHT 192

#define SCROLL_BAR_HEIGHT 16

#define SCROLL_BAR_TOP_OPEN_Y_POS SCROLLS_MARGIN
#define SCROLL_BAR_TOP_CLOSED_Y_POS ((GAME_SCREEN_HEIGHT / 2) - SCROLL_BAR_HEIGHT)

#define SCROLL_BAR_BOTTOM_OPEN_Y_POS (GAME_SCREEN_HEIGHT - SCROLLS_MARGIN - SCROLL_BAR_HEIGHT)
#define SCROLL_BAR_BOTTOM_CLOSED_Y_POS (GAME_SCREEN_HEIGHT / 2)

#define SCROLL_CONTENT_COLOR_INDEX 19
#define SCROLL_CONTENT_TO_SCREEN_Y_OFFSET (SCROLL_BAR_TOP_OPEN_Y_POS + SCROLL_BAR_HEIGHT)

#define SCROLL_LEFT_CONTENT_WIDTH (SCROLL_LEFT_WIDTH - (SCROLL_EDGE_WIDTH * 2))
// FIXME: Rollback after https://github.com/AmigaPorts/ACE/issues/162 is fixed
// #define SCROLL_LEFT_HIDDEN_X_POS -SCROLL_LEFT_WIDTH
#define SCROLL_LEFT_HIDDEN_X_POS GAME_SCREEN_WIDTH
#define SCROLL_LEFT_OPEN_X_POS SCROLLS_MARGIN

#define SCROLL_RIGHT_CONTENT_WIDTH (SCROLL_RIGHT_WIDTH - (SCROLL_EDGE_WIDTH * 2))
#define SCROLL_RIGHT_HIDDEN_X_POS GAME_SCREEN_WIDTH
#define SCROLL_RIGHT_OPEN_X_POS (GAME_SCREEN_WIDTH - SCROLLS_MARGIN - SCROLL_RIGHT_WIDTH)

typedef enum _tScrollType {
    SCROLL_LEFT,
    SCROLL_RIGHT,
    SCROLL_COUNT,
} tScrollType;

typedef enum _tScrollState {
    SCROLL_STATE_HIDDEN,
    SCROLL_STATE_CLOSED,
    SCROLL_STATE_OPEN,
} tScrollState;

/* Globals */

extern tBitMap *g_ppScrollsContentBitMap[SCROLL_COUNT];

/* Functions */

void scrollsCreate(void);

void scrollsDestroy(void);

/**
 * @brief Returns TRUE, if provided fade is at desired state. Returns FALSE
 * if not, makes step of animation to ultimately meet state requirements in
 * next game frames. It ment be called on each game loop in IF statements as
 * guard which brings neat usage.
 */
UBYTE scrollsRequestState(tScrollType eScrollType, tScrollState eScrollState);

UWORD scrollsGetContentHeight(tScrollType eScrollType);

void scrollsSetContentHeight(tScrollType eScrollType, UWORD uwContentHeight);

UWORD scrollsGetContentWidth(tScrollType eScrollType);

void scrollsDrawContentBaground(tScrollType eScrollType);

tUwCoordYX scrollsCoordsFromLocalToGlobal(tScrollType eScrollType, tUwCoordYX sLocalCoords);

tUwCoordYX scrollsCoordsFromGlobalToLocal(tScrollType eScrollType, tUwCoordYX sGlobalCoords);

#endif // GUARD_GAMESTATE_MENU_SCROLLS_H
