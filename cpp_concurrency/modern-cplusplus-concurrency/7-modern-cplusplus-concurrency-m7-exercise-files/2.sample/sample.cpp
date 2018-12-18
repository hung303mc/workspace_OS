#include "precompiled.h"

using namespace std;
using namespace concurrency;

auto main() -> int
{
    auto v = vector<int> { 1, 2, 3, 4, 5 };

    for_each(begin(v), end(v), [](int e)
    {
        printf("%d ", e);
    });

    printf("\n");

    for (int i = 0; i != 2; ++i)
    {
        parallel_for_each(begin(v), end(v), [](int e)
        {
            printf("%d %d\n", e, GetCurrentThreadId());
        });

        printf("\n");
    }
}
