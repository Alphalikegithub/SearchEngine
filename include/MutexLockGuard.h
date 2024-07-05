#pragma once
#include "MutexLock.h"

namespace wdcpp
{
class MutexLockGuard
{
public:
    MutexLockGuard(MutexLock &);
    ~MutexLockGuard();

private:
    MutexLock &_mutex;
};
}
