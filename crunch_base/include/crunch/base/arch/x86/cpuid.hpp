#ifndef CRUNCH_BASE_ARCH_X86_CPUID_HPP
#define CRUNCH_BASE_ARCH_X86_CPUID_HPP

#include "crunch/base/platform.hpp"
#include "crunch/base/stdint.hpp"

#if !defined (CRUNCH_ARCH_X86)
#   error "Unsupported archicture"
#endif

#if defined (CRUNCH_COMPILER_MSVC)
#   include <intrin.h>
#endif

namespace Crunch {

struct CpuidResult
{
    uint32 eax;
    uint32 ebx;
    uint32 ecx;
    uint32 edx;
};

enum class CpuidFunction : uint32
{
    VendorId = 0,
    ProcessorInfoAndFeatures = 1,
    CacheAndTlb = 2,
    SerialNumber = 3,
    HighestExtendedFunction = 0x80000000,
    ExtendedProcessorInfoAndFeatures = 0x80000001,
    ProcessorBrandString1 = 0x80000002,
    ProcessorBrandString2 = 0x80000003,
    ProcessorBrandString3 = 0x80000004,
    LevelOneCacheAndTlbIdentifiers = 0x80000005,
    ExtendedLevelTwoCacheFeaturs = 0x80000006,
    AdvancedPowerManagementInfo = 0x80000007,
    AddressSizes = 0x80000008
};

#if defined (CRUNCH_COMPILER_MSVC)

inline CpuidResult QueryCpuid(uint32 function, uint32 extendedFunction = 0)
{
    int result[4];
    __cpuidex(result, static_cast<int>(function), static_cast<int>(extendedFunction));
    return CpuidResult{
        static_cast<uint32>(result[0]),
        static_cast<uint32>(result[1]),
        static_cast<uint32>(result[2]),
        static_cast<uint32>(result[3])};
}

#elif defined (CRUNCH_COMPILER_GCC)

inline CpuidResult QueryCpuid(uint32 function, uint32 extendedFunction = 0)
{
    CpuidResult result;
    asm volatile(
        "cpuid"
        : "=a"(result.eax), "=b"(result.ebx), "=c"(result.ecx), "=d"(result.edx)
        : "a"(function), "c"(extendedFunction));
    return result;
}

#else
#   error "Unsupported compiler"
#endif

}

#endif
