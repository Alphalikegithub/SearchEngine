#pragma once
#include "Socket.h"
#include "InetAddress.h"

namespace wdcpp
{
//接受器类
//1.负责服务器监听前的准备工作（设置地址/端口可重用、绑定网络地址信息）
//2.开启监听
//3.获取新的客户端套接字
//4.提供获取新的 clientSock 的方法
class Acceptor
{
public:
    explicit Acceptor(const string &, unsigned short);

    void prepare();
    int accept();
    int fd();

private:
    void setReuseAddr(bool);
    void setReusePort(bool);
    void bind();
    void listen();

private:
    Socket _listenSock;
    InetAddress _serverAddr;
};
}
