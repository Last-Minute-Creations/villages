#include "gamestates/map.h"

#include <ace/managers/log.h>
#include <ace/managers/key.h>
#include <ace/managers/state.h>
#include <ace/managers/game.h>
#include <ace/managers/system.h>
#include <ace/managers/log.h>
#include <ace/managers/viewport/tilebuffer.h>
#include <ace/managers/mouse.h>

#include <ace/utils/bitmap.h>
#include <ace/utils/palette.h>

#include "game.h"
#include "../debug.h"
#include "../tileset.h"
#include "../cursor.h"

tView *s_pMapView = 0;
tVPort *s_pMapVPort = 0;
tTileBufferManager *s_pMapBuffer = 0;
tBitMap *s_pTileSet = 0;

void gsMapCreate() {
	logBlockBegin("gsMapCreate()");

	systemUse();

	s_pTileSet = bitmapCreateFromFile("data/mirmirowo_tileset.bm", 0);

	s_pMapView = viewCreate(0,
		TAG_VIEW_GLOBAL_CLUT, 1,
	TAG_END);

	s_pMapVPort = vPortCreate(0,
		TAG_VPORT_VIEW, s_pMapView,
		TAG_VPORT_BPP, WINDOW_SCREEN_BPP,
	TAG_END);

	s_pMapBuffer = tileBufferCreate(0,
		TAG_TILEBUFFER_VPORT, s_pMapVPort,
		TAG_TILEBUFFER_BITMAP_FLAGS, BMF_CLEAR | BMF_INTERLEAVED,
		TAG_TILEBUFFER_BOUND_TILE_X, MAP_SIZE,
		TAG_TILEBUFFER_BOUND_TILE_Y, MAP_SIZE,
		TAG_TILEBUFFER_IS_DBLBUF, 1,
		TAG_TILEBUFFER_TILE_SHIFT, TILESET_TILE_SIZE_IN_BINARY_SHIFT,
		TAG_TILEBUFFER_REDRAW_QUEUE_LENGTH, 100,
		TAG_TILEBUFFER_TILESET, s_pTileSet,
	TAG_END);

	paletteLoad("data/mirmirowo.plt", s_pMapVPort->pPalette, 1 << WINDOW_SCREEN_BPP);
	copBlockDisableSprites(s_pMapView->pCopList, 0xFE);
	
	cursorCreate(s_pMapView, 0, "data/crosshair.bm", 0);

	prepareMapData();

	tileBufferRedrawAll(s_pMapBuffer);

	systemSetDmaBit(DMAB_SPRITE, 1);
	viewLoad(s_pMapView);

	systemUnuse();

	logBlockEnd("gsMapCreate()");
}

void gsMapLoop() {
	if (keyUse(KEY_ESCAPE) || keyUse(KEY_BACKSPACE)) {
		statePop(g_pGameStateManager);
		return;
	}

	if (keyUse(KEY_TAB)) {
		debugToggle();
	}

	handleCameraScrolling();

	debugColor(0x800);
	cursorUpdate();
	viewProcessManagers(s_pMapView);
	copProcessBlocks();
	debugColor(s_pMapVPort->pPalette[0]);

	vPortWaitForEnd(s_pMapVPort);
}

void gsMapDestroy() {
	logBlockBegin("gsMapCreate()");

	systemUse();

	viewDestroy(s_pMapView);

	bitmapDestroy(s_pTileSet);

	cursorDestroy();

	systemUnuse();

	logBlockEnd("gsMapCreate()");
}

