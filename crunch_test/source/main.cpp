// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/base/platform.hpp"

#if defined (CRUNCH_COMPILER_MSVC)
// warning C6320 Exception-filter expression is the constant EXCEPTION_EXECUTE_HANDLER. This might mask exceptions that were not intended to be handled
#   pragma warning (disable : 6320)
// warning C6322: Empty _except block
#   pragma warning (disable : 6322)
// warning C6335: Leaking process information handle 'debugger_info.hProcess'
#   pragma warning (disable : 6335)
// warning C6054: String 'format' might not be zero-terminated
#   pragma warning (disable : 6054)
// warning C6001: Using uninitialized memory
#   pragma warning (disable : 6001)
// warning C6269: Possibly incorrect order of operations: dereference ignored
#   pragma warning (disable : 6269)
// warning C6011: Dereferencing NULL pointer
#   pragma warning (disable : 6011)
// warning C6387: 'argument 1' might be '0': this does not adhere to the specification for the function ''
#   pragma warning (disable : 6387)
// warning C6031: Return value ignored
#   pragma warning (disable : 6031)
// warning C6246: Local declaration of 'c' hides declaration of the same name in outer scope
#   pragma warning (disable : 6246)
// warning C6328: 'char' passed as parameter '1' when 'unsigned char' is required in call to 'ispunct'
#   pragma warning (disable : 6328)
#endif

#include <boost/test/included/unit_test.hpp>

bool init_unit_test()
{
    return true;
}
