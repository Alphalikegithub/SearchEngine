#pragma once
#include "MutexLock.h"

namespace wdcpp
{
class Condition
:NonCopyable
{
public:
    Condition(MutexLock &);
    ~Condition();

    void wait();
    void notify();
    void notifyAll();

private:
    MutexLock &_mutex;
    pthread_cond_t _cond;
};
}
