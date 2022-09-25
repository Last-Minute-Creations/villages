#ifndef GUARD_FADE_H
#define GUARD_FADE_H

#include <ace/types.h>

#define FADE_MAX_LEVEL 0xF

typedef enum _tFadeTarget {
    FADE_TARGET_OUT, // Faded to black
    FADE_TARGET_IN, // Faded to target palette
} tFadeTarget;

typedef enum _tFadeState {
	FADE_STATE_MORPHING,
	FADE_STATE_MORPHING_DONE,
} tFadeState;

tFadeState fadeGetState(void);

tFadeTarget fadeGetTarget(void);

UBYTE fadeGetLevel(void);

void fadeMorphTo(tFadeTarget eTarget);

void fadeProcess(void);

#endif // GUARD_FADE_H
