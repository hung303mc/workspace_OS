#include "Precompiled.h"
#include <regex>

using namespace std;

auto main() -> int
{
    auto s = "Call 877-808-2321 to reach Amelia the hen!";

    auto r = regex { R"((\d{3})-(\d{3})-(\d{4}))" };

    auto m = cmatch {};
    //m = match_results<string::const_iterator> {};

    ASSERT(m.empty());

    ASSERT(regex_search(s, m, r));

    ASSERT(!m.empty());

    for (auto & sub : m)
    {
        //auto str = sub.str();

        TRACE(L"[%.*S]\n", sub.length(), &*sub.first);
    }

    auto output = m.format(R"($`<a href="link...">$1 $2 $3</a>$')");

    TRACE(L"%S\n", output.c_str());
}
