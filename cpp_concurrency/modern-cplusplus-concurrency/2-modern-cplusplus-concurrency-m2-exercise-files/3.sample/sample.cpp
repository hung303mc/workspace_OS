#include "precompiled.h"
#include <process.h>

using namespace KennyKerr;

auto __stdcall work(void *) -> unsigned
{
    TRACE(L"work!\n");
    return 0;
}

auto main() -> int
{
    auto t = null_handle
    {
        reinterpret_cast<HANDLE>(_beginthreadex(nullptr,
                       0,
                       work,
                       nullptr,
                       0,
                       nullptr))
    };

    if (t)
    {
        VERIFY_(WAIT_OBJECT_0, WaitForSingleObject(t.get(), INFINITE));
    }
}
