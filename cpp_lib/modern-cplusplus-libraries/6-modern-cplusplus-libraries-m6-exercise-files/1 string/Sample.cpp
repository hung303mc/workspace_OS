#include "Precompiled.h"
#include <string>

using namespace std;

auto trim(string const & s) -> string
{
    auto front = find_if_not(begin(s), end(s), isspace);
    auto back = find_if_not(rbegin(s), rend(s), isspace);

    return string { front, back.base() };
}

auto main() -> int
{
    auto s = string {};

    s = basic_string<char> {};

    auto ws = wstring {};

    ws = basic_string<wchar_t> {};

    static_assert(sizeof(string::value_type) == 1, "1 byte");
    static_assert(sizeof(wstring::value_type) == 2, "2 bytes");

    s = string { "cluck!" };

    ASSERT(!s.empty());
    ASSERT(s.size() == 6);

    TRACE(L"%S\n", s.c_str());

    s = string { 'C', 'l', 'u', 'c', 'k' };

    TRACE(L"%S\n", s.c_str());

    s.clear();

    ASSERT(s.empty());
    ASSERT(s.size() == 0);
    ASSERT(0 == strcmp(s.c_str(), ""));

    s = string { "The hen clucked!" };

    s = string ( s, 4, 3 );

    ASSERT(s == "hen");

    for (auto c : s)
    {
        TRACE(L"%C\n", c);
    }

    auto hen = string { "Matilda" };
    auto pasture = string { "Tomatoes" };
    auto id = hen + "@" + pasture;

    TRACE(L"%S\n", id.c_str());

    auto pos = id.find('@');

    auto domain = id.substr(pos);

    ASSERT(domain == "@Tomatoes");

    auto trimmed = trim(" \t Matilda the hen \r\n ");

    ASSERT(trimmed == "Matilda the hen");
}
