#pragma once
#include "Socket.h"
#include "InetAddress.h"
#include "SocketIO.h"

#include <memory>
#include <functional>

using std::function;
using std::shared_ptr;

namespace wdcpp
{
//TCP 连接类
//1.四元组，描述了一条已建立的连接
//2.作为与客户交互的接口，提供 msg 发送/接受的接口
//3.首先要从 Acceptor 中获取 clientSock 再建立该连接类，此后就可以
//  通过该类与 client 进行交互

class EventLoop;
class TcpConnection;
using TcpConnectionPtr = shared_ptr<TcpConnection>;

class TcpConnection
    :NonCopyable,
     public std::enable_shared_from_this<TcpConnection>
{
public:
    using TcpConnectionCallBack = function<void(const TcpConnectionPtr &)>;

public:
    explicit TcpConnection(int, EventLoop *);

    void send(const string &);
    void sendInLoop(const string &msg);
    string recv();
    string recvLine();
    string show();

    bool isClosed() const;

    void setConnectionCallBack(const TcpConnectionCallBack &);
    void setMessageCallBack(const TcpConnectionCallBack &);
    void setCloseCallBack(const TcpConnectionCallBack &);
    void handleConnectionCallBack();
    void handleMessageCallBack();
    void handleCloseCallBack();

    void notifyLoop(string);
    int getFd();

private:
    InetAddress getLocalAddr();
    InetAddress getPeerAddr();
    

private:
    Socket _clientSock;
    SocketIO _sockIO;
    InetAddress _localAddr;
    InetAddress _peerAddr;
    bool _isShutDownWrite;
    TcpConnectionCallBack _onConnectionCb;   //新连接事件的事件处理器（回调函数）
    TcpConnectionCallBack _onMessageCb;   //新连接事件的事件处理器（回调函数）
    TcpConnectionCallBack _onCloseCb;   //新连接事件的事件处理器（回调函数）
    EventLoop *_loopPtr;
};
}
