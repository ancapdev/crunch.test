#include "crunch/concurrency/processor_topology.hpp"
#include "crunch/concurrency/processor_affinity.hpp"
#include "crunch/base/platform.hpp"
#include "crunch/base/bit_utility.hpp"

#if defined (CRUNCH_ARCH_X86)
#   include "crunch/base/arch/x86/cpuid.hpp"
#endif

#include <algorithm>

// TODO: remove
#include <iostream>


namespace Crunch { namespace Concurrency {

template<typename T>
struct BinaryStreamer
{
    BinaryStreamer(T value, int delimitPer = 4) : value(value), delimitPer(delimitPer) {}

    T value;
    int delimitPer;
};

template<typename T>
std::ostream& operator << (std::ostream& os, BinaryStreamer<T> value)
{
    for (int i = 0; i < (sizeof(T) * 8); ++i)
    {
        if (i != 0 && i % value.delimitPer == 0)
            os << " ";

        os  << ((value.value & (1 << i)) == 0 ? "0" : "1");
    }
    
    return os;
}

template<typename T>
BinaryStreamer<T> StreamAsBinary(T value, int delimitPer = 4)
{
    return BinaryStreamer<T>(value, delimitPer);
}

ProcessorTopology::ProcessorTopology()
{
#if defined (CRUNCH_ARCH_X86)
    // Reference: http://software.intel.com/en-us/articles/intel-64-architecture-processor-topology-enumeration/

    std::cout << "CPU vendor: " << GetCpuidVendorId() << std::endl;
    std::cout << "Max CPUID function: " << GetCpuidMaxFunction() << std::endl;

    if (GetCpuidMaxFunction() >= 11 &&
        QueryCpuid(0).ebx != 0)
    {
        // Enumerate using leaf 11
    }
    else if (GetCpuidMaxFunction() >= 4)
    {
        // Enumerate using leaf 1 and 4
        const ProcessorAffinity processAffinity = GetCurrentProcessAffinity();

        const uint32 highest = processAffinity.GetHighestSetProcessor();

        CpuidResult const cpuid1 = QueryCpuid(1);
        CpuidResult const cpuid4 = QueryCpuid(4);

        uint32 const allMask = ~uint32(0);
        uint32 const maxNumLogicalPerPackage = ExtractBits(cpuid1.ebx, 16, 23);
        uint32 const maxNumPhysicalPerPackage = ExtractBits(cpuid4.eax, 26, 31) + 1;
        uint32 const smtMaskWidth = Log2Floor(Pow2Ceil(maxNumLogicalPerPackage) / maxNumPhysicalPerPackage);
        uint32 const smtMask = ~(allMask << smtMaskWidth);
        uint32 const coreMaskWidth = Log2Floor(maxNumPhysicalPerPackage);
        uint32 const coreMask = (~(allMask << (coreMaskWidth + smtMaskWidth))) ^ smtMask;
        uint32 const packageMask = allMask << (coreMaskWidth + smtMaskWidth);
            
        std::cout << "Max num logical per package: " << maxNumLogicalPerPackage << std::endl;
        std::cout << "Max num physical per package: " << maxNumPhysicalPerPackage << std::endl;
        std::cout << "SMT mask width: " << smtMaskWidth << std::endl;
        std::cout << "SMT mask: " << StreamAsBinary(smtMask) << std::endl;
        std::cout << "Core mask width: " << coreMaskWidth << std::endl;
        std::cout << "Core mask: " << StreamAsBinary(coreMask) << std::endl;
        std::cout << "Package mask: " << StreamAsBinary(packageMask) << std::endl;


        for (uint32 i = 0; i <= highest; ++i)
        {
            if (!processAffinity.IsSet(i))
                continue;

            ProcessorAffinity old = SetCurrentThreadAffinity(ProcessorAffinity(i));

            uint32 const stuff = QueryCpuid(1).ebx;
            std::cout << "APIC[" << i << "]: " << ExtractBits(stuff, 24, 31) << std::endl;
            for (int x = 0; x < 32; ++x)
            {
                if ((x & 3) == 0)
                    std::cout << " ";
                std::cout << ((stuff & (1 << x)) == 0 ? "0" : "1");
            }
            std::cout << std::endl;

            SetCurrentThreadAffinity(old);
        }
    }
    else
    {
        // Can't enumerate. Fall back on system call
    }

#else
    uint32 numProcessors = GetSystemNumProcessors();
    for (uint32 i = 0; i < numProcessors; ++i)
    {
        mLogicalProcessors.push_back(LogicalProcessor{i,i,0});
    }

    mPackages.push_back(Package{0, std::vector<uint32>(), std::vector<uint32>()});

    std::for_each(mLogicalProcessors.begin(), mLogicalProcessors.end(),[&] (LogicalProcessor const& p) {
        mPhysicalProcessors.push_back(PhysicalProcessor{p.id, 0, std::vector<uint32>(1, p.id)});
        mPackages[0].physicalIds.push_back(p.id);
        mPackages[0].logicalIds.push_back(p.id);
    });
#endif
}


}}
