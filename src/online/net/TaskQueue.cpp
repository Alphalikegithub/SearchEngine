#include "../../../include/TaskQueue.h"
#include "../../../include/MutexLockGuard.h"
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

namespace wdcpp
{
TaskQueue::TaskQueue(size_t capacity)
    : _capacity(capacity),
      _mutex(), // 调用MutexLock的构造函数，保证_mutex先于_full和_empty构造
      _full(_mutex),
      _empty(_mutex),
      _isExiting(false)
{
}

bool TaskQueue::full() const
{
    return _queue.size() == _capacity;
}

bool TaskQueue::empty() const
{
    return _queue.empty();
}

void TaskQueue::push(Task &&task)
{
    MutexLockGuard autolock(_mutex); // 保证在本函数退出前一定完成解锁，即防止了死锁的发生
    while (full())                   // 仓库满，阻塞生产者
    {
        _full.wait();
    }

    _queue.push(std::move(task)); // 任务入队

    if (!empty()) // 仓库非空，唤醒消费者
    {
        _empty.notifyAll();
    }
}

TaskQueue::Task TaskQueue::pop()
{
    MutexLockGuard autolock(_mutex); // 保证在本函数退出前一定完成解锁，即防止了死锁的发生
    while (!_isExiting && empty())   // 仓库空，阻塞消费者
    {
        _empty.wait();
    }

    if (_isExiting)
        return nullptr;

    Task tmp = _queue.front();
    _queue.pop(); // 任务出队

    if (!full()) // 仓库非满，唤醒生产者
    {
        _full.notifyAll();
    }

    return tmp;
}

void TaskQueue::wakeupEmpty()
{
    _isExiting = true;
    _empty.notifyAll();
}
};
