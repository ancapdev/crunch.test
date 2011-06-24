#ifndef CRUNCH_BASE_ARCH_X86_CPUID_HPP
#define CRUNCH_BASE_ARCH_X86_CPUID_HPP

#include "crunch/base/enum_class.hpp"
#include "crunch/base/platform.hpp"
#include "crunch/base/stdint.hpp"

#if !defined (CRUNCH_ARCH_X86)
#   error "Unsupported archicture"
#endif

#include <string>

namespace Crunch {

struct CpuidResult
{
    uint32 eax;
    uint32 ebx;
    uint32 ecx;
    uint32 edx;
};

CRUNCH_ENUM_CLASS CpuidFunction : uint32
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

CpuidResult QueryCpuid(uint32 function, uint32 extendedFunction = 0);

inline CpuidResult QueryCpuid(CpuidFunction function, uint32 extendedFunction = 0)
{
    return QueryCpuid(static_cast<uint32>(function), extendedFunction);
}

std::string GetCpuidVendorId();
uint32 GetCpuidMaxFunction();

}

#endif
