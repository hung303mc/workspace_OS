#include "precompiled.h"

using namespace std;
using namespace KennyKerr;

auto pack(HANDLE *) -> void {}

template <typename T, typename... Args>
auto pack(HANDLE * left,
          T const & right,
          Args const & ... args) -> void
{
    *left = right.get();
    pack(++left, args...);
}

template <typename... Args>
auto wait_all(Args const & ... args) -> void
{
    HANDLE handles[sizeof...(Args)];
    pack(handles, args...);

    VERIFY_(WAIT_OBJECT_0, WaitForMultipleObjects(sizeof...(Args),
                                                  handles,
                                                  true,
                                                  INFINITE));
}

static auto e = event { event_type::auto_reset };

auto __stdcall work(void *) -> DWORD
{
    TRACE(L"thread %d\n", GetCurrentThreadId());

    e.wait();

    TRACE(L"run %d\n", GetCurrentThreadId());

    Sleep(1000);

    e.set();

    return 0;
}

auto main() -> int
{
    auto t1 = null_handle
    {
        CreateThread(nullptr, 0, work, nullptr, 0, nullptr)
    };

    auto t2 = null_handle
    {
        CreateThread(nullptr, 0, work, nullptr, 0, nullptr)
    };

    Sleep(100);
    e.set();
    Sleep(100);

    wait_all(t1, t2);
}
