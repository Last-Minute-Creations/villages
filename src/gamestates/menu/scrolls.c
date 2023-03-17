#include "gamestates/menu/scrolls.h"

#include <ace/managers/log.h>
#include <ace/managers/key.h>

#include "game.h"
#include "utils.h"
#include "debug.h"
#include "gamestates/menu/menu.h"

/* Types */

typedef enum _tScrollBarType {
    SCROLL_BAR_TOP,
    SCROLL_BAR_BOTTOM,
    SCROLL_BAR_COUNT,
} tScrollBarType;

typedef struct _tScrollPos {
    WORD wCurrent;  // Current position to draw
    WORD wLast;  // Current position to undraw
    WORD wTarget;
} tScrollPos;

typedef struct _tScrollData {
    tScrollState eState;
    UWORD uwWidth;
    UWORD uwContentWidth;
    UWORD uwContentHeight;
    tScrollPos sXPos;
    tScrollPos pBarYPos[SCROLL_BAR_COUNT];
} tScrollData;

/* Globals */

tBitMap *g_ppScrollsContentBitMap[SCROLL_COUNT];

/* Statics */

static tBitMap *s_pScrollsBitMap;
static tBitMap *s_pScrollsMaskBitMap;
static tScrollData s_pScrollsData[SCROLL_COUNT] = {
    {
        .eState = SCROLL_STATE_HIDDEN,
        .uwWidth = SCROLL_LEFT_WIDTH,
        .uwContentWidth = SCROLL_LEFT_CONTENT_WIDTH,
        .uwContentHeight = 0,
        .sXPos = {
            .wCurrent = SCROLL_LEFT_HIDDEN_X_POS,
            .wLast = SCROLL_LEFT_HIDDEN_X_POS,
            .wTarget = SCROLL_LEFT_HIDDEN_X_POS,
        },
        .pBarYPos = {
            {
                .wCurrent = SCROLL_BAR_TOP_CLOSED_Y_POS,
                .wLast = SCROLL_BAR_TOP_CLOSED_Y_POS,
                .wTarget = SCROLL_BAR_TOP_CLOSED_Y_POS,
            },
            {
                .wCurrent = SCROLL_BAR_BOTTOM_CLOSED_Y_POS,
                .wLast = SCROLL_BAR_BOTTOM_CLOSED_Y_POS,
                .wTarget = SCROLL_BAR_BOTTOM_CLOSED_Y_POS,
            },
        },
    },
    {
        .eState = SCROLL_STATE_HIDDEN,
        .uwWidth = SCROLL_RIGHT_WIDTH,
        .uwContentWidth = SCROLL_RIGHT_CONTENT_WIDTH,
        .uwContentHeight = 0,
        .sXPos = {
            .wCurrent = SCROLL_RIGHT_HIDDEN_X_POS,
            .wLast = SCROLL_RIGHT_HIDDEN_X_POS,
            .wTarget = SCROLL_RIGHT_HIDDEN_X_POS,
        },
        .pBarYPos = {
            {
                .wCurrent = SCROLL_BAR_TOP_CLOSED_Y_POS,
                .wLast = SCROLL_BAR_TOP_CLOSED_Y_POS,
                .wTarget = SCROLL_BAR_TOP_CLOSED_Y_POS,
            },
            {
                .wCurrent = SCROLL_BAR_BOTTOM_CLOSED_Y_POS,
                .wLast = SCROLL_BAR_BOTTOM_CLOSED_Y_POS,
                .wTarget = SCROLL_BAR_BOTTOM_CLOSED_Y_POS,
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
        g_ppScrollsContentBitMap[eScrollTye] = bitmapCreate(s_pScrollsData[eScrollTye].uwContentWidth, SCROLL_EDGE_HEIGHT, GAME_BPP, 0);
        blitRect(g_ppScrollsContentBitMap[eScrollTye], 0, 0, s_pScrollsData[eScrollTye].uwContentWidth, SCROLL_EDGE_HEIGHT, SCROLL_CONTENT_COLOR_INDEX);
        blitLine(g_ppScrollsContentBitMap[eScrollTye], 0, 0, s_pScrollsData[eScrollTye].uwContentWidth, SCROLL_EDGE_HEIGHT, 0, 0xFFFF, TRUE);
        blitLine(g_ppScrollsContentBitMap[eScrollTye], s_pScrollsData[eScrollTye].uwContentWidth, 0, 0, SCROLL_EDGE_HEIGHT, 0, 0xFFFF, TRUE);
    }

    logBlockEnd("scrollsCreate()");
}

void scrollsDestroy(void) {
    logBlockBegin("scrollsDestroy()");

    for (tScrollType eScrollTye = SCROLL_LEFT; eScrollTye < SCROLL_COUNT; ++eScrollTye) {
        bitmapDestroy(g_ppScrollsContentBitMap[eScrollTye]);
    }

    bitmapDestroy(s_pScrollsMaskBitMap);
    bitmapDestroy(s_pScrollsBitMap);

    logBlockEnd("scrollsDestroy()");
}

void scrollsSetTarget(tScrollType eScrollType, tScrollState eScrollState) {
    tScrollData *pScroll = &s_pScrollsData[eScrollType];

    pScroll->eState = eScrollState;

    // To start new animation, we need to set target coords for animation to reach
    switch (eScrollState) {
        case SCROLL_STATE_OPEN:
            UWORD uwHalfContentHeight = pScroll->uwContentHeight / 2;

            pScroll->sXPos.wTarget = eScrollType == SCROLL_LEFT ? SCROLL_LEFT_OPEN_X_POS : SCROLL_RIGHT_OPEN_X_POS;
            pScroll->pBarYPos[SCROLL_BAR_TOP].wTarget = MAX(SCROLL_BAR_TOP_CLOSED_Y_POS - uwHalfContentHeight, SCROLL_BAR_TOP_OPEN_Y_POS);
            pScroll->pBarYPos[SCROLL_BAR_BOTTOM].wTarget = MIN(SCROLL_BAR_BOTTOM_CLOSED_Y_POS + uwHalfContentHeight, SCROLL_BAR_BOTTOM_OPEN_Y_POS);;
            break;
        case SCROLL_STATE_CLOSED:
            pScroll->sXPos.wTarget = eScrollType == SCROLL_LEFT ? SCROLL_LEFT_OPEN_X_POS : SCROLL_RIGHT_OPEN_X_POS;
            pScroll->pBarYPos[SCROLL_BAR_TOP].wTarget = SCROLL_BAR_TOP_CLOSED_Y_POS;
            pScroll->pBarYPos[SCROLL_BAR_BOTTOM].wTarget = SCROLL_BAR_BOTTOM_CLOSED_Y_POS;
            break;
        case SCROLL_STATE_HIDDEN:
            pScroll->sXPos.wTarget = eScrollType == SCROLL_LEFT ? SCROLL_LEFT_HIDDEN_X_POS : SCROLL_RIGHT_HIDDEN_X_POS;
            pScroll->pBarYPos[SCROLL_BAR_TOP].wTarget = SCROLL_BAR_TOP_CLOSED_Y_POS;
            pScroll->pBarYPos[SCROLL_BAR_BOTTOM].wTarget = SCROLL_BAR_BOTTOM_CLOSED_Y_POS;
            break;
    };

    logWrite("Scroll %d started animation to %d", eScrollType, eScrollState);
}

UBYTE scrollsIsScrollBarDrawYOutdated(tScrollType eScrollType, tScrollBarType eScrollBarType) {
    tScrollData *pScroll = &s_pScrollsData[eScrollType];
    tScrollPos *pYPos = &pScroll->pBarYPos[eScrollBarType];

    return pYPos->wCurrent != pYPos->wLast;
}

UBYTE scrollsIsScrollBarDrawXOutdated(tScrollType eScrollType) {
    return s_pScrollsData[eScrollType].sXPos.wCurrent != s_pScrollsData[eScrollType].sXPos.wLast;
}

UBYTE scrollsIsScrollAnimationComplete(tScrollType eScrollType) {
    tScrollData *pScroll = &s_pScrollsData[eScrollType];
    tScrollPos *pXPos = &pScroll->sXPos;

    return (pXPos->wCurrent == pXPos->wTarget) &&
        (pScroll->pBarYPos[SCROLL_BAR_TOP].wCurrent == pScroll->pBarYPos[SCROLL_BAR_TOP].wTarget) &&
        (pScroll->pBarYPos[SCROLL_BAR_BOTTOM].wCurrent == pScroll->pBarYPos[SCROLL_BAR_BOTTOM].wTarget);
}

void scrollsMakeStepToTargetPos(tScrollPos *pBarPos, UBYTE ubSpeed) {
    pBarPos->wCurrent += (
        pBarPos->wCurrent < pBarPos->wTarget
        ? MIN(pBarPos->wTarget - pBarPos->wCurrent, ubSpeed)
        : MAX(pBarPos->wTarget - pBarPos->wCurrent, -ubSpeed)
    );
}

void scrollsAnimatePos(void) {
    tScrollPos *pBarXPos = &s_pScrollsData[s_eScrollDrawTurn].sXPos;
    tScrollPos *pBarYPos = &s_pScrollsData[s_eScrollDrawTurn].pBarYPos[s_eScrollBarDrawTurn];

    if (pBarXPos->wCurrent != pBarXPos->wTarget) {
        scrollsMakeStepToTargetPos(pBarXPos, SCROLLS_X_SPEED);
    }
    else if (pBarYPos->wCurrent != pBarYPos->wTarget) {
        scrollsMakeStepToTargetPos(pBarYPos, SCROLLS_Y_SPEED);
    }
}

void scrollsDrawXAnimation(tScrollType eScrollType) {
    debugSetColor(0x080);

    tScrollData *pScroll = &s_pScrollsData[eScrollType];
    tScrollPos *pXPos = &pScroll->sXPos;

    // Due to transparency, draw background over last and current position, minding that X movement can't draw outside the screen
    {
        WORD wLeftOffscreenSize = -MIN(0, pXPos->wLast);
        WORD wRightOffscreenSize = MAX(0, pXPos->wLast + (WORD) pScroll->uwWidth - GAME_SCREEN_WIDTH);
        UWORD uwActualWidth = pScroll->uwWidth - wLeftOffscreenSize - wRightOffscreenSize;

        if (uwActualWidth) {
            blitCopy(
                g_pSplashBitMap, pXPos->wLast + wLeftOffscreenSize, SCROLL_BAR_TOP_CLOSED_Y_POS,
                g_pMenuBuffer->pBack, pXPos->wLast + wLeftOffscreenSize, SCROLL_BAR_TOP_CLOSED_Y_POS,
                uwActualWidth, SCROLL_BAR_HEIGHT * 2,
                MINTERM_COOKIE
            );
        }
    }

    // Lets draw current position, minding that X movement can't draw outside the screen
    {
        WORD wLeftOffscreenSize = -MIN(0, pXPos->wCurrent);
        WORD wRightOffscreenSize = MAX(0, pXPos->wCurrent + (WORD) pScroll->uwWidth - GAME_SCREEN_WIDTH);
        UWORD uwActualWidth = pScroll->uwWidth - wLeftOffscreenSize - wRightOffscreenSize;

        if (uwActualWidth) {
            blitCopyMask(
                s_pScrollsBitMap, wLeftOffscreenSize, SCROLL_BAR_HEIGHT * eScrollType * 2,
                g_pMenuBuffer->pBack, pXPos->wCurrent + wLeftOffscreenSize, SCROLL_BAR_TOP_CLOSED_Y_POS,
                uwActualWidth, SCROLL_BAR_HEIGHT * 2,
                (const UWORD *) s_pScrollsMaskBitMap->Planes[0]
            );
        }
    }

    // Let's remember where we just draw, to undraw that on next iteration
    pXPos->wLast = pXPos->wCurrent;

    debugSetColor(0x800);
}

void scrollsDrawYAnimation(tScrollType eScrollType, tScrollBarType eScrollBarType) {
    debugSetColor(0x080);

    tScrollData *pScroll = &s_pScrollsData[eScrollType];
    tScrollPos *pYPos = &pScroll->pBarYPos[eScrollBarType];
    tScrollPos *pXPos = &pScroll->sXPos;
    WORD wMinYPos = MIN(pYPos->wCurrent, pYPos->wLast);
    UWORD uwYPosDiff = ABS(pYPos->wCurrent - pYPos->wLast);

    // As we're using single buffering, we don't want to draw before screen beam position
    // vPortWaitForPos(g_pMenuVPort, MAX(pYPos->wCurrent + SCROLL_BAR_HEIGHT, pYPos->wLast + SCROLL_BAR_HEIGHT), 0);

    // Due to transparency, draw background over last and current position
    {
        blitCopyAligned(
            g_pSplashBitMap, pXPos->wLast, wMinYPos,
            g_pMenuBuffer->pBack, pXPos->wLast, wMinYPos,
            pScroll->uwWidth, SCROLL_BAR_HEIGHT + uwYPosDiff
        );
    }

    // Lets draw exposed content
    if (
        ((pYPos->wCurrent < pYPos->wLast) && (eScrollBarType == SCROLL_BAR_TOP)) ||
        ((pYPos->wLast < pYPos->wCurrent) && (eScrollBarType == SCROLL_BAR_BOTTOM))
    ) {
        UWORD uwScrollBarHeightOffset = SCROLL_BAR_HEIGHT - (SCROLL_BAR_HEIGHT * eScrollBarType);
        UWORD uwReDrawYPos = wMinYPos + uwScrollBarHeightOffset;

        // Draw newly exposed scroll content
        blitCopyAligned(
            g_ppScrollsContentBitMap[eScrollType], 0, uwReDrawYPos - SCROLL_CONTENT_TO_SCREEN_Y_OFFSET,
            g_pMenuBuffer->pBack, pXPos->wLast + SCROLL_EDGE_WIDTH, uwReDrawYPos,
            pScroll->uwContentWidth, uwYPosDiff
        );

        UWORD uwScrollEdgeBitMapOffset = SCROLL_BAR_HEIGHT * SCROLL_COUNT * SCROLL_BAR_COUNT;

        // Draw left scroll edge
        blitCopyMask(
            s_pScrollsBitMap, (SCROLL_EDGE_WIDTH * eScrollType * 2), uwReDrawYPos - SCROLL_CONTENT_TO_SCREEN_Y_OFFSET + uwScrollEdgeBitMapOffset,
            g_pMenuBuffer->pBack, pXPos->wLast, uwReDrawYPos,
            SCROLL_EDGE_WIDTH, uwYPosDiff,
            (const UWORD *) s_pScrollsMaskBitMap->Planes[0]
        );

        // Draw right scroll edge
        blitCopyMask(
            s_pScrollsBitMap, (SCROLL_EDGE_WIDTH * eScrollType * 2) + SCROLL_EDGE_WIDTH, uwReDrawYPos - SCROLL_CONTENT_TO_SCREEN_Y_OFFSET + uwScrollEdgeBitMapOffset,
            g_pMenuBuffer->pBack, pXPos->wLast + pScroll->uwContentWidth + SCROLL_EDGE_WIDTH, uwReDrawYPos,
            SCROLL_EDGE_WIDTH, uwYPosDiff,
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

    // Let's remember where we just draw, to undraw that on next iteration
    pYPos->wLast = pYPos->wCurrent;

    debugSetColor(0x800);
}

void scrollsDraw(void) {
    if (scrollsIsScrollBarDrawXOutdated(s_eScrollDrawTurn)) {
        scrollsDrawXAnimation(s_eScrollDrawTurn);
    }
    else if (scrollsIsScrollBarDrawYOutdated(s_eScrollDrawTurn, s_eScrollBarDrawTurn)) {
        scrollsDrawYAnimation(s_eScrollDrawTurn, s_eScrollBarDrawTurn);
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

UBYTE scrollsRequestState(tScrollType eScrollType, tScrollState eScrollState) {
    if (scrollsIsScrollAnimationComplete(eScrollType)) {
        // Animation is complete, lets check if we're at requested state
        if (s_pScrollsData[eScrollType].eState == eScrollState) {
            return TRUE;
        }

        // Animation is complete, but we're not at requested state, lets take current state closer to requested state
        scrollsSetTarget(eScrollType, s_pScrollsData[eScrollType].eState + CMP(eScrollState, s_pScrollsData[eScrollType].eState));
    }

    // Lets process animation closer to target
    {
        scrollsAnimatePos();
        scrollsDraw();
        scrollsProcessDrawOrder();
    }

    // Current state is animating, let's wait for animation completion
    return FALSE;
}

UWORD scrollsGetContentHeight(tScrollType eScrollType) {
    return s_pScrollsData[eScrollType].uwContentHeight;
}

void scrollsSetContentHeight(tScrollType eScrollType, UWORD uwContentHeight) {
    s_pScrollsData[eScrollType].uwContentHeight = uwContentHeight;
}

UWORD scrollsGetContentWidth(tScrollType eScrollType) {
    return s_pScrollsData[eScrollType].uwContentWidth;
}

tUwCoordYX scrollsCoordsFromLocalToGlobal(tScrollType eScrollType, tUwCoordYX sLocalCoords) {
    sLocalCoords.uwX += ((eScrollType == SCROLL_LEFT) ? SCROLL_LEFT_OPEN_X_POS : SCROLL_RIGHT_OPEN_X_POS) + SCROLL_EDGE_WIDTH;
    sLocalCoords.uwY += SCROLL_BAR_TOP_OPEN_Y_POS + SCROLL_BAR_HEIGHT;

    return sLocalCoords;
}

tUwCoordYX scrollsCoordsFromGlobalToLocal(tScrollType eScrollType, tUwCoordYX sGlobalCoords) {
    sGlobalCoords.uwX -= ((eScrollType == SCROLL_LEFT) ? SCROLL_LEFT_OPEN_X_POS : SCROLL_RIGHT_OPEN_X_POS) + SCROLL_EDGE_WIDTH;
    sGlobalCoords.uwY -= SCROLL_BAR_TOP_OPEN_Y_POS + SCROLL_BAR_HEIGHT;

    return sGlobalCoords;
}
