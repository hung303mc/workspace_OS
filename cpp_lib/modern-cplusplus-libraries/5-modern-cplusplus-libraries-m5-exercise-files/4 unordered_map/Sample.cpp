#include "Precompiled.h"
#include <unordered_map>

using namespace std;

struct Hen
{
    int pasture;
    int id;

    Hen(int pasture_, int id_) :
        pasture { pasture_ },
        id { id_ }
    {
    }
};

auto operator==(Hen const & left, Hen const & right) -> bool
{
    return left.pasture == right.pasture &&
           left.id == right.id;
}

namespace std
{
    template <>
    struct hash<Hen>
    {
        auto operator()(Hen const & hen) const -> size_t
        {
            return hash<int> {} (hen.pasture) ^
                   hash<int> {} (hen.id);
        }
    };
}

auto main() -> int
{
    auto c = unordered_map<Hen, double>
    {
        { { 4, 123 }, 6.2 },
        { { 4, 124 }, 5.9 },
    };

    c[{5, 12}] = 6.1;

    auto k = Hen { 4, 123 };
    c[k] = 5.9;

    for (auto & v : c)
    {
        TRACE(L"pasture %d id %d > eggs %f\n",
              v.first.pasture,
              v.first.id,
              v.second);
    }
}
