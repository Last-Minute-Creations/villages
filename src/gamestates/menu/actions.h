#ifndef GUARD_GAMESTATE_MENU_ACTIONS_H
#define GUARD_GAMESTATE_MENU_ACTIONS_H

#include <ace/types.h>

/* Types */

typedef enum _tMenuAction {
    MENU_ACTION_NONE,
    MENU_ACTION_SHOW_SLPASH,
    MENU_ACTION_DRAW_SPLASH_PROMPT,
    MENU_ACTION_UNDRAW_SPLASH_PROMPT,
    MENU_ACTION_SHOW_MAIN_MENU,
    MENU_ACTION_EXIT,
    MENU_ACTION_COUNT,
} tMenuAction;

/* Globals */

/* Functions */

tMenuAction gsMenuGetQueuedMenuAction(void);

void gsMenuSetQueuedMenuAction(tMenuAction eMenuAction, UBYTE ubUseFadeOut);

void gsMenuCallQueuedMenuAction(void);

void gsMenuCallMenuAction(tMenuAction eMenuAction);

#endif // GUARD_GAMESTATE_MENU_ACTIONS_H
