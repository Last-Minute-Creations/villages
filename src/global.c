#include "global.h"

#include <ace/generic/screen.h>
#include <ace/utils/file.h>

tFont *g_pFont;
tBitMap *g_pCharactersBitMap;
tTextBitMap *g_pTextBitMap;

CHIP UWORD g_pCursorData[288];
CHIP UWORD g_pPawnCursorData[36];

tGameConfig g_sGameConfig = {
	.pPlayersSelected = {1, 1, 1, 1, 0, 0, 0, 0}, // pPlayersSelected
	.ubPlayerCount = 4, // ubPlayerCount
	.pPlayerFirst = 0, // pPlayerFirst
	.ubPawnsPerPlayer = 7, // ubPawnsPerPlayer
	.ubPawnCount = 0, // ubPawnCount
	.pPawnFirst = 0 // pPawnFirst
};

UBYTE g_pPlayerColors[PLAYER_COUNT][2] = {
	{13, 14},
	{15, 16},
	{20, 21},
	{22, 23},
	{24, 25},
	{26, 27},
	{28, 29},
	{30, 31}
};

WORD g_pPointerSpots[8][2] = {
	{0, 0},
	{-6, 0},
	{-11, 0},
	{-15, -6},
	{-15, -11},
	{-9, -15},
	{-2, -15},
	{0, -8}
};

BYTE g_pPointerDirs[3][3] = {
	{POINTER_NW, POINTER_N,  POINTER_NE},
	{POINTER_W,  POINTER_NW, POINTER_E},
	{POINTER_SW, POINTER_S,  POINTER_SE}
};

void globalLoadCursorData(void) {
	const char *szCursorPath = "data/hand.cur";
	tFile *pFile = fileOpen(szCursorPath, "rb");

	if (!pFile) {
		logWrite("ERR: File doesn't exist! '%s'", szCursorPath);
	}

	fileSeek(pFile, 9, FILE_SEEK_CURRENT); // pominiecie naglowka: ubCursorCount + paleta
	fileRead(pFile, g_pCursorData, 576);
	fileClose(pFile);

	const char *szPawnPath = "data/pawn.cur";
	pFile = fileOpen(szPawnPath, "rb");

	if (!pFile) {
		logWrite("ERR: File doesn't exist! '%s'", szPawnPath);
	}

	fileSeek(pFile, 9, FILE_SEEK_CURRENT); // pominiecie naglowka: ubCursorCount + paleta
	fileRead(pFile, g_pPawnCursorData, 72);
	fileClose(pFile);
	// TODO:
	// mouseSetPointer(g_pCursorData, 16, 16, 0, 0);
}

void globalCreate(void) {
	logBlockBegin("globalCreate()");

	globalLoadCursorData();
	g_pFont = fontCreate("data/fonts/arpegius-15.fnt");
	g_pTextBitMap = fontCreateTextBitMap(SCREEN_PAL_WIDTH, g_pFont->uwHeight);
	g_pCharactersBitMap = bitmapCreateFromFile("data/bitmaps/goblins.bm", FALSE);

	logBlockEnd("globalCreate()");
}

void globalDestroy(void) {
	fontDestroy(g_pFont);
	fontDestroyTextBitMap(g_pTextBitMap);
	bitmapDestroy(g_pCharactersBitMap);
}
