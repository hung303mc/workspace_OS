#include "precompiled.h"

template <typename T>
class auto_lock
{
    T & m_lock;

public:

    auto_lock(auto_lock const &) = delete;
    auto_lock(auto_lock &&) = delete;
    auto operator=(auto_lock const &) -> auto_lock & = delete;
    auto operator=(auto_lock &&) -> auto_lock & = delete;

    explicit auto_lock(T & lock) throw() :
        m_lock { lock }
    {
        m_lock.enter();
    }

    ~auto_lock() throw()
    {
        m_lock.exit();
    }
};

class critical_section
{
    CRITICAL_SECTION m_lock;

public:

    critical_section(critical_section const &) = delete;
    critical_section(critical_section &&) = delete;
    auto operator=(critical_section const &) -> critical_section & = delete;
    auto operator=(critical_section &&) -> critical_section & = delete;

    critical_section() throw()
    {
        InitializeCriticalSection(&m_lock);
    }

    ~critical_section() throw()
    {
        DeleteCriticalSection(&m_lock);
    }

    auto enter() throw() -> void
    {
        EnterCriticalSection(&m_lock);
    }

    auto exit() throw() -> void
    {
        LeaveCriticalSection(&m_lock);
    }
};

auto main() -> int
{
    auto lock = critical_section {};

    {
        auto guard = auto_lock<critical_section> { lock };
        // protected
    }

    lock.enter();
    lock.enter();
    lock.exit();
    lock.exit();
}
