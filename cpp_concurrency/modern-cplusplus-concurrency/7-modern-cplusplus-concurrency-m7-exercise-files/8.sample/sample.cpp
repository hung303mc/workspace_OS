#include "precompiled.h"

using namespace std;
using namespace concurrency;

auto main() -> int
{
    critical_section cs;

    cs.lock();
    // protected
    cs.unlock();

    if (cs.try_lock())
    {
        // protected
        cs.unlock();
    }

    {
        critical_section::scoped_lock guard(cs);
        // protected
    }
}
