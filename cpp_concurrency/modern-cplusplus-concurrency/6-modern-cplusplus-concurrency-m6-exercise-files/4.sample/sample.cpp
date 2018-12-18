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

struct work_traits
{
    using pointer = PTP_WORK;

    static auto invalid() throw() -> pointer
    {
        return nullptr;
    }

    static auto close(pointer value) throw() -> void
    {
        CloseThreadpoolWork(value);
    }
};

using work = unique_handle<work_traits>;


struct wait_traits
{
    using pointer = PTP_WAIT;

    static auto invalid() throw() -> pointer
    {
        return nullptr;
    }

    static auto close(pointer value) throw() -> void
    {
        CloseThreadpoolWait(value);
    }
};

using wait = unique_handle<wait_traits>;

struct registry_key_traits
{
    using pointer = HKEY;

    static auto invalid() throw() -> pointer
    {
        return nullptr;
    }

    static auto close(pointer value) throw() -> void
    {
        VERIFY_(ERROR_SUCCESS, RegCloseKey(value));
    }
};

using registry_key = unique_handle<registry_key_traits>;

auto open_registry_key(HKEY key,
                       wchar_t const * path,
                       REGSAM access) -> registry_key
{
    auto handle = HKEY {};

    auto const result = RegOpenKeyEx(key,
                                     path,
                                     0, 
                                     access,
                                     &handle);

    if (ERROR_SUCCESS != result)
    {
        throw windows_exception(result);
    }

    return registry_key { handle };
}

struct timer_traits
{
    using pointer = PTP_TIMER;

    static auto invalid() throw() -> pointer
    {
        return nullptr;
    }

    static auto close(pointer value) throw() -> void
    {
        CloseThreadpoolTimer(value);
    }
};

using timer = unique_handle<timer_traits>;

auto relative_time(unsigned milliseconds) -> FILETIME
{
    union threadpool_time
    {
        INT64 quad;
        FILETIME ft;
    };

    auto t = threadpool_time
    {
        -static_cast<INT64>(milliseconds) * 10000
    };

    return t.ft;
}

auto __stdcall times_up(PTP_CALLBACK_INSTANCE, void *, PTP_TIMER t) -> void
{
    TRACE(L"it's time!\n");

    auto due_time = relative_time(500);

    SetThreadpoolTimer(t, &due_time, 0, 0);
}

auto main() -> int
{
    auto t = timer
    {
        CreateThreadpoolTimer(times_up,
                              nullptr,
                              nullptr)
    };

    if (!t) throw windows_exception();

    auto due_time = relative_time(5000);

    SetThreadpoolTimer(t.get(), &due_time, 0, 0);

    

    Sleep(INFINITE);
}
