#include "crunch/concurrency/detail/system_mutex.hpp"

namespace Crunch { namespace Concurrency { namespace Detail {

SystemMutex::SystemMutex()
{
    InitializeCriticalSection(&mCriticalSection);
}

SystemMutex::~SystemMutex()
{
    DeleteCriticalSection(&mCriticalSection);
}

bool SystemMutex::TryLock()
{
    return TryEnterCriticalSection(&mCriticalSection) == TRUE;
}

void SystemMutex::Lock()
{
    EnterCriticalSection(&mCriticalSection);
}

void SystemMutex::Unlock()
{
    LeaveCriticalSection(&mCriticalSection);
}

}}}
