#include "precompiled.h"

using namespace std;
using namespace concurrency;

auto main() -> int
{
    for (int i = 0; i != 10; i += 2)
    {
        printf("%d ", i);
    }

    printf("\n");

    for (int i = 0; i != 10; ++i)
    {
        parallel_for(0, 10, 2, [](int i)
        {
            printf("%d ", i);
        });

        printf("\n");
    }
}
