#include "precompiled.h"

using namespace KennyKerr;

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

auto main() -> int
{
    auto lock = slim_lock {};

    lock.enter();
    // protected
    lock.exit();   
}