void prepareMapData() {
	for (UBYTE i = 0; i < MAP_SIZE; ++i) {
		for (UBYTE j = 0; j < MAP_SIZE; ++j) {
			s_pMapBuffer->pTileData[i][j] = (i % 2) + ((j % 2) * 2);
		}
	}

	s_pMapBuffer->pTileData[3][1] = (15 * TILESET_ROTATION_COUNT) + 2;
	s_pMapBuffer->pTileData[4][1] = (20 * TILESET_ROTATION_COUNT) + 1;
	s_pMapBuffer->pTileData[5][1] = (15 * TILESET_ROTATION_COUNT) + 2;

	s_pMapBuffer->pTileData[2][2] = (8 * TILESET_ROTATION_COUNT) + 0;
	s_pMapBuffer->pTileData[3][2] = (3 * TILESET_ROTATION_COUNT) + 0;
	s_pMapBuffer->pTileData[4][2] = (7 * TILESET_ROTATION_COUNT) + 2;
	s_pMapBuffer->pTileData[5][2] = (3 * TILESET_ROTATION_COUNT) + 0;
	s_pMapBuffer->pTileData[6][2] = (8 * TILESET_ROTATION_COUNT) + 1;

	s_pMapBuffer->pTileData[1][3] = (15 * TILESET_ROTATION_COUNT) + 1;
	s_pMapBuffer->pTileData[2][3] = (3 * TILESET_ROTATION_COUNT) + 0;
	s_pMapBuffer->pTileData[3][3] = (3 * TILESET_ROTATION_COUNT) + 0;
	s_pMapBuffer->pTileData[4][3] = (3 * TILESET_ROTATION_COUNT) + 1;
	s_pMapBuffer->pTileData[5][3] = (3 * TILESET_ROTATION_COUNT) + 0;
	s_pMapBuffer->pTileData[6][3] = (3 * TILESET_ROTATION_COUNT) + 1;
	s_pMapBuffer->pTileData[7][3] = (15 * TILESET_ROTATION_COUNT) + 3;

	s_pMapBuffer->pTileData[1][4] = (20 * TILESET_ROTATION_COUNT) + 0;
	s_pMapBuffer->pTileData[2][4] = (11 * TILESET_ROTATION_COUNT) + 3;
	s_pMapBuffer->pTileData[3][4] = (3 * TILESET_ROTATION_COUNT) + 0;
	s_pMapBuffer->pTileData[4][4] = (7 * TILESET_ROTATION_COUNT) + 0;
	s_pMapBuffer->pTileData[5][4] = (3 * TILESET_ROTATION_COUNT) + 0;
	s_pMapBuffer->pTileData[6][4] = (10 * TILESET_ROTATION_COUNT) + 2;
	s_pMapBuffer->pTileData[7][4] = (20 * TILESET_ROTATION_COUNT) + 0;

	s_pMapBuffer->pTileData[2][5] = (20 * TILESET_ROTATION_COUNT) + 1;
	s_pMapBuffer->pTileData[3][5] = (15 * TILESET_ROTATION_COUNT) + 0;
	s_pMapBuffer->pTileData[4][5] = (20 * TILESET_ROTATION_COUNT) + 1;
	s_pMapBuffer->pTileData[5][5] = (15 * TILESET_ROTATION_COUNT) + 0;
	s_pMapBuffer->pTileData[6][5] = (20 * TILESET_ROTATION_COUNT) + 1;
}

void handleCameraScrolling() {
	BYTE bCameraScrollHorizontal = keyCheck(KEY_RIGHT) - keyCheck(KEY_LEFT);
	BYTE bCameraScrollVertical = keyCheck(KEY_DOWN) - keyCheck(KEY_UP);
	UBYTE ubSpeed = keyCheck(KEY_LSHIFT) ? 2 : 1;

	UWORD uwMouseX = mouseGetX(MOUSE_PORT_1);
	UWORD uwMouseY = mouseGetY(MOUSE_PORT_1);

	bCameraScrollHorizontal += WINDOW_SCREEN_WIDTH - MAP_MOUSE_SCROLL_MARGIN <= uwMouseX;
	bCameraScrollHorizontal -= uwMouseX <= MAP_MOUSE_SCROLL_MARGIN;

	bCameraScrollVertical += WINDOW_SCREEN_HEIGHT - MAP_MOUSE_SCROLL_MARGIN <= uwMouseY;
	bCameraScrollVertical -= uwMouseY <= MAP_MOUSE_SCROLL_MARGIN;

	cameraMoveBy(
		s_pMapBuffer->pCamera,
		CLAMP(bCameraScrollHorizontal, -1, 1) * ubSpeed,
		CLAMP(bCameraScrollVertical, -1, 1) * ubSpeed
	);
}
