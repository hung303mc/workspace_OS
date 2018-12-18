#include "precompiled.h"

using namespace std;
using namespace KennyKerr;

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
        h = move(other.h);
        return *this;
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

auto main() -> int
{
    auto e = event { event_type::manual_reset };

    ASSERT(!e.wait(0));

    e.set();

    ASSERT(e.wait());

    e.clear();

    ASSERT(!e.wait(0));

    auto e2 = move(e);

    e2.set();

    e.set();
}
