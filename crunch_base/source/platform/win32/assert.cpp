#include "crunch/base/assert.hpp"

#include <cstdio>

#include <Windows.h>

namespace Crunch { namespace Detail {

bool DefaultAssertHandler(char const* condition, char const* file, int line, char const* message)
{
    char buffer[4096];

    sprintf_s(
        buffer,
        "Location:\t\t%s [%d]\n"
        "Expression:\t%s\n"
        "Message:\t\t%s\n\n\n"
        "Press Abort to quit, Retry to debug, or Ignore to continue",
        file,
        line,
        condition ? condition : "<No condition>",
        message ? message : "<No message>");

    int mbResult = MessageBoxA(
        NULL,
        buffer,
        "Assert",
        MB_ABORTRETRYIGNORE | MB_ICONERROR | MB_SETFOREGROUND);

    switch (mbResult)
    {
    case IDIGNORE:
        return false;

    case IDABORT:
        std::exit(-1);
    
    default:
        return true;
    }
}

}}