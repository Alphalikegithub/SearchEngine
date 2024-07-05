#pragma once
#include "TcpConnection.h"
#include "WebPageSearcher.h"
#include "KeyRecommander.h"
#include "Dictionary.h"

#include <unistd.h>

#include <sw/redis++/redis++.h>
#include <unistd.h>
#include <iostream>
using std::cout;
using std::endl;
using namespace sw::redis;

namespace wdcpp
{
class MyTask
{
public:
    MyTask(const string &msg, const TcpConnectionPtr &connPtr, WebPageSearcher &webPageSearcher, KeyRecommander &recommander, sw::redis::Redis &redis,const string &decode_url)
        : _msg(msg),
          _connPtr(connPtr),
          _webPageSearcher(webPageSearcher),
          _recommander(recommander),
          _redis(redis),
          _decode_url(decode_url)
    {
    }
    
    /*MyTask(const string &msg, const TcpConnectionPtr &connPtr, WebPageSearcher &webPageSearcher, KeyRecommander &recommander)
        : _msg(msg),
          _connPtr(connPtr),
          _webPageSearcher(webPageSearcher),
          _recommander(recommander)
    {
    }
    */
    void process(); // 由子线程（TheadPoll）调用！！！

private:
    string _msg;
    TcpConnectionPtr _connPtr;
    WebPageSearcher &_webPageSearcher;
    KeyRecommander &_recommander;
    sw::redis::Redis &_redis;
    string _decode_url;
};
}; // namespace wdcpp
