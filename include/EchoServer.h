#pragma once
#include "ThreadPool.h"
#include "TcpServer.h"
#include "WebPageSearcher.h"
#include "TimerThread.h"
#include "KeyRecommander.h"

#include <sw/redis++/redis++.h>
#include <iostream>
using std::cin;
using std::cout;
using std::endl;
using namespace sw::redis;

namespace wdcpp
{
class EchoServer
{
public:
    EchoServer(const string &, unsigned short);

    void start();
    void stop();

    void onConnection(const TcpConnectionPtr &);
    void onMessage(const TcpConnectionPtr &);
    void onClose(const TcpConnectionPtr &);

private:
    const size_t INIT_WORKER_NUM = 5;
    const size_t INIT_TASKQUEUE_CAPACITY = 10;
    ThreadPool _pool;
    TcpServer _server;
    WebPageSearcher _webPageSearcher;
    KeyRecommander _recommander; 
    sw::redis::Redis _redis;
    TimerThread _timerThread;
    void handleResponse(int fd, const std::string& response);
    void handleRequest(int fd, const char* buf, ssize_t count);
};
} // namespace wdcpp
