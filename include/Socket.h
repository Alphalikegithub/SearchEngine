#pragma once
#include "NonCopyable.h"

namespace wdcpp
{
//网络套接字
//1.负责套接字的创建和关闭（利用 RAII 技术管理）
//2.不具有复制语义
//3.提供更改套接字模式/选项的方法
class Socket
:NonCopyable
{
public:
    Socket();
    explicit Socket(int);
    ~Socket();

    int fd() const;
    void shutDownWrite();
    void setNonBlock();

private:
    int _fd;
};
}
