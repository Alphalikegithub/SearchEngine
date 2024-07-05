#include "../../include/TimerThread.h"

#include <iostream>
using std::cout;
using std::endl;

namespace wdcpp
{
TimerThread::TimerThread(TimerCallBack &&_cb, int init, int periodic)
    : _timer(std::move(_cb), init, periodic),
      _thread(std::bind(&Timer::start, &_timer))
{
}

void TimerThread::start()
{
    _thread.create();
}

void TimerThread::stop()
{
    _timer.stop();
    _thread.join();
}
}; // namespace wdcpp
