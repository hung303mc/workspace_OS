#include "Precompiled.h"
#include <map>

namespace std
{
    template <typename T>
    auto begin(pair<T, T> const & range) -> T
    {
        return range.first;
    }

    template <typename T>
    auto end(pair<T, T> const & range) -> T
    {
        return range.second;
    }
}

using namespace std;

auto main() -> int
{
    auto c = multimap<int, double>
    {
        { 1, 1.1 },
        { 2, 2.1 },
        { 2, 2.2 },
        { 3, 3.1 }
    };

    ASSERT(!c.empty());
    ASSERT(c.size() == 4);

    auto result = c.emplace(2, 2.3);
    ASSERT(result->first == 2);
    ASSERT(result->second == 2.3);

    for (auto & v : c)
    {
        TRACE(L"%d : %f\n", v.first, v.second);
    }

    auto range = c.equal_range(2);

    for (auto i = range.first; i != range.second; ++i)
    {
        TRACE(L"range %d : %f\n", i->first, i->second);
    }

    for (auto & v : c.equal_range(2))
    {
        TRACE(L"range %d : %f\n", v.first, v.second);
    }
}
