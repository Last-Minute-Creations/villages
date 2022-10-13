#ifndef GUARD_GAMESTATE_MENU_SCROLLS_H
#define GUARD_GAMESTATE_MENU_SCROLLS_H

#include <ace/utils/bitmap.h>

#include "game.h"

/* Types */

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
 * @brief Runs single animation loop update and draws possible changes, if
 * needed at all.
 */
void scrollsProcess(void);

/**
 * @brief Returns TRUE, if provided fade is at desired state. Returns FALSE
 * if not, makes step of animation to ultimately meet state requirements in
 * next game frames. It ment be called on each game loop in IF statements as
 * guard which brings neat usage.
 */
UBYTE scrollsRequestState(tScrollType eScrollType, tScrollState eScrollState);

void scrollsSetContentHeight(tScrollType eScrollType, UWORD uwContentHeight);

#endif // GUARD_GAMESTATE_MENU_SCROLLS_H
