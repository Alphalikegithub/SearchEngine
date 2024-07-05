#pragma once
#include <pthread.h>
#include "NonCopyable.h"

namespace wdcpp
{
class MutexLock
:NonCopyable
{
public:
    MutexLock();
    ~MutexLock();

    void lock();
    void unlock();
    pthread_mutex_t *getLock();

private:
    pthread_mutex_t _mutex;
};
}
