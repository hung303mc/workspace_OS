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

auto __stdcall the_wait_is_over(PTP_CALLBACK_INSTANCE,
                                void *,
                                PTP_WAIT,
                                TP_WAIT_RESULT) -> void
{
    TRACE(L"wait no more!\n");
}

auto main() -> int
{
    auto w = wait
    {
        CreateThreadpoolWait(the_wait_is_over,
                             nullptr,
                             nullptr)
    };

    if (!w)
    {
        throw windows_exception();
    }

    auto e = event { event_type::manual_reset };

    SetThreadpoolWait(w.get(), e.get(), nullptr);

    auto k = open_registry_key(HKEY_CURRENT_USER,
                               L"Sample",
                               KEY_NOTIFY);

    auto const filter = REG_NOTIFY_CHANGE_LAST_SET | 
                        REG_NOTIFY_THREAD_AGNOSTIC;

    auto result = RegNotifyChangeKeyValue(k.get(),
                                          false,
                                          filter,
                                          e.get(),
                                          true);

    if (result != ERROR_SUCCESS)
    {
        throw windows_exception(result);
    }

    Sleep(5000);

    SetThreadpoolWait(w.get(), nullptr, nullptr);
    WaitForThreadpoolWaitCallbacks(w.get(), true);
}
