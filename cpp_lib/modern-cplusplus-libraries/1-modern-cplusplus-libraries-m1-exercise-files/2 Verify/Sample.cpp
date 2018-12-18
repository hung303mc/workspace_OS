#include "Precompiled.h"
#include <windows.h>

#define ASSERT _ASSERTE

#ifdef _DEBUG
#define VERIFY ASSERT
#else
#define VERIFY(expression) (expression)
#endif

struct LastException
{
    DWORD result;

    LastException() : result { GetLastError() } {}
};

struct ManualResetEvent
{
    HANDLE m_handle;

    ManualResetEvent()
    {
        m_handle = CreateEvent(nullptr,
                               true,
                               false,
                               nullptr);

        if (!m_handle)
        {
            throw LastException();
        }
    }

    ~ManualResetEvent()
    {
        VERIFY(CloseHandle(m_handle));
    }
};

auto main() -> int
{
    auto e = ManualResetEvent {};
    auto e2 = e;
}
