#include "precompiled.h"

using namespace std;
using namespace concurrency;

auto task_1() -> int
{
    TRACE(L"task 1\n");

    wait(5000);

    return 1;
}

auto main() -> int
{
    auto t = task<int> {};

    t = task<int> { task_1 };

    TRACE(L"result %d\n", t.get());

    t = task<int> { [] { TRACE(L"task 2\n"); return 2; } };

    TRACE(L"result %d\n", t.get());

    t = create_task([]
    {
        TRACE(L"task 3\n");
        return 3;
    });

    TRACE(L"result %d\n", t.get());

    t = create_task([]
    {
        TRACE(L"task 4a\n");
        return 4;
    });

    t = t.then([](int result)
    {
        TRACE(L"task 4b\n");
        return result * 10;
    });

    TRACE(L"result %d\n", t.get());

    auto a = create_task([] { TRACE(L"task A\n"); });
    auto b = create_task([] { TRACE(L"task B\n"); });

    auto c = (a && b).then([] { TRACE(L"task C\n"); });

    c.wait();
}
