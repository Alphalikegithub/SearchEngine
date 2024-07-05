#pragma once
#include <string>

using std::string;

namespace wdcpp
{
//IO操作类
//1.提供了多种封装好的IO操作
//2.实现具体的IO操作
class SocketIO
{
public:
    explicit SocketIO(int);

    size_t readn(void *, size_t);
    size_t writen(const void *, size_t);
    size_t readline(char *, size_t);
    void wirten_1(const string &);

private:
    int _fd;
};
}
