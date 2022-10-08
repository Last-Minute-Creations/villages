#ifndef GUARD_UTILS_H
#define GUARD_UTILS_H

#include <ace/types.h>

/* Types */

/* Globals */

/* Functions */

#define CMP(a, b) (((a) > (b)) - ((a) < (b)))

UWORD ceilToFactor(UWORD uwValue, UWORD uwFactor);

#endif // GUARD_UTILS_H
