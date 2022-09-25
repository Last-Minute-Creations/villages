#include "utils.h"

UWORD ceilToFactor(UWORD uwValue, UWORD uwFactor) {
    UWORD uwTmp = uwValue + uwFactor - 1;
    return uwTmp - (uwTmp % uwFactor);
}
