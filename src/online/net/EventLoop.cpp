#include "../../../include/EventLoop.h"
#include "../../../include/ERROR_CHECK.h"

#include <sys/eventfd.h>
#include <unistd.h>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

namespace wdcpp
{
EventLoop::EventLoop(Acceptor &acceptor)
: _epFd(createEpoll()), 
 _eventFd(createEvent()),
 _isLooping(false),
 _acceptor(acceptor),
 _eventList(1024)
{
    //将listenfd放在红黑树上进行监听
    addEpollFd(_acceptor.fd());
    //eventfd也需要进行监听
    addEpollFd(_eventFd);
}

EventLoop::~EventLoop() 
{
    close(_epFd);
    close(_eventFd);
}

void EventLoop::loop()
{
    _isLooping = true;
    while(_isLooping)
    {
        waitEpoll();
    }
}

void EventLoop::unloop()
{
    _isLooping = false;
}

void EventLoop::setConnectionCallBack(EventLoopCallBack &&onConnectionCb)
{
    _onConnectionCb = std::move(onConnectionCb);
}

void EventLoop::setMessageCallBack(EventLoopCallBack &&onMessageCb)
{
    _onMessageCb = std::move(onMessageCb);
}

void EventLoop::setCloseCallBack(EventLoopCallBack &&onCloseCb)
{
    _onCloseCb =  std::move(onCloseCb);
}

void EventLoop::waitEpoll()
{
    int nready;
    do
    {
        /* ::epoll_wait(_epfd, _evtList.data(), _evtList.size(), 5000); */
        nready = ::epoll_wait(_epFd, &*_eventList.begin(),_eventList.size(), 5000);
    }while(-1 == nready && errno == EINTR);
    
    if(-1 == nready)
    {
        perror("epoll_wait error\n");
        return;
    }
    else if(0 == nready)
    {
        cout << ">>epoll_wait timeout" << endl;
    }
    else
    {
        //考虑到扩容的问题，因为初始情况监听的文件描述符只写了1024
        if(nready == (int)_eventList.size())
        {
           _eventList.resize(2 * nready);
        }
        
        for(int idx = 0; idx < nready; ++idx)
        {
            if(_eventList[idx].data.fd == _acceptor.fd())
            {
                if(_eventList[idx].events & EPOLLIN)
                {
                    //处理新的连接
                    handleNewConnection();
                }
            }
            else if(_eventList[idx].data.fd == _eventFd)
            {
                if(_eventList[idx].events & EPOLLIN)
                {
                    readCounter(); // 将内核计数器的值取出
                    handlePendingCbs(); // 执行所有的 pendingCb，即 send(response)
                }
            }
            else
            {
                //如果是老的连接,connfd
                if(_eventList[idx].events & EPOLLIN)
                {
                    //处理老的连接上发过来的数据
                     handleOldConnection(_eventList[idx].data.fd);
                }
            }
        }
    }
}

void EventLoop::handleNewConnection()
{
    int peerFd = _acceptor.accept();
    if( peerFd < 0)
    {
        perror("handleNewConnection\n");
        return;
    }
    //将新的文件描述符connfd放到红黑树进行监听
     addEpollFd( peerFd);
    
    //如果connfd是正常的，那么就表明三次握手成功，表明连接就建立好了
    TcpConnectionPtr con(new TcpConnection( peerFd, this));

    //注册三个回调函数
    con->setConnectionCallBack(_onConnectionCb);
    con->setMessageCallBack(_onMessageCb);
    con->setCloseCallBack(_onCloseCb);

     _connMap.insert(std::make_pair(peerFd, con)); // 将该条新连接加入已连接集合 _connMap

    con->handleConnectionCallBack(); // 执行新连接事件的事件处理器
}

void EventLoop::handleOldConnection(int peerFd)
{
    auto conn_it = _connMap.find(peerFd); // 查找 peerFd 的那条连接 conn

    if (conn_it != _connMap.end())
    {
        if (conn_it->second->isClosed()) // conn 已断开
        {
            conn_it->second->handleCloseCallBack(); // 执行连接断开事件的事件处理器

            delEpollFd(peerFd);      // 将 peerFd 从监听集合 _epFd 中删除，即不再监听它了
            _connMap.erase(conn_it); // 将这条已断开的连接 conn 从已连接集合 _connMap 中删除（完成 TCP 挥手）
        }
        else                                          // conn 没有断开
            conn_it->second->handleMessageCallBack(); // 执行新消息事件的事件处理器
    }
    else
    {
        cout << "该连接不存在 " << endl;
        return;
    }
}

int EventLoop::createEpoll()
{
    int fd = ::epoll_create1(0);
    if(fd < 0)
    {
        perror("epoll_create1\n");
        return fd;
    }

    return fd;
}

void EventLoop::addEpollFd(int fd)
{
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = fd;

    int ret = ::epoll_ctl(_epFd, EPOLL_CTL_ADD, fd, &evt);
    if(ret < 0)
    {
        perror("epoll_ctl add\n");
        return;
    }
}

void EventLoop::delEpollFd(int fd)
{
    struct epoll_event evt;
    evt.data.fd = fd;

    int ret = ::epoll_ctl(_epFd, EPOLL_CTL_DEL, fd, &evt);
    if(ret < 0)
    {
        perror("epoll_ctl add\n");
        return;
    }
}

int EventLoop::createEvent()
{
    int fd = ::eventfd(0, 0);
    if(fd < 0)
    {
        perror("eventfd\n");
    }

    return fd;
}

void EventLoop::writeCounter()
{
    // cout << "Worker Thread: writeCounter" << endl;
    uint64_t one = 1;
    int ret = ::write(_eventFd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("write\n");
        return;
    }
}

void EventLoop::setPendingCallBack(PendingCallBack &&cb)
{
    MutexLockGuard autoLock(_mutex);

    _pendingCbs.push_back(std::move(cb));
}

void EventLoop::readCounter()
{
    uint64_t one;
    ssize_t ret = read(_eventFd, &one, sizeof(one));
    if(ret != sizeof(one))
    {
        perror("read\n");
        return;
    }
}

void EventLoop::handlePendingCbs()
{
    vector<PendingCallBack> tmp;//局部变量
    {
        MutexLockGuard autoLock(_mutex);
        tmp.swap(_pendingCbs);
    }
    //对共享资源vector进行了读操作
    //在读操作的过程中有没有写的可能呢？那如果有，
    //就会发生读与写共存的现象，那么这种情况就
    //有可能发生意想不到bug
    for(auto &cb: tmp)
    {
        cb();
    }
}

};
