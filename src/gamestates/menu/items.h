#ifndef GUARD_GAMESTATE_MENU_ITEMS_H
#define GUARD_GAMESTATE_MENU_ITEMS_H

#include <ace/types.h>

/* Types */

#define MENU_ITEMS_SPACING 8
#define MENU_ITEMS_TEXT_COLOR_INDEX 8
#define MENU_ITEMS_TEXT_HOVERED_COLOR_INDEX 22

typedef enum _tMenuMainItemType {
	MENU_MAIN_ITEM_NEW_GAME,
	MENU_MAIN_ITEM_HOW_TO_PLAY,
	MENU_MAIN_ITEM_CREDITS,
	MENU_MAIN_ITEM_QUIT,
	MENU_MAIN_ITEM_COUNT,
} tMainMenuItemType;

typedef enum _tBackMenuItemType {
	MENU_BACK_ITEM_BACK,
	MENU_BACK_ITEM_COUNT,
} tBackMenuItemType;

/* Functions */

void menuItemMainMenuDraw(void);
tMainMenuItemType menuItemMainMenuProcess();

void menuItemBackMenuDraw(void);
tBackMenuItemType menuItemBackMenuProcess(void);

#endif // GUARD_GAMESTATE_MENU_ITEMS_H
