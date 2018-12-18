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

        unique_handle(unique_handle const &);
        auto operator=(unique_handle const &) -> unique_handle &;

    public:

        explicit unique_handle(pointer value = Traits::invalid()) throw() :
            m_value { value }
        {
        }

        unique_handle(unique_handle && other) throw() :
            m_value { other.release() }
        {
            TRACE(L"move ctor\n");
        }

        auto operator=(unique_handle && other) throw() -> unique_handle &
        {
            TRACE(L"move assignment\n");

            if (this != &other)
            {
                reset(other.release());
            }

            return *this;
        }

        ~unique_handle() throw()
        {
            TRACE(L"destructor\n");

            close();
        }

        explicit operator bool() const throw()
        {
            return m_value != Traits::invalid();
        }

        auto get() const throw() -> pointer
        {
            return m_value;
        }

        auto release() throw() -> pointer
        {
            auto value = m_value;
            m_value = Traits::invalid();
            return value;
        }

        auto reset(pointer value = Traits::invalid()) throw() -> bool
        {
            if (m_value != value)
            {
                close();
                m_value = value;
            }

            return static_cast<bool>(*this);
        }

        auto swap(unique_handle<Traits> & other) throw() -> void
        {
            std::swap(m_value, other.m_value);
        }
    };

    template <typename Traits>
    auto swap(unique_handle<Traits> & left,
              unique_handle<Traits> & right) throw() -> void
    {
        TRACE(L"swap\n");

        left.swap(right);
    }

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

    struct invalid_handle_traits
    {
        typedef HANDLE pointer;

        static auto invalid() throw() -> pointer
        {
            return INVALID_HANDLE_VALUE;
        }

        static auto close(pointer value) throw() -> void
        {
            VERIFY(CloseHandle(value));
        }
    };

    typedef unique_handle<null_handle_traits> null_handle;
    typedef unique_handle<invalid_handle_traits> invalid_handle;
}

struct map_view_deleter
{
    typedef char const * pointer;

    auto operator()(pointer value) const throw() -> void
    {
        VERIFY(UnmapViewOfFile(value));
    }
};

#include <algorithm>

using namespace std;
using namespace KennyKerr;

auto main() -> int
{
    null_handle handles [10];

    reverse(begin(handles), end(handles));
}
