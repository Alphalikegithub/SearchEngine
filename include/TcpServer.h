#pragma once
#include "Acceptor.h"
#include "EventLoop.h"

namespace wdcpp
{
class TcpServer
{
public:
    using TcpConnectionCallBack = EventLoop::EventLoopCallBack;

public:
    TcpServer(const string &, unsigned short);

    void start();
    void stop();

    void setConnectionCallBack(TcpConnectionCallBack &&);
    void setMessageCallBack(TcpConnectionCallBack &&);
    void setCloseCallBack(TcpConnectionCallBack &&);

private:
    Acceptor _acceptor;
    EventLoop _loop;
};
}
