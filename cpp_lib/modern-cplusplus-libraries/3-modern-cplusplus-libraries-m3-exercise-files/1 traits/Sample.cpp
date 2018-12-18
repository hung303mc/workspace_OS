#include "Precompiled.h"

namespace KennyKerr
{
    template <typename Traits>
    class unique_handle
    {
    };

    struct null_handle_traits
    {
        typedef HANDLE pointer;

        static auto invalid() throw() -> pointer
        {
            return nullptr;
        }

        static auto close(pointer value) throw() -> void
        {
            VERIFY(CloseHandle(value));
        }
    };

    typedef unique_handle<null_handle_traits> null_handle;
}

using namespace std;
using namespace KennyKerr;

auto main() -> int
{
    auto h = null_handle {};
}
