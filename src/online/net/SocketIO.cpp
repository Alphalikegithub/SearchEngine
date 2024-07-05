#include "../../../include/SocketIO.h"
#include "../../../include/ERROR_CHECK.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <sstream>
#include <string>
#include <codecvt>
#include <locale>
//#include <boost/asio.hpp>

//using boost::asio::ip::tcp;
using std::string;

namespace wdcpp
{
SocketIO::SocketIO(int fd)
    : _fd(fd)
{
}

/*
 *  1. 从 RCV 中至少读取 count 个字节到 buf 中
 *  2. 返回成功读到的字节数
 *  3. 出错返回 count + 1
 *  4. 不保证字符串 buf 以 \0 结尾
 */
size_t SocketIO::readn(void *buf, size_t count)
{
    int left = count;        // 待读取的字节数
    char *ptr = (char *)buf; // 下一个字节写入的位置

    int ret = 0;

    while (left > 0)
    {
        do
        {
            ret = ::recv(_fd, ptr, left, 0);
        } while (-1 == ret && errno == EINTR); // 中断事件直接忽略

        if (-1 == ret)
        {
            ::perror("recv");
            return count - ret; // 返回 count + 1
        }
        else if (0 == ret) // peerFd 已断开
        {
            printf("recv: peerFd(%d) disconnected\n", _fd);
            break;
        }
        else // 更新 ptr 和 left
        {
            ptr += ret;
            left -= ret;
        }
    }

    return count;
}


/*
size_t SocketIO::readn(void *buf, size_t count)
{
    int left = count;
    char *pstr = (char *)buf;
    int ret = 0;

    while(left > 0)
    {
        ret = read(_fd, pstr, left);
        if(-1 == ret && errno == EINTR)
        {
            continue;
        }
        else if(-1 == ret)
        {
            perror("read error -1");
            return count - ret;
        }
        else if(0 == ret)
        {
            break;
        }
        else
        {
            pstr += ret;
            left -= ret;
        }
    }

    return count - left;
}
*/

/*
 *  1. 将 buf 中的恰好 count 个字节写入到 SND 中
 *  2. 返回成功写入的字节数
 *  3. 出错返回 count + 1
 *  4. 未写完返回值将 < count
 */
size_t SocketIO::writen(const void *buf, size_t count)
{
    int left = count;              // 待希尔的字节数
    const char *ptr = (char *)buf; // 下一个待写入字节的位置

    int ret = 0;

    while (left > 0)
    {
        do
        {
            ret = ::send(_fd, ptr, left, 0);
        } while (-1 == ret && errno == EINTR); // 中断事件直接忽略

        if (-1 == ret) // 出错
        {
            ::perror("send");
            return count - ret; // 返回 count + 1
        }
        else if (0 == ret) // peerFd 已断开
        {
            printf("send: peerFd(%d) disconnected\n", _fd);
            break;
        }
        else // 更新 ptr 和 left
        {
            ptr += ret;
            left -= ret;
        }
    }

    return count;
}

void SocketIO::wirten_1(const string &content)
{
    const std::string header = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: text/plain;charset=utf-8\r\nContent-Length: " + std::to_string(content.size()) + "\r\n\r\n";
    std::string data = header + content;
    int ret = ::send(_fd, data.c_str(), data.size(), 0);
    if (ret == -1) {
        perror("send");
        close(_fd);
    }
    /*
    std::ostringstream response_stream;
    response_stream << "HTTP/1.1 200 OK\r\n"
                    << "Content-Type: application/json\r\n"
                    << "Content-Length: " << content.length() << "\r\n"
                    << "\r\n"
                    << content;
    string responseStr = response_stream.str();
    std::cout << "responseStr: " << responseStr << std::endl;
    ::send(_fd, responseStr.c_str(), responseStr.size(), 0);
    */
}
/*
 *  1. 从 RCV 中读取一行数据到 buf 中，这一行最大不超过 max 个字节
 *  2. 若成功读入一整行则返回成功读到的字节数
 *     若读完 max 仍未找到 \n，返回成功读到的字节数
 *     若连接已断开，返回一个小于 strlen(buf) 的值
 *  3. 出错返回 max + 1
 *  4. 不保证字符串 buf 以 \0 结尾
 */
/*
size_t SocketIO::readline(char *buf, size_t max)
{
    int left = max;  // 还可读取的字节数
    int total = 0;   // 当前已读取的字节数
    char *ptr = buf; // 下一个字节写入的位置

    int ret = 0;

    size_t rcv_size = 0;
    socklen_t optlen = sizeof(rcv_size);
    ret = ::getsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &optlen);
    if(ret == -1) 
    {
        std::cout << "Error: getsockopt"  << std::endl;
    }
    if (rcv_size < max - 1) // max 超出 RCV 的大小，直接报错
    {
        printf("readline: max is too large. max should less than or equal to %ld\n", rcv_size + 1);
        return max + 1;
    }

    while (left > 0)
    {
        do
        {
            ret = ::recv(_fd, ptr, left, MSG_PEEK); // 扫描 RCV 但不取出
        } while (-1 == ret && errno == EINTR);      // 中断事件直接忽略

        if (-1 == ret) // 出错
        {
            ::perror("recv");
            return max - ret; // max + 1
        }
        else if (0 == ret) // peerFd 已断开
        {
            printf("recv: peerFd(%d) disconnected\n", _fd);
            break;
        }
        else
        {
            int realReadCnt = 0;
            for (int idx = 0; idx < ret && idx < left; ++idx, ++realReadCnt)
            {
                if (ptr[idx] == '\n')
                {
                    int sz = idx + 1;

                    int ret = readn(ptr, sz); // 将 RCV 中已扫描到的数据（包含 \n）取出存入 buf
                    if (ret > sz)             // 出错，返回 max + 1
                        return max + 1;
                    else if (ret < sz) // 未读完连接已断开，应该返回比 strlen(buf) 更小的值
                    {
                        total += ret;
                        left -= ret;
                        return (total - left) < 0 ? 0 : total - left;
                    }

                    return total + sz;
                }
            }

            int ret = readn(ptr, realReadCnt); // 将 RCV 中已扫描的数据（不含 \n）取出存入 buf
            if (ret > realReadCnt)             // 出错，返回 max + 1
                return max + 1;
            total += ret;
            ptr += ret;
            left -= ret;
        }
    }

    char clean_buf[1024] = {0};
    do
    {
        ret = ::recv(_fd, clean_buf, sizeof(clean_buf), MSG_DONTWAIT); // 非阻塞读，清空 RCV
        // 若连接未断开，且 RCV 中有数据，返回 > 0
        // 若连接未断开，且 RCV 中没有数据，返回 < 0
        // 若连接已断开，直接返回 0
    } while (ret > 0 || (ret == -1 && errno == EINTR));

    return (total - left) < 0 ? 0 : total - left;
}
*/

/*
size_t SocketIO::readline(char *buf, size_t max)
{
    int left = max - 1;
    char *pstr = buf;
    int ret = 0, total = 0;
    std::cout << "left: " << left << std::endl;

    while(left > 0)
    {
        //MSG_PEEK不会将缓冲区中的数据进行清空,只会进行拷贝操作
        ret = recv(_fd, pstr, left, MSG_PEEK);
        std::cout << "ret: " << ret << std::endl;
        if(-1 == ret && errno == EINTR)
        {
            continue;
        }
        else if(-1 == ret)
        {
            perror("readLine error -1");
            return max - ret;
        }
        else if(0 == ret)
        {
            break;
        }
        else
        {
            for(int idx = 0; idx < ret; ++idx)
            {
                if(pstr[idx] == '\n')
                {
                    int sz = idx + 1;
                    readn(pstr, sz);
                    pstr += sz;
                    *pstr = '\0';//C风格字符串以'\0'结尾

                    return total + sz;
                }
            }

            readn(pstr, ret);//从内核态拷贝到用户态
            total += ret;
            pstr += ret;
            left -= ret;
        }
    }
     *pstr = '\0';

    return total - left;

}
*/

/*
size_t SocketIO::readline(char *buf, size_t max)
{
    int n = read(_fd,buf,max);
    buf[n] = '\0';
}
*/

size_t SocketIO::readline(char *buf, size_t max)
{
    string http_request = "";
    int recv_bytes = 0;
    while ((recv_bytes = recv(_fd, buf, max - 1, 0)) > 0) 
    {
        buf[recv_bytes] = '\0';
        http_request += buf;

        // 判断HTTP请求是否读取完毕
        if (http_request.find("\r\n\r\n") != string::npos)
        {
            break;
        }
    }
}
};
