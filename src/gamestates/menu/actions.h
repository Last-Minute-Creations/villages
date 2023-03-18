#ifndef GUARD_GAMESTATE_MENU_ACTIONS_H
#define GUARD_GAMESTATE_MENU_ACTIONS_H

/* Types */

typedef enum _tActionState {
    ACTION_STATE_KEEP_RUNNING,
    ACTION_STATE_ADVANCE,
} tActionState;

typedef tActionState (*tActionPtr)(void);

/* Globals */

/* Functions */

tActionState actionExitGame(void);

tActionState actionMainMenuClose(void);

tActionState actionSplashPromtDraw(void);

void actionsInit(void);

void actionsProcess(void);

void actionsQueue(tActionPtr pAction);

#endif // GUARD_GAMESTATE_MENU_ACTIONS_H
