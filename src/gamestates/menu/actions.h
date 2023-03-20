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

tActionState actionSplashStart(void);
tActionState actionSplashPromtDraw(void);
tActionState actionSplashCheckForNewActions(void);
tActionState actionSplashStop(void);
tActionState actionMainMenuStart(void);
tActionState actionMainMenuCheckForNewActions(void);
tActionState actionCreditsMenuStart(void);
tActionState actionCreditsMenuCheckForNewActions(void);
tActionState actionCreditsMenuStop(void);
tActionState actionHowToPlayMenuStart(void);
tActionState actionHowToPlayMenuCheckForNewActions(void);
tActionState actionHideRightScroll(void);
tActionState actionCloseRightScroll(void);
tActionState actionExitGame(void);

void actionsInit(void);

void actionsProcess(void);

void actionsQueue(tActionPtr pAction);

#endif // GUARD_GAMESTATE_MENU_ACTIONS_H
