#pragma once
#include <memory>
#include <vector>
#include "Thread.h"
#include "TaskQueue.h"

using std::unique_ptr;
using std::vector;

namespace wdcpp
{
class ThreadPool
{
    friend class WorkerThread;

private:
    using Task = std::function<void()>;

public:
    ThreadPool(size_t, size_t);
    ~ThreadPool();

public:
    void start();            //开启线程池（创建子线程对象，开启线程）
    void stop();             //关闭线程池（等待队列为空 -> 退出标记置为true -> 唤醒所有工人 -> join）
    void addTask(Task &&);   //往队列中添加具体的任务（Task 提供了每个线程 run 时所需的数据） 

private:
    void doTask();      //工人线程 run 中调用（run -> doTask -> getTask -> process） 
    Task getTask();    //获取任务（从队列中取出任务）

private:
    vector<unique_ptr<Thread>> _workers;
    size_t _workerNum; //工人数
    size_t _capacity; //队列容量（任务数的最大值）
    TaskQueue _taskQueue;
    bool _isExiting;  //退出标记
};
}
