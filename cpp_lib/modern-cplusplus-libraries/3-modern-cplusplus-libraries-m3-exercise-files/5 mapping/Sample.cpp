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
        }

        auto operator=(unique_handle && other) throw() -> unique_handle &
        {
            if (this != &other)
            {
                reset(other.release());
            }

            return *this;
        }

        ~unique_handle() throw()
        {
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

using namespace std;
using namespace KennyKerr;

struct map_view_deleter
{
    typedef char const * pointer;

    auto operator()(pointer value) const throw() -> void
    {
        VERIFY(UnmapViewOfFile(value));
    }
};

auto main() -> int
{
    auto filename = LR"(C:\Sample\Sample.cpp)";

    auto file = invalid_handle
    {
        CreateFile(filename,
                   GENERIC_READ,
                   FILE_SHARE_READ,
                   nullptr,
                   OPEN_EXISTING,
                   FILE_ATTRIBUTE_NORMAL,
                   nullptr)
    };

    if (!file)
    {
        printf("CreateFile failed %d\n", GetLastError());
        return 0;
    }

    auto size = LARGE_INTEGER {};

    if (!GetFileSizeEx(file.get(), &size))
    {
        printf("GetFileSizeEx failed %d\n", GetLastError());
        return 0;
    }

    if (size.QuadPart == 0)
    {
        printf("File is empty\n");
        return 0;
    }

    auto map = null_handle
    {
        CreateFileMapping(file.get(),
                          nullptr,
                          PAGE_READONLY,
                          0,0,
                          nullptr)
    };

    if (!map)
    {
        printf("CreateFileMapping failed %d\n", GetLastError());
        return 0;
    }

    file.reset();

    auto view = unique_ptr<char, map_view_deleter>
    {
        static_cast<char *>(MapViewOfFile(map.get(),
                                          FILE_MAP_READ,
                                          0,0,
                                          0))
    };

    if (!view)
    {
        printf("MapViewOfFile failed %d\n", GetLastError());
        return 0;
    }

    printf("%.*s\n", static_cast<unsigned>(size.QuadPart), view.get());
}
