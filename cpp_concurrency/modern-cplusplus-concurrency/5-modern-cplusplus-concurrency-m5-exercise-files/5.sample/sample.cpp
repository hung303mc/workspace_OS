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


class serial_pool
{
    pool m_pool;
    environment m_environment;

    using StatelessFunction = void(*)();

    static auto __stdcall callback(PTP_CALLBACK_INSTANCE, void * args) -> void
    {
        auto w = static_cast<StatelessFunction>(args);

        w();
    }

public:

    serial_pool() :
        m_pool { CreateThreadpool(nullptr) }
    {
        SetThreadpoolThreadMaximum(m_pool.get(), 1);

        if (!SetThreadpoolThreadMinimum(m_pool.get(), 1))
        {
            throw windows_exception();
        }

        SetThreadpoolCallbackPool(m_environment.get(), m_pool.get());
    }

    template <TP_CALLBACK_PRIORITY P = TP_CALLBACK_PRIORITY_NORMAL, typename W>
    auto submit(W work) -> void
    {
        SetThreadpoolCallbackPriority(m_environment.get(), P);

        if (!TrySubmitThreadpoolCallback(callback,
                                         static_cast<StatelessFunction>(work),
                                         m_environment.get()))
        {
            throw windows_exception();
        }
    }
};

auto main() -> int
{
    auto sp = serial_pool {};

    for (auto i = 0; i != 10; ++i)
    {
        sp.submit([]
        {
            TRACE(L"normal %d\n", GetCurrentThreadId());
        });

        sp.submit<TP_CALLBACK_PRIORITY_LOW>([]
        {
            TRACE(L"low %d\n", GetCurrentThreadId());
        });

        sp.submit<TP_CALLBACK_PRIORITY_HIGH>([]
        {
            TRACE(L"high %d\n", GetCurrentThreadId());
        });
    }

    Sleep(INFINITE);
}
