#include "precompiled.h"
#include <wrl.h>

using namespace Microsoft::WRL::Wrappers;

auto main() -> int
{
    SRWLock lock;

    {
        auto guard = lock.LockExclusive();
        // protected
    }

    {
        auto guard = lock.LockShared();
        // protected
    }

    {
        auto guard = lock.TryLockExclusive();

        if (guard.IsLocked())
        {
            // protected
        }

        // possibly protected!?
    }

    {
        auto guard = lock.TryLockShared();

        if (guard.IsLocked())
        {
            // protected
        }

        // possibly protected!?
    }
}
