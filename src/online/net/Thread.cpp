#include "../../../include/Thread.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
using std::cin;
using std::cout;
using std::endl;

namespace wdcpp
{
__thread size_t __thread_id; // 工作线程的编号（0, 1, 2, ... , _workerNum-1）

Thread::Thread(ThreadCallBack &&cb)
    : _thid(0),
      _isRunning(false),
      _cb(std::move(cb))
{
}

Thread::Thread(size_t id, ThreadCallBack &&cb)
    : _id(id),
      _thid(0),
      _isRunning(false),
      _cb(std::move(cb))
{
}

Thread::~Thread()
{
    // cout << "~Thread()" << endl;
    if (_isRunning)
    {
        pthread_detach(_thid); // 交给系统回收_thid线程的资源，而不交给主线程回收
        // join(); // 保证主线程后于子线程终止（即交给主线程回收子线程的资源）
    }
}

void Thread::create()
{
    int ret = pthread_create(&_thid, nullptr, threadFunc, (void *)this); // this指针一定是Thread类型
    if (ret)
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    _isRunning = true;
}

void Thread::join()
{
    if (_isRunning)
    {
        pthread_join(_thid, nullptr);
        _isRunning = false;
    }
}
void *Thread::threadFunc(void *args)
{
    Thread *pThread = (Thread *)args;
    __thread_id = pThread->_id; // 设置该线程是几号线程
    if (pThread)
    {
        pThread->_cb(); // doTask -> getTask -> task
    }
    cout << "worker thread " << pthread_self() << ": exit" << endl;
    pthread_exit(nullptr);
}
};
