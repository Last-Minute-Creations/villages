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

#define SCROLL_BAR_TOP_OPEN_Y_POS SCROLLS_MARGIN
#define SCROLL_BAR_TOP_CLOSED_Y_POS ((GAME_SCREEN_HEIGHT / 2) - SCROLL_BAR_HEIGHT)

#define SCROLL_BAR_BOTTOM_OPEN_Y_POS (GAME_SCREEN_HEIGHT - SCROLLS_MARGIN - SCROLL_BAR_HEIGHT)
#define SCROLL_BAR_BOTTOM_CLOSED_Y_POS (GAME_SCREEN_HEIGHT / 2)

#define SCROLL_CONTENT_OVERDRAW_SIZE 8
#define SCROLL_CONTENT_COLOR_INDEX 14
#define SCROLL_CONTENT_TO_SCREEN_Y_OFFSET (SCROLL_BAR_TOP_OPEN_Y_POS + SCROLL_BAR_HEIGHT - SCROLL_CONTENT_OVERDRAW_SIZE)

#define SCROLL_LEFT_CONTENT_WIDTH (SCROLL_LEFT_WIDTH - (SCROLL_EDGE_WIDTH * 2))
#define SCROLL_LEFT_HIDDEN_X_POS -SCROLL_LEFT_WIDTH
#define SCROLL_LEFT_OPEN_X_POS SCROLLS_MARGIN

#define SCROLL_RIGHT_CONTENT_WIDTH (SCROLL_RIGHT_WIDTH - (SCROLL_EDGE_WIDTH * 2))
#define SCROLL_RIGHT_HIDDEN_X_POS GAME_SCREEN_WIDTH
#define SCROLL_RIGHT_OPEN_X_POS (GAME_SCREEN_WIDTH - SCROLLS_MARGIN - SCROLL_RIGHT_WIDTH)

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
    tScrollStage eStage;
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
        .eStage = SCROLL_STAGE_CLOSED,
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
    tScrollData *pScroll = &s_pScrollsData[eScrollType];

    pScroll->eStage = eScrollStage;

    // To start new animation, we need to set target coords for animation to reach
    switch (eScrollStage) {
        case SCROLL_STAGE_OPEN:
            pScroll->sXPos.wTarget = eScrollType == SCROLL_LEFT ? SCROLL_LEFT_OPEN_X_POS : SCROLL_RIGHT_OPEN_X_POS;
            pScroll->pBarYPos[SCROLL_BAR_TOP].wTarget = SCROLL_BAR_TOP_OPEN_Y_POS;
            pScroll->pBarYPos[SCROLL_BAR_BOTTOM].wTarget = SCROLL_BAR_BOTTOM_OPEN_Y_POS;
            break;
        case SCROLL_STAGE_CLOSED:
            pScroll->sXPos.wTarget = eScrollType == SCROLL_LEFT ? SCROLL_LEFT_OPEN_X_POS : SCROLL_RIGHT_OPEN_X_POS;
            pScroll->pBarYPos[SCROLL_BAR_TOP].wTarget = SCROLL_BAR_TOP_CLOSED_Y_POS;
            pScroll->pBarYPos[SCROLL_BAR_BOTTOM].wTarget = SCROLL_BAR_BOTTOM_CLOSED_Y_POS;
            break;
        case SCROLL_STAGE_HIDDEN:
            pScroll->sXPos.wTarget = eScrollType == SCROLL_LEFT ? SCROLL_LEFT_HIDDEN_X_POS : SCROLL_RIGHT_HIDDEN_X_POS;
            pScroll->pBarYPos[SCROLL_BAR_TOP].wTarget = SCROLL_BAR_TOP_CLOSED_Y_POS;
            pScroll->pBarYPos[SCROLL_BAR_BOTTOM].wTarget = SCROLL_BAR_BOTTOM_CLOSED_Y_POS;
            break;
    };

    logWrite("Scroll %d started animation to %d", eScrollType, eScrollStage);
}

