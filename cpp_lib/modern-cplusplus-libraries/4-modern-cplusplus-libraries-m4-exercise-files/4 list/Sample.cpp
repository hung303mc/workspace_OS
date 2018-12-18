#include "Precompiled.h"
#include <list>

using namespace std;

auto main() -> int
{
    auto c = list<int> {};

    ASSERT(c.empty());
    ASSERT(c.size() == 0);

    c = list<int> { 1, 2, 3, 4, 5 };

    ASSERT(!c.empty());
    ASSERT(c.size() == 5);

    c = list<int> { begin(c), end(c) };

    ASSERT(!c.empty());
    ASSERT(c.size() == 5);

    c = list<int> { ++begin(c), --end(c) };

    ASSERT(c.size() == 3);

    auto v = vector<int> { 1, 2, 3 };
    c = list<int> { begin(v) + 1, end(v) - 1 };

    ASSERT(c.size() == 1);

    ASSERT(c.front() == 2);

    c = list<int> (10);
    ASSERT(c.size() == 10);

    for (auto e : c)
    {
        ASSERT(e == 0);
    }

    c = list<int> (10, 123);

    ASSERT(c.size() == 10);

    for (auto e : c)
    {
        ASSERT(e == 123);
    }

    c = list<int> { 1, 2, 3, 4, 5 };

    for (auto i = rbegin(c); i != rend(c); ++i)
    {
        TRACE(L"%d\n", *i);
    }

    c.emplace_back(6);
    c.emplace_front(0);

    c.emplace(begin(c), -1);
    c.emplace(end(c), 7);

    for (auto e : c)
    {
        TRACE(L"%d\n", e);
    }

    c.erase(begin(c));

    c.pop_front();

    c.erase(--end(c));

    c.pop_back();

    ASSERT(c.size() == 5);

    c.erase(++begin(c), --end(c));

    ASSERT(c.size() == 2);
    ASSERT(c.front() == 1);
    ASSERT(c.back() == 5);

    c.insert(++begin(c), { 2, 3, 4 });

    auto pos = ++begin(c);
    ASSERT(*pos == 2);

    c.reverse();

    ASSERT(*pos == 2);

    c.sort();

    ASSERT(*pos == 2);

    c.remove_if([] (int value)
    {
        return value & 1;
    });

    TRACE(L"\n");

    for (auto e : c)
    {
        TRACE(L"%d\n", e);
    }
}
