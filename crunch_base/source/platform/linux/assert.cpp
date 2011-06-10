#include "crunch/base/assert.hpp"

#include <iostream>

namespace Crunch { namespace Detail {

bool DefaultAssertHandler(char const* condition, char const* file, int line, char const* message)
{
    std::cerr << "Assertion:" << std::endl;
    std::cerr << "\tLocation: " << file << "[" << line << "]" << std::endl;
    std::cerr << "\tExpression: " << condition << std::endl;
    std::cerr << "\tMessage: " << message << std::endl;
    return true;
}

}}
