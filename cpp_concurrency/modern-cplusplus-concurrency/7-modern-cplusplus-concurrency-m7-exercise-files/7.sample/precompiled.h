#pragma once

#include "debug.h"
#include "handle.h"
#include <memory>
#include <deque>
#include <vector>
#include <iterator>
#include <ppl.h>
#include <algorithm>
#include <numeric>
#include <concurrent_vector.h>
#include <concurrent_queue.h>

namespace KennyKerr
{
    namespace Details
    {
        inline auto pack(HANDLE *) -> void {}

        template <typename T, typename... Args>
        auto pack(HANDLE * left,
                  T const & right,
                  Args const & ... args) -> void
        {
            *left = right.get();
            pack(++left, args...);
        }

        using thread_callback = void (*)();

        inline auto __stdcall thread_wrapper(void * args) -> DWORD
        {
            auto callback = static_cast<thread_callback>(args);

            callback();

            return 0;
        }
    }

    struct windows_exception
    {
        DWORD error;

        explicit windows_exception(DWORD const value = GetLastError()) :
            error { value }
        {}
    };

    inline auto wait_one(HANDLE const h,
                         DWORD const milliseconds = INFINITE) -> bool
    {
        auto const result = WaitForSingleObject(h, milliseconds);

        if (result == WAIT_OBJECT_0) return true;

        if (result == WAIT_TIMEOUT) return false;

        throw windows_exception();
    }

    template <typename... Args>
    auto wait_all(Args const & ... args) -> void
    {
        HANDLE handles[sizeof...(Args)];
        Details::pack(handles, args...);

        VERIFY_(WAIT_OBJECT_0, WaitForMultipleObjects(sizeof...(Args),
                                                      handles,
                                                      true,
                                                      INFINITE));
    }

    enum class event_type
    {
        auto_reset,
        manual_reset
    };

    class event
    {
        null_handle h;

    public:

        event(event const &) = delete;
        auto operator=(event const &) -> event & = delete;
        ~event() = default;

        explicit event(event_type type) :
            h { CreateEvent(nullptr, static_cast<BOOL>(type), false, nullptr) }
        {
            if (!h) throw windows_exception();
        }

        event(event && other) throw() :
            h { other.h.release() }
        {}

        auto operator=(event && other) throw() -> event &
        {
            h = std::move(other.h);
        }

        auto set() throw() -> void
        {
            VERIFY(SetEvent(h.get()));
        }

        auto clear() throw() -> void
        {
            VERIFY(ResetEvent(h.get()));
        }

        auto wait(DWORD const milliseconds = INFINITE) throw() -> bool
        {
            auto const result = WaitForSingleObject(h.get(), milliseconds);

            ASSERT(result == WAIT_OBJECT_0 || result == WAIT_TIMEOUT);

            return result == WAIT_OBJECT_0;
        }

        auto get() const throw() -> HANDLE
        {
            return h.get();
        }
    };

    template <typename T>
    class auto_lock
    {
        using UnlockAction = void (T::*)();

        T & m_lock;
        UnlockAction m_action;

    public:

        auto_lock(auto_lock const &) = delete;
        auto_lock(auto_lock &&) = delete;
        auto operator=(auto_lock const &) -> auto_lock & = delete;
        auto operator=(auto_lock &&) -> auto_lock & = delete;

        explicit auto_lock(T & lock, UnlockAction action) throw() :
            m_lock { lock },
            m_action { action }
        {
        }

        ~auto_lock() throw()
        {
            (m_lock.*m_action)();
        }
    };

    class slim_lock
    {
        SRWLOCK m_lock;

    public:

        slim_lock(slim_lock const &) = delete;
        slim_lock(slim_lock &&) = delete;
        auto operator=(slim_lock const &) -> slim_lock & = delete;
        auto operator=(slim_lock &&) -> slim_lock & = delete;

        slim_lock() throw() :
            m_lock {}
        {}

        auto enter() throw() -> void
        {
            AcquireSRWLockExclusive(&m_lock);
        }

        auto exit() throw() -> void
        {
            ReleaseSRWLockExclusive(&m_lock);
        }

        auto native_handle() throw() -> SRWLOCK *
        {
            return &m_lock;
        }

        auto enter_shared() throw() -> void
        {
            AcquireSRWLockShared(&m_lock);
        }

        auto exit_shared() throw() -> void
        {
            ReleaseSRWLockShared(&m_lock);
        }

        auto get_exclusive() throw() -> auto_lock<slim_lock>
        {
            enter();
            return auto_lock<slim_lock>(*this, &slim_lock::exit);
        }

        auto get_shared() throw() -> auto_lock<slim_lock>
        {
            enter_shared();
            return auto_lock<slim_lock>(*this, &slim_lock::exit_shared);
        }
    };

    template <typename F>
    auto make_thread(F callback) throw() -> null_handle
    {
        return null_handle
        {
            CreateThread(nullptr,
                         0,
                         Details::thread_wrapper,
                         static_cast<Details::thread_callback>(callback),
                         0,
                         nullptr)
        };
    }

    class condition_variable
    {
        CONDITION_VARIABLE m_cv;

    public:

        condition_variable(condition_variable const &) = delete;
        condition_variable(condition_variable &&) = delete;
        auto operator=(condition_variable const &) -> condition_variable & = delete;
        auto operator=(condition_variable &&) -> condition_variable & = delete;

        condition_variable() throw() :
            m_cv {}
        {}

        template <typename T>
        auto wait_while(slim_lock & lock,
                        T predicate) throw() -> void
        {
            while (predicate())
            {
                VERIFY(SleepConditionVariableSRW(&m_cv,
                                                 lock.native_handle(),
                                                 INFINITE,
                                                 0));
            }
        }

        auto wake_one() throw() -> void
        {
            WakeConditionVariable(&m_cv);
        }

        auto wake_all() throw() -> void
        {
            WakeAllConditionVariable(&m_cv);
        }
    };

    template <typename T>
    class blocking_queue
    {
        std::deque<T> m_queue;
        slim_lock m_lock;
        condition_variable m_cv;

    public:

        using value_type = T;

        auto push_back(T const & value) -> void
        {
            auto guard = m_lock.get_exclusive();

            m_queue.push_back(value);

            m_cv.wake_one();
        }

        auto pop() -> T
        {
            auto guard = m_lock.get_exclusive();

            m_cv.wait_while(m_lock, [&]
            {
                return m_queue.empty();
            });

            T value = m_queue.front();
            m_queue.pop_front();
            return value;
        }

        auto unsafe_empty() const throw() -> bool
        {
            return m_queue.empty();
        }
    };

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

    struct cleanup_group_traits
    {
        using pointer = PTP_CLEANUP_GROUP;

        static auto invalid() throw() -> pointer
        {
            return nullptr;
        }

        static auto close(pointer value) throw() -> void
        {
            CloseThreadpoolCleanupGroup(value);
        }
    };

    using cleanup_group = unique_handle<cleanup_group_traits>;
}
