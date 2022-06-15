#ifndef GUARD_GAMESTATE_MENU_CONTROLS_H
#define GUARD_GAMESTATE_MENU_CONTROLS_H

#include <clib/exec_protos.h> // Amiga typedefs
#include <ace/utils/coord.h>

#define CONTROL_BTN
#define CONTROL_SELECT

typedef struct {
	void (*fn)(void);
} tBtnControl;

typedef struct {
	UBYTE ubOptionCount;
	char **pOptionLabels;
	UBYTE *pOptionValues;
} tSelectControl;

typedef struct _tControl {
	struct _tControl *pNext;
	tUwCoordYX sLabelCoord;
	tUwCoordYX sCoord;
	UBYTE ubType;
	union {
		tBtnControl sBtnExtra;
		tSelectControl sSelectExtra;
	} uExtra;
} tControl;

typedef struct {
	UBYTE ubCount;
	tControl *pHead;
} tControlList;

#endif
