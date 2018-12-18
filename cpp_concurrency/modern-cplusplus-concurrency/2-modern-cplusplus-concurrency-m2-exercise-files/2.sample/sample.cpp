#include "precompiled.h"

using namespace KennyKerr;

struct windows_exception
{
    DWORD error;

    explicit windows_exception(DWORD const value = GetLastError()) :
        error { value }
    {}
};

auto wait_one(HANDLE const h,
              DWORD const milliseconds = INFINITE) -> bool
{
    auto const result = WaitForSingleObject(h, milliseconds);

    if (result == WAIT_OBJECT_0) return true;

    if (result == WAIT_TIMEOUT) return false;

    throw windows_exception();
}

auto __stdcall work(void *) -> DWORD
{
    TRACE(L"work!\n");
    return 123;
}

auto main() -> int
{
    auto t = null_handle
    {
        CreateThread(nullptr,
                     0, 
                     work, 
                     nullptr,
                     0,
                     nullptr)
    };

    if (t)
    {
        if (wait_one(t.get(), 0)) TRACE(L"before!\n");

        VERIFY(wait_one(t.get()));

        if (wait_one(t.get(), 0)) TRACE(L"after!\n");

        auto exit = DWORD {};

        VERIFY(GetExitCodeThread(t.get(), &exit));

        ASSERT(exit == 123); // don't use STILL_ACTIVE (259)
    }
}
