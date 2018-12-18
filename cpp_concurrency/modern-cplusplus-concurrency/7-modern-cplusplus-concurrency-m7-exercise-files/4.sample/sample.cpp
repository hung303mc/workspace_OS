#include "precompiled.h"

using namespace std;
using namespace concurrency;

template <typename C>
auto sum(C const & c) -> typename C::value_type
{
    return parallel_reduce(begin(c), end(c), 0);
}

template <typename C>
auto product(C const & c) -> typename C::value_type
{
    return parallel_reduce(begin(c), end(c), 1, [](int left, int right)
    {
        return left * right;
    });
}

auto main() -> int
{
    auto v = vector<int> { 1, 2, 3, 4, 5 };
    auto s = int {};
    auto p = int {};

    parallel_invoke
    (
        [&] { s = sum(v); },
        [&] { p = product(v); }
    );

    printf("sum: %d\nproduct: %d\n", s, p);
}
