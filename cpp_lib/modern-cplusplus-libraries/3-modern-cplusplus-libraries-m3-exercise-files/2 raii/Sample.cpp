#include "Precompiled.h"

namespace KennyKerr
{
    template <typename Traits>
    class unique_handle
    {
        typedef typename Traits::pointer pointer;

        pointer m_value;

        auto close() throw() -> void
        {
            if (*this)
            {
                Traits::close(m_value);
            }
        }

    public:

        explicit unique_handle(pointer value = Traits::invalid()) throw() :
            m_value { value }
        {
        }

        ~unique_handle() throw()
        {
            close();
        }

        explicit operator bool() const throw()
        {
            return m_value != Traits::invalid();
        }

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
    auto raw = HANDLE { nullptr };
    auto h = null_handle { raw };

    if (h)
    {
    }
}
