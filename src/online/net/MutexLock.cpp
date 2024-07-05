#include "../../../include/MutexLock.h"

namespace wdcpp
{
MutexLock::MutexLock()
{
    pthread_mutex_init(&_mutex, nullptr);
}

MutexLock::~MutexLock()
{
    pthread_mutex_destroy(&_mutex);
}

void MutexLock::lock()
{
    pthread_mutex_lock(&_mutex);
}

void MutexLock::unlock()
{
    pthread_mutex_unlock(&_mutex);
}

pthread_mutex_t *MutexLock::getLock()
{
    return &_mutex;
}
};
