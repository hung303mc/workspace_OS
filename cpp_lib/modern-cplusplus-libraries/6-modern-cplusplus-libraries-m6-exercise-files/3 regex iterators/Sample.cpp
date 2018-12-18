#include "Precompiled.h"

using namespace std;
using namespace KennyKerr;

#pragma comment(lib, "urlmon.lib")

auto main() -> int
{
    wchar_t filename[MAX_PATH];

    auto hr = URLDownloadToCacheFile(nullptr,
                                     L"http://live.sysinternals.com/",
                                     filename,
                                     _countof(filename),
                                     0,
                                     nullptr);

    if (S_OK != hr)
    {
        return 0;
    }

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
        return 0;
    }

    auto size = LARGE_INTEGER {};

    if (!GetFileSizeEx(file.get(), &size))
    {
        return 0;
    }

    if (size.QuadPart == 0)
    {
        return 0;
    }

    auto map = null_handle
    {
        CreateFileMapping(file.get(),
                          nullptr,
                          PAGE_READONLY,
                          0, 0, 
                          nullptr)
    };

    if (!map)
    {
        return 0;
    }

    auto view = unique_ptr<char, map_view_deleter>
    {
        static_cast<char *>(MapViewOfFile(map.get(),
                                          FILE_MAP_READ,
                                          0, 0,
                                          0))
    };

    if (!view)
    {
        return 0;
    }

    auto r = regex { R"*(<A HREF="/(\w+\.\w{3})">)*" };

    auto begin = view.get();
    auto end = begin + size.QuadPart;

    for (auto i = cregex_iterator { begin, end, r };
         i != cregex_iterator {};
         ++i)
    {
        auto & match = (*i)[1];

        TRACE(L"%.*S\n", match.length(), match.first);
    }
}
