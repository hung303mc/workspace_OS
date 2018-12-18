#include "precompiled.h"

using namespace std;
using namespace concurrency;

auto main() -> int
{
    auto v = vector<int> { 1, 2, 3, 4, 5 };

    parallel_transform(begin(v), end(v), begin(v), [](int e)
    {
        return e * e;
    });

    for (auto e : v)
    {
        printf("%d ", e);
    }

    printf("\n");
}
