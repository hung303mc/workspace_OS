#include "precompiled.h"
#include <wrl.h>

using namespace Microsoft::WRL::Wrappers;

auto main() -> int
{
    CriticalSection lock { 400 };

    ASSERT(lock.IsValid());

    {
        auto guard = lock.Lock();
        // protected
    }

    {
        auto guard = lock.TryLock();

        if (guard.IsLocked())
        {
            // protected
        }

        // possibly protected!?
    }
}
