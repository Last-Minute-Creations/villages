#include "gamestates/menu/scrolls.h"

#include <ace/managers/log.h>
#include <ace/utils/bitmap.h>
#include <ace/managers/key.h>

#include "game.h"
#include "utils.h"
#include "debug.h"
#include "gamestates/menu/menu.h"

/* Types */

#define SCROLLS_MARGIN 16
#define SCROLLS_X_SPEED 2
#define SCROLLS_Y_SPEED 16

#define SCROLL_LEFT_WIDTH 144
#define SCROLL_RIGHT_WIDTH 128

#define SCROLL_EDGE_WIDTH 16
#define SCROLL_EDGE_HEIGHT 208

#define SCROLL_BAR_HEIGHT 16

#define SCROLL_BAR_TOP_OPEN_POS SCROLLS_MARGIN
#define SCROLL_BAR_TOP_CLOSED_POS ((GAME_SCREEN_HEIGHT / 2) - SCROLL_BAR_HEIGHT)

#define SCROLL_BAR_BOTTOM_OPEN_POS (GAME_SCREEN_HEIGHT - SCROLLS_MARGIN - SCROLL_BAR_HEIGHT)
#define SCROLL_BAR_BOTTOM_CLOSED_POS (GAME_SCREEN_HEIGHT / 2)

#define SCROLL_CONTENT_OVERDRAW_SIZE 8
#define SCROLL_CONTENT_COLOR_INDEX 14
#define SCROLL_CONTENT_TO_SCREEN_OFFSET (SCROLL_BAR_TOP_OPEN_POS + SCROLL_BAR_HEIGHT - SCROLL_CONTENT_OVERDRAW_SIZE)

#define SCROLL_LEFT_CONTENT_WIDTH (SCROLL_LEFT_WIDTH - (SCROLL_EDGE_WIDTH * 2))
#define SCROLL_RIGHT_CONTENT_WIDTH (SCROLL_RIGHT_WIDTH - (SCROLL_EDGE_WIDTH * 2))

typedef enum _tScrollBarType {
    SCROLL_BAR_TOP,
    SCROLL_BAR_BOTTOM,
    SCROLL_BAR_COUNT,
} tScrollBarType;

typedef enum _tScrollState {
    SCROLL_STATE_MOVING,
    SCROLL_STATE_MOVING_DONE,
} tScrollState;

typedef struct _tScrollPos {
    WORD wCurrent;  // Current position to draw
    WORD wLast;  // Current position to undraw
    WORD wTarget;
} tScrollPos;

typedef struct _tScrollData {
    tScrollStage eStage;
    tScrollState eState;
    UWORD uwWidth;
    UWORD uwContentWidth;
    UWORD uwContentHeight;
    tScrollPos sXPos;
    tScrollPos pBarYPos[SCROLL_BAR_COUNT];
} tScrollData;

/* Globals */

/* Statics */

static tBitMap *s_pScrollsBitMap;
static tBitMap *s_pScrollsMaskBitMap;
static tBitMap *s_ppScrollsContentBitMap[SCROLL_COUNT];
static tScrollData s_pScrollsData[SCROLL_COUNT] = {
    {
        .eStage = SCROLL_STAGE_CLOSED,
        .eState = SCROLL_STATE_MOVING_DONE,
        .uwWidth = SCROLL_LEFT_WIDTH,
        .uwContentWidth = SCROLL_LEFT_CONTENT_WIDTH,
        .uwContentHeight = 0,
        .sXPos = {
            .wCurrent = SCROLLS_MARGIN,
            .wLast = SCROLLS_MARGIN,
            .wTarget = SCROLLS_MARGIN,
        },
        .pBarYPos = {
            {
                .wCurrent = SCROLL_BAR_TOP_CLOSED_POS,
                .wLast = SCROLL_BAR_TOP_CLOSED_POS,
                .wTarget = SCROLL_BAR_TOP_CLOSED_POS,
            },
            {
                .wCurrent = SCROLL_BAR_BOTTOM_CLOSED_POS,
                .wLast = SCROLL_BAR_BOTTOM_CLOSED_POS,
                .wTarget = SCROLL_BAR_BOTTOM_CLOSED_POS,
            },
        },
    },
    {
        .eStage = SCROLL_STAGE_CLOSED,
        .eState = SCROLL_STATE_MOVING_DONE,
        .uwWidth = SCROLL_RIGHT_WIDTH,
        .uwContentWidth = SCROLL_RIGHT_CONTENT_WIDTH,
        .uwContentHeight = 0,
        .sXPos = {
            .wCurrent = GAME_SCREEN_WIDTH - SCROLL_RIGHT_WIDTH - SCROLLS_MARGIN,
            .wLast = GAME_SCREEN_WIDTH - SCROLL_RIGHT_WIDTH - SCROLLS_MARGIN,
            .wTarget = GAME_SCREEN_WIDTH - SCROLL_RIGHT_WIDTH - SCROLLS_MARGIN,
        },
        .pBarYPos = {
            {
                .wCurrent = SCROLL_BAR_TOP_CLOSED_POS,
                .wLast = SCROLL_BAR_TOP_CLOSED_POS,
                .wTarget = SCROLL_BAR_TOP_CLOSED_POS,
            },
            {
                .wCurrent = SCROLL_BAR_BOTTOM_CLOSED_POS,
                .wLast = SCROLL_BAR_BOTTOM_CLOSED_POS,
                .wTarget = SCROLL_BAR_BOTTOM_CLOSED_POS,
            },
        },
    },
};

