#pragma once

#include <crtdbg.h>
#include <windows.h>

#define ASSERT _ASSERTE

#ifdef _DEBUG
#define VERIFY ASSERT
#else
#define VERIFY(expression) (expression)
#endif
