#ifndef GUARD_VILLAGES_GAMESTATES_MAP_H
#define GUARD_VILLAGES_GAMESTATES_MAP_H

#define MAP_SIZE 77
#define MAP_MOUSE_SCROLL_MARGIN 5

void gsMapCreate();
void gsMapLoop();
void gsMapDestroy();

void prepareMapData();
void handleCameraScrolling();

#endif