static tScrollType s_eScrollDrawTurn = SCROLL_LEFT;
static tScrollBarType s_eScrollBarDrawTurn = SCROLL_BAR_TOP;

/* Functions */

void scrollsCreate(void) {
    logBlockBegin("scrollsCreate()");

    s_pScrollsBitMap = bitmapCreateFromFile("data/debug/scrolls.bm", FALSE);
    s_pScrollsMaskBitMap = bitmapCreateFromFile("data/debug/scrolls_mask.bm", FALSE);

    for (tScrollType eScrollTye = SCROLL_LEFT; eScrollTye < SCROLL_COUNT; ++eScrollTye) {
        s_ppScrollsContentBitMap[eScrollTye] = bitmapCreate(s_pScrollsData[eScrollTye].uwContentWidth, SCROLL_EDGE_HEIGHT, GAME_BPP, 0);
        blitRect(s_ppScrollsContentBitMap[eScrollTye], 0, 0, s_pScrollsData[eScrollTye].uwContentWidth, SCROLL_EDGE_HEIGHT, SCROLL_CONTENT_COLOR_INDEX);
    }

    logBlockEnd("scrollsCreate()");
}

void scrollsDestroy(void) {
    logBlockBegin("scrollsDestroy()");

    for (tScrollType eScrollTye = SCROLL_LEFT; eScrollTye < SCROLL_COUNT; ++eScrollTye) {
        bitmapDestroy(s_ppScrollsContentBitMap[eScrollTye]);
    }

    bitmapDestroy(s_pScrollsMaskBitMap);
    bitmapDestroy(s_pScrollsBitMap);

    logBlockEnd("scrollsDestroy()");
}

void scrollsMoveTo(tScrollType eScrollType, tScrollStage eScrollStage) {
    if (s_pScrollsData[eScrollType].eStage != eScrollStage) {
        s_pScrollsData[eScrollType].eStage = eScrollStage;
        s_pScrollsData[eScrollType].eState = SCROLL_STATE_MOVING;

        // As we just started new animation, we need to set target coords for animation to reach
        switch(eScrollStage) {
            case SCROLL_STAGE_OPEN:
                s_pScrollsData[eScrollType].pBarYPos[SCROLL_BAR_TOP].wTarget = SCROLL_BAR_TOP_OPEN_POS;
                s_pScrollsData[eScrollType].pBarYPos[SCROLL_BAR_BOTTOM].wTarget = SCROLL_BAR_BOTTOM_OPEN_POS;
                break;
            case SCROLL_STAGE_CLOSED:
                s_pScrollsData[eScrollType].pBarYPos[SCROLL_BAR_TOP].wTarget = SCROLL_BAR_TOP_CLOSED_POS;
                s_pScrollsData[eScrollType].pBarYPos[SCROLL_BAR_BOTTOM].wTarget = SCROLL_BAR_BOTTOM_CLOSED_POS;
                break;
            default:
        }
    }
}

