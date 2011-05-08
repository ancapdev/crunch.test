#include "crunch/base/assert.hpp"

#include <cstdarg>
#include <cstdio>

namespace Crunch {

namespace
{
    bool DefaultAssertHandler(char const* condition, char const* file, int line, char const* message)
    {
        return true;
    }

    AssertHandler gAssertHandler(&DefaultAssertHandler);
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

    int count = vsprintf_s(buffer, format, args);

    return GetAssertHandler()(condition, file, line, count < 0 ? "<assert format error>" : buffer);
}


}