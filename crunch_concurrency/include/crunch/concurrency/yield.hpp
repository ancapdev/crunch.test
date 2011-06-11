#ifndef CRUNCH_CONCURRENCY_YIELD_HPP
#define CRUNCH_CONCURRENCY_YIELD_HPP

#include "crunch/base/platform.hpp"

#include <boost/thread/thread.hpp>


#if defined (CRUNCH_ARCH_X86)
#   if defined (CRUNCH_COMPILER_MSVC) && defined (CRUNCH_ARCH_X86)
        extern "C" void _mm_pause();
#       pragma intrinsic(_mm_pause)
#       define CRUNCH_PAUSE() _mm_pause()
#   elif defined (CRUNCH_COMPILER_GCC)
#       define CRUNCH_PAUSE() __asm__ __volatile__("pause;");
#   endif
#endif

namespace Crunch { namespace Concurrency {

#if defined (CRUNCH_ARCH_X86)
inline void Pause(int count)
{
    for (int i = 0; i < count; ++i)
        CRUNCH_PAUSE();
}
#endif


class ExponentialBackoff
{
public:
    static int const DefaultYieldLimit = 16;

    ExponentialBackoff(int yieldLimit = DefaultYieldLimit)
        : mCount(1)
        , mLimit(yieldLimit)
    {}

    void Pause()
    {
        if (mCount <= mLimit)
        {
            Concurrency::Pause(mCount);
            mCount *= 2;
        }
        else
        {
            boost::this_thread::yield();
        }
    }

    bool TryPause()
    {
        if (mCount <= mLimit)
        {
            Concurrency::Pause(mCount);
            mCount *= 2;
            return true;
        }
        else
        {
            return false;
        }
    }
    
    void Reset()
    {
        mCount = 1;
    }

private:
    int mCount;
    int const mLimit;
};

}}

#endif
