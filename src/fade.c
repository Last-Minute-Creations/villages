#include "fade.h"

#include <ace/managers/log.h>
#include <ace/utils/palette.h>

#include "game.h"

/* Types */

#define FADE_MIN_LEVEL 0x0
#define FADE_MAX_LEVEL 0xF
#define FADE_SPEED 1

/* Globals */

/* Statics */

static tFadeState s_eState = FADE_STATE_OUT;
static UBYTE s_ubCurrentLevel = FADE_MIN_LEVEL;
static UBYTE s_ubTargetLevel = FADE_MIN_LEVEL;

/* Functions */

void fadeSetTarget(tFadeState eFadeState) {
	s_eState = eFadeState;

	switch (eFadeState) {
		case FADE_STATE_OUT:
			s_ubTargetLevel = FADE_MIN_LEVEL;
			break;
		case FADE_STATE_IN:
			s_ubTargetLevel = FADE_MAX_LEVEL;
			break;
	}

	logWrite("Fade started animation to %d", eFadeState);
}

UBYTE fadeRequestState(tFadeState eFadeState) {
	if (s_ubCurrentLevel == s_ubTargetLevel) {
		// Animation is complete, lets check if we're at requested state
		if (s_eState == eFadeState) {
			return TRUE;
		}

		// Animation is complete, but we're not at requested state, lets take current state closer to requested state
		fadeSetTarget(eFadeState);
	}

	// Lets process animation closer to target
	{
		s_ubCurrentLevel += (
			s_ubCurrentLevel < s_ubTargetLevel
			? MIN(s_ubTargetLevel - s_ubCurrentLevel, FADE_SPEED)
			: MAX(s_ubTargetLevel - s_ubCurrentLevel, -FADE_SPEED)
		);

		paletteDim(g_pPalette, (UWORD *) g_pCustom->color, GAME_COLOR_COUNT, s_ubCurrentLevel);
	}

	// Current state is animating, let's wait for animation completion
	return FALSE;
}
