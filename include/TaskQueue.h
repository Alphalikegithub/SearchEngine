#pragma once
#include "Condition.h"
#include "MutexLock.h"
#include <queue>
#include <functional>

using std::queue;

namespace wdcpp
{
class TaskQueue
{
private:
    using Task = std::function<void()>;

public:
    TaskQueue(size_t);

    bool full() const;
    bool empty() const;
    void push(Task &&); //包工头调用
    Task pop();         //工人调用
    void wakeupEmpty(); //唤醒所有工人

private:
    queue<Task> _queue;
    size_t _capacity;
    MutexLock _mutex;
    Condition _full;  //阻塞包工头（主线程），这里只有一个线程会产生任务
    Condition _empty; //阻塞工人（子线程），对应一个工人睡眠队列
    bool _isExiting; //退出标记（当线程被唤醒后，哪怕队列中没有任务也无需等待,直接退出）
};
}
