#include "precompiled.h"

using namespace KennyKerr;

class environment
{
    TP_CALLBACK_ENVIRON m_handle;

public:

    environment(environment const &) = delete;
    environment(environment &&) = delete;
    auto operator=(environment const &) -> environment & = delete;
    auto operator=(environment &&) -> environment & = delete;

    environment() throw()
    {
        InitializeThreadpoolEnvironment(&m_handle);
    }

    ~environment() throw()
    {
        DestroyThreadpoolEnvironment(&m_handle);
    }

    auto get() throw() -> PTP_CALLBACK_ENVIRON
    {
        return &m_handle;
    }
};

auto __stdcall work(PTP_CALLBACK_INSTANCE instance, void * args) -> void
{
    //VERIFY(CallbackMayRunLong(instance));

    auto & id = *static_cast<DWORD *>(args);

    id = GetCurrentThreadId();

    TRACE(L"work %d\n", GetCurrentThreadId());
}

auto main() -> int
{
    auto e = environment {};
    SetThreadpoolCallbackRunsLong(e.get());

    auto id = DWORD {};

    if (TrySubmitThreadpoolCallback(work,
                                    &id,
                                    e.get()))
    {
        TRACE(L"submitted!\n");
    }

    Sleep(100);

    TRACE(L"work completed on %d\n", id);
}
