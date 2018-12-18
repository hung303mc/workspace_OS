#pragma once

#include "debug.h"
#include "handle.h"
#include <memory>

namespace KennyKerr
{
    namespace Details
    {
        inline auto pack(HANDLE *) -> void {}

        template <typename T, typename... Args>
        auto pack(HANDLE * left,
                  T const & right,
                  Args const & ... args) -> void
        {
            *left = right.get();
            pack(++left, args...);
        }
    }

    struct windows_exception
    {
        DWORD error;

        explicit windows_exception(DWORD const value = GetLastError()) :
            error { value }
        {}
    };

    inline auto wait_one(HANDLE const h,
                         DWORD const milliseconds = INFINITE) -> bool
    {
        auto const result = WaitForSingleObject(h, milliseconds);

        if (result == WAIT_OBJECT_0) return true;

        if (result == WAIT_TIMEOUT) return false;

        throw windows_exception();
    }

    template <typename... Args>
    auto wait_all(Args const & ... args) -> void
    {
        HANDLE handles[sizeof...(Args)];
        Details::pack(handles, args...);

        VERIFY_(WAIT_OBJECT_0, WaitForMultipleObjects(sizeof...(Args),
                                                      handles,
                                                      true,
                                                      INFINITE));
    }

    enum class event_type
    {
        auto_reset,
        manual_reset
    };

    class event
    {
        null_handle h;

    public:

        event(event const &) = delete;
        auto operator=(event const &) -> event & = delete;
        ~event() = default;

        explicit event(event_type type) :
            h { CreateEvent(nullptr, static_cast<BOOL>(type), false, nullptr) }
        {
            if (!h) throw windows_exception();
        }

        event(event && other) throw() :
            h { other.h.release() }
        {}

        auto operator=(event && other) throw() -> event &
        {
            h = std::move(other.h);
        }

        auto set() throw() -> void
        {
            VERIFY(SetEvent(h.get()));
        }

        auto clear() throw() -> void
        {
            VERIFY(ResetEvent(h.get()));
        }

        auto wait(DWORD const milliseconds = INFINITE) throw() -> bool
        {
            auto const result = WaitForSingleObject(h.get(), milliseconds);

            ASSERT(result == WAIT_OBJECT_0 || result == WAIT_TIMEOUT);

            return result == WAIT_OBJECT_0;
        }

        auto get() const throw() -> HANDLE
        {
            return h.get();
        }
    };
}
