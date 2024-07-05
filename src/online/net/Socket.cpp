#include "../../../include/Socket.h"
#include "../../../include/ERROR_CHECK.h"
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;

namespace wdcpp
{

Socket::Socket()
{
    _fd = ::socket(AF_INET, SOCK_STREAM, 0);
    ERROR_CHECK(_fd, -1, "socket");
}

Socket::Socket(int fd)
    : _fd(fd)
{
}

Socket::~Socket()
{
    while (::close(_fd) < 0 && errno == EINTR)
        ;
}

int Socket::fd() const
{
    return _fd;
}

void Socket::shutDownWrite()
{
    int ret = ::shutdown(_fd, SHUT_WR); // <sys/socket.h>
    ERROR_CHECK(ret, -1, "shutdown");
}

void Socket::setNonBlock()
{
    int flags = ::fcntl(_fd, F_GETFL, 0);
    flags |= O_NONBLOCK;
    ::fcntl(_fd, F_SETFL, 0);
}
};
