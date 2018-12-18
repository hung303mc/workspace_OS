#include "precompiled.h"

using namespace std;
using namespace concurrency;

auto producer(concurrent_queue<int> & c) -> void
{
    for (auto i = 0; i != 10; ++i)
    {
        wait(1);
        c.push(i);
    }
}

auto consumer(concurrent_queue<int> & c) -> void
{
    for (auto i = 0; i != 10; ++i)
    {
        auto value = int {};
        auto faults = int {};

        while (!c.try_pop(value))
        {
            ++faults;
            wait(1);
        }

        TRACE(L"%d (%d)\n", value, faults);
    }
}

auto main() -> int
{
    auto c = concurrent_queue<int> {};

    ASSERT(c.empty());

    ASSERT(c.unsafe_size() == 0);

    parallel_invoke
    (
        [&] { producer(c); },
        [&] { consumer(c); }
    );

    ASSERT(c.empty());
}
