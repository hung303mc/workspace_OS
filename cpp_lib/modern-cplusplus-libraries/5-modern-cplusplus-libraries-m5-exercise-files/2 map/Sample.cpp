#include "Precompiled.h"
#include <map>

using namespace std;

auto main() -> int
{
    auto c = map<int, double> {};

    ASSERT(c.empty());

    c = map<int, double>
    {
        { 1, 1.1 },
        { 4, 4.1 },
        { 2, 2.1 },
        { 3, 3.1 },
        { 5, 5.1 }
    };

    ASSERT(!c.empty());
    ASSERT(c.size() == 5);

    ASSERT(c[4] == 4.1);

    c[6] = 6.1;

    auto v = c[7];
    ASSERT(v == 0.0);

    ASSERT(c.size() == 7);

    auto result = c.insert(make_pair(8, 8.1));
    ASSERT(result.second);

    result = c.emplace(9, 9.1);

    auto i = c.find(6);
    ASSERT(i->first == 6);
    ASSERT(i->second == 6.1);

    c.erase(i);
    c.erase(7);

    for (auto & v : c)
    {
        TRACE(L"%d : %f\n", v.first, v.second);
    }
}
