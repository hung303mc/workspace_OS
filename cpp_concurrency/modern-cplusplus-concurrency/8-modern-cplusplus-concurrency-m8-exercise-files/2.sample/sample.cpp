#include "precompiled.h"

using namespace std;

auto do_stuff(int value) -> void
{
    TRACE(L"stuff: %d\n", value);
}

auto main() -> int
{
    auto t = thread {};

    ASSERT(t.get_id() == thread::id {});

    ASSERT(!t.joinable());

    t = thread { do_stuff, 123 };

    ASSERT(t.get_id() != thread::id {});

    ASSERT(t.joinable());

    auto h = t.native_handle();
    TRACE(L"id = %d\n", GetThreadId(h));

    t.join();

    ASSERT(t.get_id() == thread::id {});
    ASSERT(!t.joinable());

    h = t.native_handle();
    TRACE(L"id = %d\n", GetThreadId(h));

    t = thread
    {
        [] { TRACE(L"another thread!\n"); }
    };

    t.detach();
}
