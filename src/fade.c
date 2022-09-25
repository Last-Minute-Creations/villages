#include "fade.h"

static tFadeTarget s_eTarget = FADE_TARGET_OUT;
static tFadeState s_eState = FADE_STATE_MORPHING_DONE;
static UBYTE s_ubLevel = 0;

tFadeTarget fadeGetTarget(void) {
	return s_eTarget;
}

tFadeState fadeGetState(void) {
	return s_eState;
}

UBYTE fadeGetLevel(void) {
	return s_ubLevel;
}

void fadeMorphTo(tFadeTarget eTarget) {
    if (s_eTarget != eTarget) {
        s_eTarget = eTarget;
        s_eState = FADE_STATE_MORPHING;
    }
}

void fadeProcess(void) {
    if (s_eState == FADE_STATE_MORPHING_DONE) {
        return;
    }

	if (s_eTarget == FADE_TARGET_IN) {
        if (s_ubLevel < FADE_MAX_LEVEL) {
			++s_ubLevel;
		}
		else {
			s_eState = FADE_STATE_MORPHING_DONE;
		}
	}
	else {
		if (0 < s_ubLevel) {
			--s_ubLevel;
		}
		else {
			s_eState = FADE_STATE_MORPHING_DONE;
		}
	}
}
