#include "../../../include/ThreadPool.h"
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

namespace wdcpp
{
ThreadPool::ThreadPool(size_t workerNum, size_t capacity)
    : _workerNum(workerNum),
      _capacity(capacity),
      _taskQueue(capacity),
      _isExiting(false)
{
    _workers.reserve(_workerNum); // 申请 _workerNum * sizeof(unique_ptr<Thread>) 大小的空间
}

ThreadPool::~ThreadPool()
{
    stop();
}

void ThreadPool::addTask(Task &&task)
{
    if (task)
        _taskQueue.push(std::move(task));
}

ThreadPool::Task ThreadPool::getTask()
{
    return _taskQueue.pop(); // 可能阻塞
}

void ThreadPool::doTask()
{
    while (!_isExiting)
    {
        Task task = getTask(); // 取出任务（可能阻塞）
        if (task)
        {
            // cout << "worker thread " << pthread_self() << ": getTask" << endl;
            task(); // 执行任务（真正的执行任务！！！）
            // cout << "worker thread " << pthread_self() << ": finish task" << endl;
        }
    }
}

void ThreadPool::start()
{
    for (size_t idx = 0; idx < _workerNum; ++idx) // 创建子线程对象
    {
        unique_ptr<Thread> pThread(new Thread(idx, std::bind(&ThreadPool::doTask, this))); // 将 doTask 传给 Thread::_cb，并设置线程的 id
        _workers.push_back(std::move(pThread));
    }

    for (auto &worker : _workers) // 开启子线程
        worker->create();
}

void ThreadPool::stop()
{
    if (!_isExiting)
    {
        while (!_taskQueue.empty())
        {
            // 等待所有任务被取出
        }
        _isExiting = true;            // 设置退出标记为 true（当还在执行任务的线程完成任务后，会自动退出）
        _taskQueue.wakeupEmpty();     // 唤醒所有处于睡眠状态的子线程（当线程一旦被唤醒，会自动退出）
        for (auto &worker : _workers) // 回收所有子线程
        {
            worker->join();
        }
    }
}
};
