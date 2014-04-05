#ifndef _UTIL_H
#define _UTIL_H

#include "GeophileException.h"

#define GEOPHILE_ASSERT(condition) { if (!(condition)) throw GeophileException("condition"); }

#endif
