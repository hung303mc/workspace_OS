#include "Precompiled.h"
#include <vector>

using namespace std;

auto main() -> int
{
    auto c = vector<int> {};

    ASSERT(c.empty());
    ASSERT(c.size() == 0);

    c = vector<int> { 1, 2, 3, 4, 5 };

    ASSERT(!c.empty());
    ASSERT(c.size() == 5);

    c = vector<int> { begin(c), end(c) };

    ASSERT(!c.empty());
    ASSERT(c.size() == 5);

    c = vector<int> { begin(c) + 1, end(c) - 1 };

    ASSERT(c.size() == 3);

    ASSERT(c[0] == 2);
    ASSERT(c[1] == 3);
    ASSERT(c[2] == 4);

    c = vector<int>(10, 123);

    ASSERT(c.size() == 10);

    for (auto e : c)
    {
        ASSERT(e == 123);
    }

    c = vector<int> { 1, 2, 3, 4, 5 };

    for (auto e : c)
    {
        TRACE(L"%d\n", e);
    }

    for (auto i = begin(c); i != end(c); ++i)
    {
        TRACE(L"%d\n", *i);
    }

    for (auto i = rbegin(c); i != rend(c); ++i)
    {
        TRACE(L"%d\n", *i);
    }

    for (auto i = 0u; i != c.size(); ++i)
    {
        TRACE(L"%d\n", c[i]);
    }

    c.emplace_back(6);
    c.emplace(begin(c), 0);

    TRACE(L"\n");

    for (auto e : c)
    {
        TRACE(L"%d\n", e);
    }
}