BYTE scrollsIsScrollBarDrawOutdated(tScrollType eScrollType, tScrollBarType eScrollBarType) {
    tScrollData *pScroll = &s_pScrollsData[eScrollType];
    tScrollPos *pYPos = &pScroll->pBarYPos[eScrollBarType];

    return pYPos->wCurrent != pYPos->wLast;
}

void scrollsMoveYPos() {
    if (
        !(
            (s_pScrollsData[s_eScrollDrawTurn].eStage == SCROLL_STAGE_OPEN) ||
            (s_pScrollsData[s_eScrollDrawTurn].eStage == SCROLL_STAGE_CLOSED)
        ) &&
        (s_pScrollsData[s_eScrollDrawTurn].eState != SCROLL_STATE_MOVING)
    ) {
        // If there is no Y animation currently running, exit early
        return;
    }

    tScrollPos *pBarPos = &s_pScrollsData[s_eScrollDrawTurn].pBarYPos[s_eScrollBarDrawTurn];
    tScrollPos *pBarTopPos = &s_pScrollsData[s_eScrollDrawTurn].pBarYPos[SCROLL_BAR_TOP];
    tScrollPos *pBarBottomPos = &s_pScrollsData[s_eScrollDrawTurn].pBarYPos[SCROLL_BAR_BOTTOM];

    // Make current value one step closer to target
    pBarPos->wCurrent += pBarPos->wCurrent < pBarPos->wTarget ? MIN(pBarPos->wTarget - pBarPos->wCurrent, SCROLLS_Y_SPEED) : MAX(pBarPos->wTarget - pBarPos->wCurrent, -SCROLLS_Y_SPEED);

    // Finish animation if both scroll bars reached their targets
    if (
        (pBarTopPos->wCurrent == pBarTopPos->wTarget) &&
        (pBarBottomPos->wCurrent == pBarBottomPos->wTarget)
    ) {
        s_pScrollsData[s_eScrollDrawTurn].eState = SCROLL_STATE_MOVING_DONE;
    }
}

void scrollsDrawBar(tScrollType eScrollType, tScrollBarType eScrollBarType) {
    debugSetColor(0x080);

    tScrollData *pScroll = &s_pScrollsData[eScrollType];
    tScrollPos *pYPos = &pScroll->pBarYPos[eScrollBarType];
    tScrollPos *pXPos = &pScroll->sXPos;
    WORD wMinYPos = MIN(pYPos->wCurrent, pYPos->wLast);

    vPortWaitForPos(g_pMenuVPort, MAX(pYPos->wCurrent + SCROLL_BAR_HEIGHT, pYPos->wLast + SCROLL_BAR_HEIGHT), 0);

    // Due to transparency, draw background over last and current position
    {
        blitCopyAligned(
            g_pSplashBitMap, pXPos->wLast, wMinYPos,
            g_pMenuBuffer->pBack, pXPos->wLast, wMinYPos,
            pScroll->uwWidth, SCROLL_BAR_HEIGHT + SCROLLS_Y_SPEED
        );
    }

    // Due to overdraw and transparency, lets draw overdraw with exposed content
    {
        UBYTE isOpening = (
            ((pYPos->wCurrent < pYPos->wLast) && (eScrollBarType == SCROLL_BAR_TOP)) ||
            ((pYPos->wLast < pYPos->wCurrent) && (eScrollBarType == SCROLL_BAR_BOTTOM))
        );
        UWORD uwScrollBarHeightOffset = (SCROLL_BAR_HEIGHT - SCROLL_CONTENT_OVERDRAW_SIZE) - ((SCROLL_BAR_HEIGHT - SCROLL_CONTENT_OVERDRAW_SIZE) * eScrollBarType);
        UWORD uwReDrawYPos = (isOpening ? wMinYPos + uwScrollBarHeightOffset : pYPos->wCurrent + SCROLL_CONTENT_OVERDRAW_SIZE - (SCROLL_CONTENT_OVERDRAW_SIZE * eScrollBarType));
        UWORD uwScrollHeightToDraw = (isOpening ? SCROLLS_Y_SPEED + SCROLL_CONTENT_OVERDRAW_SIZE : SCROLL_CONTENT_OVERDRAW_SIZE);

        // Draw newly exposed scroll content
        blitCopyAligned(
            s_ppScrollsContentBitMap[eScrollType], 0, uwReDrawYPos - SCROLL_CONTENT_TO_SCREEN_OFFSET,
            g_pMenuBuffer->pBack, pXPos->wLast + SCROLL_EDGE_WIDTH, uwReDrawYPos,
            pScroll->uwContentWidth, uwScrollHeightToDraw
        );

        UWORD uwScrollEdgeBitMapOffset = SCROLL_BAR_HEIGHT * SCROLL_COUNT * SCROLL_BAR_COUNT;

        // Draw left scroll edge
        blitCopyMask(
            s_pScrollsBitMap, (SCROLL_EDGE_WIDTH * eScrollType * 2), uwReDrawYPos - SCROLL_CONTENT_TO_SCREEN_OFFSET + uwScrollEdgeBitMapOffset,
            g_pMenuBuffer->pBack, pXPos->wLast, uwReDrawYPos,
            SCROLL_EDGE_WIDTH, uwScrollHeightToDraw,
            (const UWORD *) s_pScrollsMaskBitMap->Planes[0]
        );

        // Draw right scroll edge
        blitCopyMask(
            s_pScrollsBitMap, (SCROLL_EDGE_WIDTH * eScrollType * 2) + SCROLL_EDGE_WIDTH, uwReDrawYPos - SCROLL_CONTENT_TO_SCREEN_OFFSET + uwScrollEdgeBitMapOffset,
            g_pMenuBuffer->pBack, pXPos->wLast + pScroll->uwContentWidth + SCROLL_EDGE_WIDTH, uwReDrawYPos,
            SCROLL_EDGE_WIDTH, uwScrollHeightToDraw,
            (const UWORD *) s_pScrollsMaskBitMap->Planes[0]
        );
    }

    // Draw scroll bar at current position
    {
        blitCopyMask(
            s_pScrollsBitMap, 0, (SCROLL_BAR_HEIGHT * eScrollType * 2) + (SCROLL_BAR_HEIGHT * eScrollBarType),
            g_pMenuBuffer->pBack, pXPos->wCurrent, pYPos->wCurrent,
            pScroll->uwWidth, SCROLL_BAR_HEIGHT,
            (const UWORD *) s_pScrollsMaskBitMap->Planes[0]
        );
    }

    pYPos->wLast = pYPos->wCurrent;

    debugSetColor(0x800);
}

