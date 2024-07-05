#pragma once
#include <arpa/inet.h>
#include <string>

using std::string;

namespace wdcpp
{
//网络地址类
//提供 ip 和 port 与 sockaddr_in 类型的 _addr 的转换方法
class InetAddress
{
public:
    explicit InetAddress(const string &, unsigned short);
    explicit InetAddress(const struct sockaddr_in &);

    string ip() const;
    unsigned short port() const;
    const struct sockaddr_in *getInetAddress() const;

private:
    struct sockaddr_in _addr;
};
}
