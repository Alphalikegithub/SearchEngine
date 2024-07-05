#include "../../../include/MutexLockGuard.h"

namespace wdcpp
{
MutexLockGuard::MutexLockGuard(MutexLock &mutex)
    : _mutex(mutex)
{
    _mutex.lock();
}

MutexLockGuard::~MutexLockGuard()
{
    _mutex.unlock();
}
};
