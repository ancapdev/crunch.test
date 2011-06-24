#include "crunch/base/arch/x86/cpuid.hpp"

#if defined (CRUNCH_COMPILER_MSVC)
#   include <intrin.h>
#endif

namespace Crunch {

CpuidResult QueryCpuid(uint32 function, uint32 extendedFunction)
{
#if defined (CRUNCH_COMPILER_MSVC)
    int result[4];
    __cpuidex(result, static_cast<int>(function), static_cast<int>(extendedFunction));
    return CpuidResult{
        static_cast<uint32>(result[0]),
        static_cast<uint32>(result[1]),
        static_cast<uint32>(result[2]),
        static_cast<uint32>(result[3])};
#elif defined (CRUNCH_COMPILER_GCC)
    CpuidResult result;
    asm volatile(
        "cpuid"
        : "=a"(result.eax), "=b"(result.ebx), "=c"(result.ecx), "=d"(result.edx)
        : "a"(function), "c"(extendedFunction));
    return result;
#else
#   error "Unsupported compiler"
#endif
}

std::string GetCpuidVendorId()
{
    CpuidResult vendor = QueryCpuid(CpuidFunction::VendorId);

    std::string result;

    for (int i = 0; i < 4; ++i)
        result.push_back(static_cast<char>((vendor.ebx >> (i * 8)) & 0xff));

    for (int i = 0; i < 4; ++i)
        result.push_back(static_cast<char>((vendor.edx >> (i * 8)) & 0xff));

    for (int i = 0; i < 4; ++i)
        result.push_back(static_cast<char>((vendor.ecx >> (i * 8)) & 0xff));

    return result;
}

uint32 GetCpuidMaxFunction()
{
    return QueryCpuid(CpuidFunction::VendorId).eax;
}

}
