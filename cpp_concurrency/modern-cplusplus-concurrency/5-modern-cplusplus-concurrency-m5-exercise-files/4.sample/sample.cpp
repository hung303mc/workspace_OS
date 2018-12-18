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

struct pool_traits
{
    using pointer = PTP_POOL;

    static auto invalid() throw() -> pointer
    {
        return nullptr;
    }

    static auto close(pointer value) throw() -> void
    {
        CloseThreadpool(value);
    }
};

using pool = unique_handle<pool_traits>;

auto __stdcall work(PTP_CALLBACK_INSTANCE instance, void * args) -> void
{
    VERIFY(CallbackMayRunLong(instance));

    auto & id = *static_cast<DWORD *>(args);

    id = GetCurrentThreadId();

    TRACE(L"work %d\n", GetCurrentThreadId());
}

auto main() -> int
{
    auto p = pool
    {
        CreateThreadpool(nullptr)
    };

    if (!p)
    {
        throw windows_exception();
    }

    SetThreadpoolThreadMaximum(p.get(), 1);

    if (!SetThreadpoolThreadMinimum(p.get(), 1))
    {
        throw windows_exception();
    }

    Sleep(1);

    auto e = environment {};
    //SetThreadpoolCallbackRunsLong(e.get());
    SetThreadpoolCallbackPool(e.get(), p.get());

    auto id = DWORD {};

    if (TrySubmitThreadpoolCallback(work,
                                    &id,
                                    e.get()))
    {
        TRACE(L"submitted!\n");
    }

    Sleep(INFINITE);

    TRACE(L"work completed on %d\n", id);
}
