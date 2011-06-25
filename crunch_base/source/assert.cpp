// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/base/assert.hpp"
#include "crunch/base/platform.hpp"

#include <cstdarg>
#include <cstdio>

namespace Crunch {

namespace Detail
{
    // defined in platform/<name>/assert.cpp
    bool DefaultAssertHandler(char const* condition, char const* file, int line, char const* message);
}

namespace
{
    AssertHandler gAssertHandler(&Detail::DefaultAssertHandler);
}

void SetAssertHandler(AssertHandler const& handler)
{
    gAssertHandler = handler;
}

AssertHandler const& GetAssertHandler()
{
    return gAssertHandler;
}

bool HandleAssert(char const* condition, char const* file, int line)
{
    return GetAssertHandler()(condition, file, line, nullptr);
}

bool HandleAssert(char const* condition, char const* file, int line, char const* format, ...)
{
    char buffer[4096];

    va_list args;
    va_start(args, format);

#if defined (CRUNCH_PLATFORM_WIN32)
    int count = vsprintf_s(buffer, format, args);
#else
    int count = vsprintf(buffer, format, args);
#endif

    return GetAssertHandler()(condition, file, line, count < 0 ? "<assert format error>" : buffer);
}


}
