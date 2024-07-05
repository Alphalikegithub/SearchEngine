#pragma once
#include "Acceptor.h"
#include "TcpConnection.h"
#include "MutexLockGuard.h"

#include <sys/epoll.h>
#include <vector>
#include <map>

using std::map;
using std::vector;

namespace wdcpp
{
//epoll 事件循环列
//
//1.3个 EventLoopCallBack 函数对象就是为了传给 Tcpconnection
//  让 TcpConnection 注册的
class EventLoop
{
    friend void TcpConnection::notifyLoop(string);

public:
    using EventLoopCallBack = TcpConnection::TcpConnectionCallBack;

private:
    using EventList = vector<struct epoll_event>;
    using ConnectionMap = map<int, TcpConnectionPtr>;
    using PendingCallBack = function<void()>;

public:
    EventLoop(Acceptor &);
    ~EventLoop();

    void loop();
    void unloop();

    void setConnectionCallBack(EventLoopCallBack &&);
    void setMessageCallBack(EventLoopCallBack &&);
    void setCloseCallBack(EventLoopCallBack &&);
    void setPendingCallBack(PendingCallBack &&);

private:
    int createEpoll();
    void addEpollFd(int);
    void delEpollFd(int);

    int createEvent();
    void readCounter();
    void writeCounter();

    void waitEpoll();
    void handleNewConnection();
    void handleOldConnection(int);
    void handlePendingCbs();

private:
    const size_t INIT_RPOLLNUM = 1024; //初始时监听事件的最大数目
    int _epFd;                         //监听集合（红黑树根节点）
    int _eventFd;                      //内核计数器
    Acceptor &_acceptor;               //接收器
    bool _isLooping;                   //循环开启标志
    EventList _eventList;              //就绪事件链表
    ConnectionMap _connMap;            //已连接集合

    EventLoopCallBack _onConnectionCb;
    EventLoopCallBack _onMessageCb;
    EventLoopCallBack _onCloseCb;

    MutexLock _mutex;
    vector<PendingCallBack> _pendingCbs;  //延迟事件处理器集合（即多个 send 操作）
};
}
