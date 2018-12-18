#include "precompiled.h"

using namespace std;
using namespace concurrency;

auto main() -> int
{
    auto c = concurrent_vector<int> {};

    ASSERT(c.empty());

    ASSERT(c.size() == 0);

    auto l = { 1, 2, 3, 4, 5 };

    c = concurrent_vector<int> { begin(l), end(l) };

    ASSERT(!c.empty());

    ASSERT(c.size() == 5);

    c = concurrent_vector<int> { begin(c) + 1, end(c) - 1 };

    ASSERT(c.size() == 3);

    ASSERT(c[0] == 2);
    ASSERT(c[1] == 3);
    ASSERT(c[2] == 4);

    c.clear();

    ASSERT(c.empty());

    parallel_for(0, 10, [&](int i)
    {
        c.push_back(i);
    });

    for (auto e : c)
    {
        printf("%d\n", e);
    }
}
