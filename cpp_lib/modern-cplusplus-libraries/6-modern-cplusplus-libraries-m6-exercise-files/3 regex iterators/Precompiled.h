#pragma once

#include <stdio.h>
#include <crtdbg.h>
#include <windows.h>
#include <memory>
#include <utility>
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <string>
#include <regex>

#define ASSERT _ASSERTE

#ifdef _DEBUG
#define VERIFY ASSERT
#else
#define VERIFY(expression) (expression)
#endif

#ifdef _DEBUG

inline auto Trace(wchar_t const * format, ...) -> void
{
    va_list args;
    va_start(args, format);

    wchar_t buffer [256];

    ASSERT(-1 != _vsnwprintf_s(buffer,
                               _countof(buffer) -1,
                               format,
                               args));

    va_end(args);

    OutputDebugString(buffer);
}

struct Tracer
{
    char const * m_filename;
    unsigned m_line;

    Tracer(char const * filename, unsigned const line) :
        m_filename { filename },
        m_line { line }
    {

    }

    template <typename... Args>
    auto operator()(wchar_t const * format, Args... args) const -> void
    {
        wchar_t buffer [256];

        auto count = swprintf_s(buffer,
                                L"%S(%d): ",
                                m_filename,
                                m_line);

        ASSERT(-1 != count);

        ASSERT(-1 != _snwprintf_s(buffer + count,
                                  _countof(buffer) - count,
                                  _countof(buffer) - count - 1,
                                  format,
                                  args...));

        OutputDebugString(buffer);
    }
};

#endif

#ifdef _DEBUG
#define TRACE Tracer(__FILE__, __LINE__)
#else
#define TRACE __noop
#endif

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

        auto swap(unique_handle<Traits> & other) throw() -> void
        {
            std::swap(m_value, other.m_value);
        }
    };

    template <typename Traits>
    auto swap(unique_handle<Traits> & left,
              unique_handle<Traits> & right) throw() -> void
    {
        left.swap(right);
    }

    template <typename Traits>
    auto operator==(unique_handle<Traits> const & left,
                    unique_handle<Traits> const & right) throw() -> bool
    {
        return left.get() == right.get();
    }

    template <typename Traits>
    auto operator!=(unique_handle<Traits> const & left,
                    unique_handle<Traits> const & right) throw() -> bool
    {
        return left.get() != right.get();
    }

    template <typename Traits>
    auto operator<(unique_handle<Traits> const & left,
                   unique_handle<Traits> const & right) throw() -> bool
    {
        return left.get() < right.get();
    }

    template <typename Traits>
    auto operator>=(unique_handle<Traits> const & left,
                    unique_handle<Traits> const & right) throw() -> bool
    {
        return left.get() >= right.get();
    }

    template <typename Traits>
    auto operator>(unique_handle<Traits> const & left,
                   unique_handle<Traits> const & right) throw() -> bool
    {
        return left.get() > right.get();
    }

    template <typename Traits>
    auto operator<=(unique_handle<Traits> const & left,
                    unique_handle<Traits> const & right) throw() -> bool
    {
        return left.get() <= right.get();
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
