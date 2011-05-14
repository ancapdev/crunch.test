#ifndef CRUNCH_BASE_ASSERT_HPP
#define CRUNCH_BASE_ASSERT_HPP

#include "crunch/base/debug.hpp"

#include <boost/function.hpp>

namespace Crunch {

/// \param condition The expression which failed assertion. Can be null.
/// \param file The file the assert was triggered from.
/// \param line The line in the file the assert was triggered from.
/// \param message A message describing the assertion. Can be null.
typedef boost::function<bool (char const* condition, char const* file, int line, char const* message)> AssertHandler;

void SetAssertHandler(AssertHandler const& handler);
AssertHandler const& GetAssertHandler();

bool HandleAssert(char const* condition, char const* file, int line);
bool HandleAssert(char const* condition, char const* file, int line, char const* format, ...);

}

#if defined (CRUNCH_DEBUG)
#   define CRUNCH_ASSERT_ENABLED
#endif

#define CRUNCH_ASSERT_ALWAYS(cond) \
    do \
    { \
        if (!(cond) && ::Crunch::HandleAssert(#cond, __FILE__, __LINE__)) \
            CRUNCH_HALT(); \
    } while(0)

#define CRUNCH_ASSERT_MSG_ALWAYS(cond, msg, ...) \
    do \
    { \
        if (!(cond) && ::Crunch::HandleAssert(#cond, __FILE__, __LINE__, msg, __VA_ARGS__)) \
            CRUNCH_HALT(); \
    } while(0)

#define CRUNCH_ABORT_ALWAYS(msg, ...) \
    do \
    { \
        if (::Crunch::HandleAssert(nullptr, __FILE__, __LINE__, msg, __VA_ARGS__)) \
            CRUNCH_HALT(); \
    } while(0)

#if defined (CRUNCH_ASSERT_ENABLED)
#   define CRUNCH_ASSERT(cond) CRUNCH_ASSERT_ALWAYS(cond)
#   define CRUNCH_ASSERT_MSG(cond, msg, ...) CRUNCH_ASSERT_MSG_ALWAYS(cond, msg, __VA_ARGS__)
#   define CRUNCH_ABORT(msg, ...) CRUNCH_ABORT_ALWAYS(ms, __VA_ARGS__)
#else
#   define CRUNCH_ASSERT(cond) do { (void)sizeof(cond); } while(0)
#   define CRUNCH_ASSERT_MSG(cond, msg, ...) do { (void)sizeof(cond); (void)sizeof(msg); } while(0) 
#   define CRUNCH_ABORT(msg, ...) do { (void)sizeof(msg); } while(0)
#endif

#define CRUNCH_ASSERT_ALIGNMENT(ptr, alignment) CRUNCH_ASSERT((reinterpret_cast<std::size_t>(ptr) & ((alignment)-1)) == 0)

#endif