BYTE scrollsIsScrollBarDrawYOutdated(tScrollType eScrollType, tScrollBarType eScrollBarType) {
    tScrollData *pScroll = &s_pScrollsData[eScrollType];
    tScrollPos *pYPos = &pScroll->pBarYPos[eScrollBarType];

    return pYPos->wCurrent != pYPos->wLast;
}

BYTE scrollsIsScrollBarDrawXOutdated(tScrollType eScrollType) {
    return s_pScrollsData[eScrollType].sXPos.wCurrent != s_pScrollsData[eScrollType].sXPos.wLast;
}

BYTE scrollIsScrollAnimationInProgress(tScrollType eScrollType) {
    tScrollData *pScroll = &s_pScrollsData[eScrollType];
    tScrollPos *pXPos = &pScroll->sXPos;

    return (pXPos->wCurrent != pXPos->wTarget) ||
        (pScroll->pBarYPos[SCROLL_BAR_TOP].wCurrent != pScroll->pBarYPos[SCROLL_BAR_TOP].wTarget) ||
        (pScroll->pBarYPos[SCROLL_BAR_BOTTOM].wCurrent != pScroll->pBarYPos[SCROLL_BAR_BOTTOM].wTarget);
}

void scrollsMakeStepToTargetPos(tScrollPos *pBarPos, UBYTE ubSpeed) {
    pBarPos->wCurrent += (
        pBarPos->wCurrent < pBarPos->wTarget
        ? MIN(pBarPos->wTarget - pBarPos->wCurrent, ubSpeed)
        : MAX(pBarPos->wTarget - pBarPos->wCurrent, -ubSpeed)
    );
}

