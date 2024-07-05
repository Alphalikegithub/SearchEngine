#include "../../include/Timer.h"
#include "../../include/ERROR_CHECK.h"
#include <sys/timerfd.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <poll.h>
#include <iostream>
using std::cout;
using std::endl;

namespace wdcpp
{
Timer::Timer(TimerCallBack &&_cb, int init, int periodic)
    : _timerCb(std::move(_cb)),
      //   _timerFd(createTimerFd()), // 若在这儿 create timerFd，它只属于主线程！
      _timerFd(),
      _initTime(init),
      _periodicTime(periodic),
      _isRunning(false)
{
}

/**
 *  1. 子线程调用
 *  2. 创建 _timerFd
 *  3. 设置定时器时间
 *  4. 开始监听
 */
void Timer::start()
{
    _timerFd = createTimerFd();           // 2
    setTimerFd(_initTime, _periodicTime); // 3

    struct pollfd pfd;
    pfd.events = POLLIN;
    pfd.fd = _timerFd;

    _isRunning = true;
    while (_isRunning)
    {
        int nready = -1;
        do
        {
            nready = ::poll(&pfd, 1, -1); // 4
        } while (nready == -1 && errno == EINTR);

        if (nready == -1)
        {
            perror("poll");
            return;
        }
        else if (0 == nready)
            printf("poll timeout!\n");
        else // 定时器到时
        {
            if (pfd.revents & POLLIN)
            {
                handleRead();
                if (_timerCb)
                    _timerCb();
            }
        }
    }
}

void Timer::stop()
{
    _isRunning = false;
    setTimerFd(0, 0);
}

int Timer::createTimerFd()
{
    int fd = timerfd_create(CLOCK_REALTIME, 0);
    if (fd < 0)
    {
        perror("timerfd_create");
    }
    return fd;
}

void Timer::setTimerFd(int init, int periodic)
{
    struct itimerspec value;
    value.it_value.tv_sec = init; // 不能设置为 0！
    value.it_value.tv_nsec = 0;
    value.it_interval.tv_sec = periodic;
    value.it_interval.tv_nsec = 0;
    int ret = ::timerfd_settime(_timerFd, 0, &value, nullptr);
    if (ret < 0)
    {
        perror("timerfd_settime");
    }
}

void Timer::handleRead()
{
    uint64_t value; // 可以不设初值
    int ret = ::read(_timerFd, &value, sizeof(value));
    if (ret != sizeof(value))
    {
        perror("read");
    }
}
}; // namespace wdcpp
