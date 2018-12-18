#include "precompiled.h"

using namespace std;

auto main() -> int
{
    auto m = mutex {};

    m.lock();
    // protected
    m.unlock();

    if (m.try_lock())
    {
        // protected
        m.unlock();
    }

    {
        auto guard = lock_guard<mutex> { m };
        // protected
    }
}
