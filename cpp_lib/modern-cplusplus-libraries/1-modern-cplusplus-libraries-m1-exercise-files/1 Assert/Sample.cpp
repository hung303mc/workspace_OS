#include "Precompiled.h"

#include <crtdbg.h>

#define ASSERT _ASSERTE

static_assert(sizeof(float) == 4, "can't serialize floats!");

auto main() -> int
{
    ASSERT(4 == 5);
}
