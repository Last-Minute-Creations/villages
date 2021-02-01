#include "gamestates/map.h"

#include <ace/managers/log.h>
#include <ace/managers/key.h>
#include <ace/managers/state.h>
#include <ace/managers/game.h>
#include <ace/managers/system.h>
#include <ace/managers/log.h>
#include <ace/managers/viewport/tilebuffer.h>

#include <ace/utils/bitmap.h>
#include <ace/utils/palette.h>

#include "game.h"
#include "../debug.h"
#include "../tileset.h"

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

	for (UBYTE i = 0; i < MAP_SIZE; ++i) {
		for (UBYTE j = 0; j < MAP_SIZE; ++j) {
			if(i == 0 || j == 0 || i == MAP_SIZE-1 || j == MAP_SIZE-1) {
				s_pMapBuffer->pTileData[i][j] = (i % 2) + ((j % 2) * 2);
			}
		}
	}
	s_pMapBuffer->pTileData[0][0] = 6;

	tileBufferRedrawAll(s_pMapBuffer);

	viewLoad(s_pMapView);

	systemUnuse();

	logBlockEnd("gsMapCreate()");
}

void gsMapLoop() {
	if (keyUse(KEY_ESCAPE) || keyUse(KEY_BACKSPACE)) {
		statePop(g_pGameStateManager);
		return;
	}

	if (KEY_TAB) {
		debugToggle();
	}

	UBYTE ubSpeed = keyCheck(KEY_LSHIFT) ? 2 : 1;

	cameraMoveBy(
		s_pMapBuffer->pCamera,
		(keyCheck(KEY_RIGHT) - keyCheck(KEY_LEFT)) * ubSpeed,
		(keyCheck(KEY_DOWN) - keyCheck(KEY_UP)) * ubSpeed
	);

	debugColor(0x800);
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

	systemUnuse();

	logBlockEnd("gsMapCreate()");
}
