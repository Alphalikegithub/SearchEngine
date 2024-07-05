#include "../../../include/TcpConnection.h"
#include "../../../include/EventLoop.h"
#include "../../../include/ERROR_CHECK.h"
#include <sys/socket.h>
#include <string.h>
#include <sstream>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;
using std::ostringstream;

namespace wdcpp
{
TcpConnection::TcpConnection(int fd, EventLoop *loopPtr)
    : _clientSock(fd),
      _sockIO(fd),
      _localAddr(getLocalAddr()),
      _peerAddr(getPeerAddr()),
      _isShutDownWrite(false),
      _loopPtr(loopPtr)
{
}


//发送数据
void TcpConnection::send(const string &msg)
{
    /*
    const size_t length = msg.size();
    _sockIO.writen(&length, sizeof(size_t)); // 发送车头
    _sockIO.writen(msg.c_str(), length);     // 发送车厢
    */
    _sockIO.wirten_1(msg);
    
    //_sockIO.writen(msg.c_str(), msg.size());
}

//获取数据
string TcpConnection::recv()
{
    /* size_t length = 0;
    _sockIO.readn(&length, sizeof(size_t)); // 接收车头
    cout << 1111111 << endl;
    cout << length << endl;
    char buf[length + 1] = {0};
    cout << 2222222 << endl;
    */
    char buf[65535] = {0};
    //_sockIO.readn(buf, length); // 接收车厢
    _sockIO.readline(buf, sizeof(buf)); 

    return string(buf);
}

/**
 *  获取一行数据
 */
string TcpConnection::recvLine()
{
    char buf[65536] = {0};                           // 64K
    size_t ret = _sockIO.readline(buf, sizeof(buf)); // 若成功读取，buf 的尾部一定为 ...\n\0...

    if (ret > sizeof(buf) || ret < strlen(buf)) // 出错或连接已断开
        return {};
    else if (ret == strlen(buf) && buf[ret - 1] != '\n') // buf 不含 \n
    {
        ERROR_PRINT("recv: msg do not contain \\n\n");
        return {};
    }

    string res(buf);

    return res.substr(0, res.size() - 1); // 剔除末尾的 \n 再返回
}

string TcpConnection::show()
{
    ostringstream oss;

    oss << "[tcp] (local)" << _localAddr.ip() << ":" << _localAddr.port()
        << " --> "
        << "(peer)" << _peerAddr.ip() << ":" << _peerAddr.port();

    return oss.str();
}

bool TcpConnection::isClosed() const
{
    char tmp_buf[128] = {0};
    int ret = 0;

    do
    {
        ret = ::recv(_clientSock.fd(), tmp_buf, sizeof(tmp_buf), MSG_PEEK); // 扫描 _clientSock._fd 的 RCV 但不取出数据
    } while (ret == -1 && errno == EINTR);                                  // 收到中断信号直接忽略

    return ret == 0; // 返回 0 表示连接已断开
}

InetAddress TcpConnection::getLocalAddr()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);

    int ret = ::getsockname(_clientSock.fd(), (struct sockaddr *)&addr, &len);
    if(ret == -1)
    {
        perror("getsockname");
    }
    return InetAddress(addr);
}

InetAddress TcpConnection::getPeerAddr()
{
    struct sockaddr_in addr;
    socklen_t len = sizeof(struct sockaddr);

    int ret = ::getpeername(_clientSock.fd(), (struct sockaddr *)&addr, &len);
    if(ret == -1)
    {
        perror("getpeername");
    }

    return InetAddress(addr);
}

void TcpConnection::setConnectionCallBack(const TcpConnectionCallBack &onConnectionCb)
{
    // _onConnectionCb = std::move(onConnectionCb);
    _onConnectionCb = onConnectionCb;
}

void TcpConnection::setMessageCallBack(const TcpConnectionCallBack &onMessageCb)
{
    // _onMessageCb = std::move(onMessageCb);
    _onMessageCb = onMessageCb;
}

void TcpConnection::setCloseCallBack(const TcpConnectionCallBack &onCloseCb)
{
    // _onCloseCb = std::move(onCloseCb);
    _onCloseCb = onCloseCb;
}

void TcpConnection::handleConnectionCallBack()
{
    if (_onConnectionCb)
        _onConnectionCb(shared_from_this());
}

void TcpConnection::handleMessageCallBack()
{
    if (_onMessageCb)
        _onMessageCb(shared_from_this());
}

void TcpConnection::handleCloseCallBack()
{
    if (_onCloseCb)
        _onCloseCb(shared_from_this());
}

/**
 *  将 send(response) 作为回调函数传入 loop，并将主线程从 loop 中唤醒
 *
 *  1. 子线程调用
 *  2. 流程：notifyLoop -> setPendingCallBack -> writeCounter
 *  3. 这里只能先注册、后写 counter，顺序不能调转，否则会出现 BUG
 */
void TcpConnection::notifyLoop(string msg)
{
    // cout << "Worker Thread: notifyLoop" << endl;

    // 注册 onPendingCb（即插入一个 onPendingCb）
    _loopPtr->setPendingCallBack(std::bind(&TcpConnection::send, this, msg));

    // 向内核计数器中写值
    _loopPtr->writeCounter();
}

int TcpConnection::getFd()
{
    return _clientSock.fd();
}
};
