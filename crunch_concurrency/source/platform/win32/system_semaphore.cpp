// Copyright (c) 2011, Christian Rorvik
// Distributed under the Simplified BSD License (See accompanying file LICENSE.txt)

#include "crunch/concurrency/detail/system_semaphore.hpp"
#include "crunch/base/assert.hpp"

#include <limits>

#include <windows.h>

namespace Crunch { namespace Concurrency { namespace Detail {

SystemSemaphore::SystemSemaphore(uint32 initialCount)
    : mSemaphore(::CreateSemaphore(NULL, static_cast<LONG>(initialCount), std::numeric_limits<LONG>::max(), NULL))
{}

SystemSemaphore::~SystemSemaphore()
{
    ::CloseHandle(mSemaphore);
}

void SystemSemaphore::Post()
{
    CRUNCH_ASSERT_ALWAYS(::ReleaseSemaphore(mSemaphore, 1, NULL) == TRUE);
}
void SystemSemaphore::Wait()
{
    CRUNCH_ASSERT_ALWAYS(::WaitForSingleObject(mSemaphore, INFINITE) == WAIT_OBJECT_0);
}

}}}
