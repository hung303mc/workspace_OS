#include "Precompiled.h"
#include <set>

using namespace std;

auto main() -> int
{
    auto c = set<int, less<int>> {};

    ASSERT(c.empty());
    ASSERT(c.size() == 0);

    c = set<int> { 1, 2, 4, 5, 3 };

    ASSERT(!c.empty());
    ASSERT(c.size() == 5);

    for (auto v : c)
    {
        TRACE(L"%d\n", v);
    }

    c.insert(0);
    auto result = c.emplace(6);

    ASSERT(*result.first == 6);
    ASSERT(result.second);

    result = c.emplace(6);

    ASSERT(*result.first == 6);
    ASSERT(!result.second);

    auto next = c.erase(begin(c));

    ASSERT(*next == 1);

    ASSERT(1 == c.erase(6));
    ASSERT(0 == c.erase(123));

    auto i = c.find(3);
    ASSERT(*i == 3);
    ASSERT(end(c) == c.find(123));

    auto upper = c.upper_bound(3);
    ASSERT(*upper == 4);

    auto lower = c.lower_bound(3);
    ASSERT(*lower == 3);

    auto range = c.equal_range(3);
    ASSERT(range.first == lower);
    ASSERT(range.second == upper);

    for (auto i = begin(c); i != lower; ++i)
    {
        TRACE(L"lower %d\n", *i);
    }

    for (auto i = lower; i != upper; ++i)
    {
        TRACE(L"key %d\n", *i);
    }

    for (auto i = upper; i != end(c); ++i)
    {
        TRACE(L"upper %d\n", *i);
    }
}