void scrollsAnimatePos() {
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
    // {
    //     WORD wLeftOffscreenSize = -MIN(0, pXPos->wLast);
    //     WORD wRightOffscreenSize = MAX(0, pXPos->wLast + (WORD) pScroll->uwWidth - GAME_SCREEN_WIDTH);
    //     UWORD uwActualWidth = pScroll->uwWidth - wLeftOffscreenSize - wRightOffscreenSize;

    //     if (uwActualWidth) {
    //         logWrite("Last:");
    //         logWrite("Offscreens: %d %d", wLeftOffscreenSize, wRightOffscreenSize);
    //         logWrite("Source: %d %d", wLeftOffscreenSize, SCROLL_BAR_HEIGHT * eScrollType * 2);
    //         logWrite("Desc: %d %d", pXPos->wLast + wLeftOffscreenSize, SCROLL_BAR_TOP_CLOSED_Y_POS);
    //         logWrite("Sizes: %d %d", uwActualWidth, SCROLL_BAR_HEIGHT * 2);
    //         logWrite(" ");
    //         blitCopy(
    //             g_pSplashBitMap, wLeftOffscreenSize, SCROLL_BAR_HEIGHT * eScrollType * 2,
    //             g_pMenuBuffer->pBack, pXPos->wLast + wLeftOffscreenSize, SCROLL_BAR_TOP_CLOSED_Y_POS,
    //             uwActualWidth, SCROLL_BAR_HEIGHT * 2,
    //             MINTERM_COOKIE
    //         );
    //     }
    // }

    // Lets draw current position, minding that X movement can't draw outside the screen
    {
        WORD wLeftOffscreenSize = -MIN(0, pXPos->wCurrent);
        WORD wRightOffscreenSize = MAX(0, pXPos->wCurrent + (WORD) pScroll->uwWidth - GAME_SCREEN_WIDTH);

        logWrite("Current:");
        logWrite("Offscreens: %d %d", wLeftOffscreenSize, wRightOffscreenSize);
        logWrite("Source: %d %d", wLeftOffscreenSize, SCROLL_BAR_HEIGHT * eScrollType * 2);
        logWrite("Desc: %d %d", pXPos->wCurrent + wLeftOffscreenSize, SCROLL_BAR_TOP_CLOSED_Y_POS);
        logWrite("Sizes: %d %d", pScroll->uwWidth - wLeftOffscreenSize - wRightOffscreenSize, SCROLL_BAR_HEIGHT * 2);
        logWrite(" - - - - -");
        // FIXME
        blitCopyMask(
            s_pScrollsBitMap, 20, SCROLL_BAR_HEIGHT * eScrollType * 2,
            g_pMenuBuffer->pBack, 0, SCROLL_BAR_TOP_CLOSED_Y_POS,
            124, SCROLL_BAR_HEIGHT * 2,
            (const UWORD *) s_pScrollsMaskBitMap->Planes[0]
        );
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
            s_ppScrollsContentBitMap[eScrollType], 0, uwReDrawYPos - SCROLL_CONTENT_TO_SCREEN_Y_OFFSET,
            g_pMenuBuffer->pBack, pXPos->wLast + SCROLL_EDGE_WIDTH, uwReDrawYPos,
            pScroll->uwContentWidth, uwScrollHeightToDraw
        );

        UWORD uwScrollEdgeBitMapOffset = SCROLL_BAR_HEIGHT * SCROLL_COUNT * SCROLL_BAR_COUNT;

        // Draw left scroll edge
        blitCopyMask(
            s_pScrollsBitMap, (SCROLL_EDGE_WIDTH * eScrollType * 2), uwReDrawYPos - SCROLL_CONTENT_TO_SCREEN_Y_OFFSET + uwScrollEdgeBitMapOffset,
            g_pMenuBuffer->pBack, pXPos->wLast, uwReDrawYPos,
            SCROLL_EDGE_WIDTH, uwScrollHeightToDraw,
            (const UWORD *) s_pScrollsMaskBitMap->Planes[0]
        );

        // Draw right scroll edge
        blitCopyMask(
            s_pScrollsBitMap, (SCROLL_EDGE_WIDTH * eScrollType * 2) + SCROLL_EDGE_WIDTH, uwReDrawYPos - SCROLL_CONTENT_TO_SCREEN_Y_OFFSET + uwScrollEdgeBitMapOffset,
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
    // s_eScrollBarDrawTurn += 1;

    // if (s_eScrollBarDrawTurn == SCROLL_BAR_COUNT) {
    //     s_eScrollBarDrawTurn %= SCROLL_BAR_COUNT;

        // s_eScrollDrawTurn = (s_eScrollDrawTurn + 1) % SCROLL_COUNT;
    // }
}

// void scrollsDrawAll(void) {
//     for (UBYTE ubScroll = SCROLL_LEFT; ubScroll < SCROLL_COUNT; ++ubScroll) {
//         for (UBYTE ubScrollBar = SCROLL_BAR_TOP; ubScrollBar < SCROLL_BAR_COUNT; ++ubScrollBar) {
//             scrollsDrawYAnimation(ubScroll, ubScrollBar);
//         }
//     }
// }

void scrollsProcess(void) {
    scrollsAnimatePos();

    scrollsDraw();

    scrollsProcessDrawOrder();
}

BYTE scrollsRequestStage(tScrollType eScrollType, tScrollStage eScrollStage) {
    if (s_pScrollsData[eScrollType].eStage == eScrollStage) {
        // Target stage is either in progress or already prepared
        return !scrollIsScrollAnimationInProgress(eScrollType);
    }

    // Target stage is higher or lower than current stage, if we can, lets advance current stage closer to target
    if (!scrollIsScrollAnimationInProgress(eScrollType)) {
        scrollsMoveTo(eScrollType, s_pScrollsData[eScrollType].eStage + CMP(eScrollStage, s_pScrollsData[eScrollType].eStage));
    }

    // Current stage is animating, let's wait for animation completion
    return FALSE;
}
