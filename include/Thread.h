#pragma once
#include <pthread.h>
#include "NonCopyable.h"
#include <functional>

namespace wdcpp
{
class Thread
:NonCopyable
{
private:
    using ThreadCallBack = std::function<void()>;

public:
    Thread(ThreadCallBack &&);
    Thread(size_t, ThreadCallBack &&);
    virtual ~Thread();

    void create(); // 开启子线程(pthread_create -> threadFunc)
    void join(); // 回收子线程(pthread_join)

private:
    static void *threadFunc(void *); // 线程的入口函数

private:
    size_t _id;
    pthread_t _thid;
    bool _isRunning;
    ThreadCallBack _cb; // 这里会传入ThreadPool::doTask
};
}
