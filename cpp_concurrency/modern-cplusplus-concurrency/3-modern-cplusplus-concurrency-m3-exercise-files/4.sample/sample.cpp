#include "precompiled.h"

using namespace KennyKerr;

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
};

auto main() -> int
{
    auto lock = slim_lock {};

    lock.enter();
    // protected
    lock.exit();
   
}
