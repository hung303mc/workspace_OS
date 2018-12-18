#include "precompiled.h"

using namespace KennyKerr;

auto main() -> int
{
    auto lock = null_handle
    {
        CreateMutex(nullptr,
                    false,
                    nullptr)
    };

    if (!lock)
    {
        throw windows_exception();
    }

    VERIFY(wait_one(lock.get()));

    VERIFY(ReleaseMutex(lock.get()));
}
