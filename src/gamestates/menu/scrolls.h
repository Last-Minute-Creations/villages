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

typedef enum _tScrollStage {
    SCROLL_STAGE_HIDDEN,
    SCROLL_STAGE_CLOSED,
    SCROLL_STAGE_OPEN,
} tScrollStage;

/* Globals */

/* Functions */

void scrollsCreate(void);

void scrollsDestroy(void);

/**
 * @brief Runs single animation loop update and draws possible changes, if
 * needed at all.
 */
void scrollsProcess(void);

/**
 * @brief Returns TRUE, if provided scroll is at desired stage. Returns FALSE
 * if not, but also starts the animation loop flags for scrollsProcess to
 * ultimately meet stage requirements in next game frames. It ment be called on
 * each game loop in IF statements as guard which brings neat usage.
 */
BYTE scrollsRequestStage(tScrollType eScrollType, tScrollStage eScrollStage);

// FIXME: Remove as it will be not used in final form
void scrollsDrawAll(void);

#endif // GUARD_GAMESTATE_MENU_SCROLLS_H
