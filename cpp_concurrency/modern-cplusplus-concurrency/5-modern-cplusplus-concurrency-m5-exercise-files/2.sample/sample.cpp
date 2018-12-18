#include "precompiled.h"

using namespace KennyKerr;

auto __stdcall work(PTP_CALLBACK_INSTANCE instance, void * args) -> void
{
    VERIFY(CallbackMayRunLong(instance));

    auto & id = *static_cast<DWORD *>(args);

    id = GetCurrentThreadId();

    TRACE(L"work %d\n", GetCurrentThreadId());
}

auto main() -> int
{
    auto id = DWORD {};

    if (TrySubmitThreadpoolCallback(work,
                                    &id,
                                    nullptr))
    {
        TRACE(L"submitted!\n");
    }

    Sleep(100);

    TRACE(L"work completed on %d\n", id);
}
