#ifndef GUARD_FADE_H
#define GUARD_FADE_H

#include <ace/types.h>

/* Types */

typedef enum _tFadeState {
	FADE_STATE_OUT, // Faded to black
	FADE_STATE_IN, // Original palette
} tFadeState;

/* Globals */

/* Functions */

/**
 * @brief Returns TRUE, if provided fade is at desired state. Returns FALSE
 * if not, makes step of animation to ultimately meet state requirements in
 * next game frames. It ment be called on each game loop in IF statements as
 * guard which brings neat usage.
 */
UBYTE fadeRequestState(tFadeState eState);

#endif // GUARD_FADE_H
