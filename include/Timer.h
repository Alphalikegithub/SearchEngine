#pragma once

#include <functional>
using std::function;

namespace wdcpp
{
class Timer
{
    using TimerCallBack = function<void()>;

public:
    Timer(TimerCallBack &&, int, int);

    void start();
    void stop();

private:
    int createTimerFd();
    void setTimerFd(int, int);
    void handleRead();

private:
    TimerCallBack _timerCb;
    int _timerFd;
    int _initTime;
    int _periodicTime;
    bool _isRunning;
};
}; // namespace wdcpp