void scrollsDraw(void) {
    if (scrollsIsScrollBarDrawOutdated(s_eScrollDrawTurn, s_eScrollBarDrawTurn)) {
        scrollsDrawBar(s_eScrollDrawTurn, s_eScrollBarDrawTurn);
    }
}

void scrollsProcessDrawOrder(void) {
    // To optimize drawing speeds, we want to reduce blits per one frame to single scroll bar
    s_eScrollBarDrawTurn += 1;

    if (s_eScrollBarDrawTurn == SCROLL_BAR_COUNT) {
        s_eScrollBarDrawTurn %= SCROLL_BAR_COUNT;

        s_eScrollDrawTurn = (s_eScrollDrawTurn + 1) % SCROLL_COUNT;
    }
}

void scrollsDrawAll(void) {
    for (UBYTE ubScroll = SCROLL_LEFT; ubScroll < SCROLL_COUNT; ++ubScroll) {
        for (UBYTE ubScrollBar = SCROLL_BAR_TOP; ubScrollBar < SCROLL_BAR_COUNT; ++ubScrollBar) {
            scrollsDrawBar(ubScroll, ubScrollBar);
        }
    }
}

void scrollsProcess(void) {
    scrollsMoveYPos();

    scrollsDraw();

    scrollsProcessDrawOrder();
}

BYTE scrollsRequestStage(tScrollType eScrollType, tScrollStage eScrollStage) {
    if (s_pScrollsData[eScrollType].eStage == eScrollStage) {
        // Target stage is either in progress or already prepared
        return s_pScrollsData[eScrollType].eState == SCROLL_STATE_MOVING_DONE;
    }

    // Target stage is higher or lower than current stage, if we can, lets advance current stage closer to target
    if (s_pScrollsData[eScrollType].eState == SCROLL_STATE_MOVING_DONE) {
        scrollsMoveTo(eScrollType, s_pScrollsData[eScrollType].eStage - CMP(s_pScrollsData[eScrollType].eStage, eScrollStage));
    }

    // Current stage is animating, let's wait for animation completion

    return FALSE;
}
