#include "gamestates/menu/items.h"

#include <ace/managers/mouse.h>

#include "debug.h"

#include "gamestates/menu/actions.h"
#include "gamestates/menu/scrolls.h"
#include "gamestates/menu/menu.h"

/* Types */

#define MENU_ITEMS_SPACING 8

typedef struct _tMenuItem {
	char *szText;
	void (*pAction)(void);
	tUwRect sButtonRect;
	UBYTE ubIsHovered;
} tMenuItem;

/* Globals */

/* Statics */

tMenuItem s_pMainMenuItems[] = {
	{
		.szText = "New Game",
		.pAction = actionsMainMenuClose,
		.sButtonRect = {
			.uwX = 0,
			.uwY = 0,
			.uwWidth = 0,
			.uwHeight = 0,
		},
		.ubIsHovered = 0,
	},
	{
		.szText = "How To Play?",
		.pAction = actionsMainMenuClose,
		.sButtonRect = {
			.uwX = 0,
			.uwY = 0,
			.uwWidth = 0,
			.uwHeight = 0,
		},
		.ubIsHovered = 0,
	},
	{
		.szText = "Credits",
		.pAction = actionsMainMenuClose,
		.sButtonRect = {
			.uwX = 0,
			.uwY = 0,
			.uwWidth = 0,
			.uwHeight = 0,
		},
		.ubIsHovered = 0,
	},
};

UBYTE s_ubMainMenuItemsCount = sizeof(s_pMainMenuItems) / sizeof(s_pMainMenuItems[0]);

/* Functions */

UBYTE menuItemGetTextColor(UBYTE ubIsHovered) {
	return ubIsHovered ? 1 : 0;
}

void menuItemMainMenuDraw(void) {
	UWORD uwScrollContentHeight = ((g_pFont->uwHeight + MENU_ITEMS_SPACING) * s_ubMainMenuItemsCount) + MENU_ITEMS_SPACING;
	scrollsSetContentHeight(SCROLL_RIGHT, uwScrollContentHeight);

	for (UBYTE ubItemIndex = 0; ubItemIndex < s_ubMainMenuItemsCount; ++ubItemIndex) {
		tMenuItem *pMenuItem = &s_pMainMenuItems[ubItemIndex];

		tUwCoordYX sTextDimensions = fontMeasureText(g_pFont, pMenuItem->szText);
		tUwCoordYX sTextLocalCoords = {
			.uwX = (scrollsGetContentWidth(SCROLL_RIGHT) / 2) - (sTextDimensions.uwX / 2),
			.uwY = (MENU_ITEMS_SPACING / 2) + (SCROLL_EDGE_HEIGHT / 2) - (uwScrollContentHeight / 2) + ((g_pFont->uwHeight + MENU_ITEMS_SPACING) * ubItemIndex),
		};
		tUwCoordYX sTextGlobalCoords = scrollsCoordsFromLocalToGlobal(SCROLL_RIGHT, sTextLocalCoords);

		pMenuItem->ubIsHovered = FALSE;
		pMenuItem->sButtonRect = (tUwRect) {
			.uwX = sTextGlobalCoords.uwX,
			.uwY = sTextGlobalCoords.uwY,
			.uwWidth = sTextDimensions.uwX,
			.uwHeight = sTextDimensions.uwY,
		};

		// At this stage draw everything on scroll buffer, as its contenxt are about to be shown by using scroll buffer
		fontDrawStr(
			g_pFont,
			g_ppScrollsContentBitMap[SCROLL_RIGHT],
			sTextLocalCoords.uwX,
			sTextLocalCoords.uwY,
			pMenuItem->szText,
			menuItemGetTextColor(FALSE),
			FONT_COOKIE,
			g_pTextBitMap
		);

#ifdef GAME_DEBUG
		debugDrawBox(g_ppScrollsContentBitMap[SCROLL_RIGHT], (tUwRect) {
			.uwX = sTextLocalCoords.uwX,
			.uwY = sTextLocalCoords.uwY,
			.uwWidth = sTextDimensions.uwX,
			.uwHeight = sTextDimensions.uwY,
		});
#endif
	}
}

void menuItemMainMenuProcess(void) {
	for (UBYTE ubItemIndex = 0; ubItemIndex < s_ubMainMenuItemsCount; ++ubItemIndex) {
		tMenuItem *pMenuItem = &s_pMainMenuItems[ubItemIndex];

		UBYTE ubIsHovered = mouseInRect(MOUSE_PORT_1, pMenuItem->sButtonRect);

		if (ubIsHovered != pMenuItem->ubIsHovered) {
			pMenuItem->ubIsHovered = ubIsHovered;

			fontDrawStr(
				g_pFont,
				g_pMenuBuffer->pBack,
				pMenuItem->sButtonRect.uwX,
				pMenuItem->sButtonRect.uwY,
				pMenuItem->szText,
				menuItemGetTextColor(ubIsHovered),
				FONT_COOKIE,
				g_pTextBitMap
			);

#ifdef GAME_DEBUG
			debugDrawBox(g_pMenuBuffer->pBack, pMenuItem->sButtonRect);
#endif
		}

		if (ubIsHovered && mouseUse(MOUSE_PORT_1, MOUSE_LMB)) {
			actionsSetTo(pMenuItem->pAction);
			return;
		}
	}
}
