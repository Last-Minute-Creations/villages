#ifndef GUARD_GAMESTATE_MENU_ACTIONS_H
#define GUARD_GAMESTATE_MENU_ACTIONS_H

/* Types */

/* Globals */

/* Functions */

void actionExitGame(void);

void actionsMainMenuClose(void);

void actionSplashPromtDraw(void);

void actionsInit(void);

void actionsProcess(void);

void actionsSetTo(void (*pAction)(void));

#endif // GUARD_GAMESTATE_MENU_ACTIONS_H
