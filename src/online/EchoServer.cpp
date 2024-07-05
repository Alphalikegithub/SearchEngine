#include "../../include/EchoServer.h"
#include "../../include/MyTask.h"
#include "../../include/MyLog.h"
#include "../../include/TimerTask.h"
#include "../../include/Configuration.h"
// #include "../../include/workflow/WFHttpServer.h"
// #include "../../include/workflow/HttpMessage.h"
// #include "../../include/workflow/WFFacilities.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <functional>

#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
#include <unordered_map>

#include "nlohmann/json.hpp"
#include "../../include/fifo_map.hpp"
using namespace nlohmann;
/* 以下为 nlohmann/json 库使用，保证插入顺序不变 */
template <class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, A>;
using my_json = basic_json<my_workaround_fifo_map>;
using Json = my_json;



namespace wdcpp
{
#define INIT_WORKER_NUM 6
#define INIT_TASKQUEUE_CAPACITY 10

EchoServer::EchoServer(const string &ip, unsigned short port)
    :_pool(INIT_WORKER_NUM, INIT_TASKQUEUE_CAPACITY),
     _server(ip, port),
     _webPageSearcher(),
     _redis("tcp://127.0.0.1:6379"),
     _timerThread(std::bind(&TimerTask::process, TimerTask()),
                  stoi(Configuration::getInstance()->getConfigMap()["initTime"]),
                  stoi(Configuration::getInstance()->getConfigMap()["periodicTime"]))
{
}

/*EchoServer::EchoServer(const string &ip, unsigned short port)
    :_pool(INIT_WORKER_NUM, INIT_TASKQUEUE_CAPACITY),
     _server(ip, port),
     _webPageSearcher(),
     _timerThread(std::bind(&TimerTask::process, TimerTask()),
                  stoi(Configuration::getInstance()->getConfigMap()["initTime"]),
                  stoi(Configuration::getInstance()->getConfigMap()["periodicTime"]))
{
}
*/
void EchoServer::start()
{
    _pool.start();

    _timerThread.start();

    using namespace std::placeholders;
    _server.setConnectionCallBack(std::bind(&EchoServer::onConnection, this, _1));
    _server.setMessageCallBack(std::bind(&EchoServer::onMessage, this, _1));
    _server.setCloseCallBack(std::bind(&EchoServer::onClose, this, _1));

    _server.start();
}

void EchoServer::stop()
{
    _server.stop();

    _timerThread.stop();

    _pool.stop();
}

void EchoServer::onConnection(const TcpConnectionPtr &connPtr)
{
    LogInfo("\n\t%s connected", connPtr->show().c_str());
    cout << connPtr->show() << " connected!" << endl;
}

/*
void EchoServer::onMessage(const TcpConnectionPtr &connPtr)
{
    // recv
    Json root;
    string msg = connPtr->recv();
    cout << ">>recv msg from client: " << msg << endl;
    
    cout << 1111111 << endl;
    root = json::parse(msg); // 解析 _msg
    cout << 22222222 << endl;
    if (root.is_object())
    {
        cout << 33333333 << endl;
        size_t msgID = root["msgID"];
        cout << 4444444 << endl;
        if (msgID == 3)
        {
            // 处理跨域请求
            int fd = connPtr->getFd();
            std::string response = root.dump();
            handleResponse(fd, response);
        }
        else
        {
            // 处理其他请求
            MyTask task(msg, connPtr, _webPageSearcher, _recommander, _redis);
            _pool.addTask(std::bind(&MyTask::process, task)); // 因此 ThreadPool ..> MyTask
        }
    }
}
*/

unsigned char FromHex(unsigned char x) 
{ 
    unsigned char y;
    if (x >= 'A' && x <= 'Z') y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z') y = x - 'a' + 10;
    else if (x >= '0' && x <= '9') y = x - '0';
    else assert(0);
    return y;
}


std::string UrlDecode(const std::string& str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (str[i] == '+') strTemp += ' ';
        else if (str[i] == '%')
        {
            assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high*16 + low;
        }
        else strTemp += str[i];
    }
    return strTemp;
}

void get_url_and_params(const string& http_request, string& url, unordered_map<string, string>& params) {
    // 从HTTP请求中获取请求行
    size_t start = http_request.find(" ");
    if (start == string::npos) {
        cout << "Invalid HTTP request" << endl;
        return;
    }
    size_t end = http_request.find(" ", start + 1);
    if (end == string::npos) {
        cout << "Invalid HTTP request" << endl;
        return;
    }
    string request_line = http_request.substr(start + 1, end - start - 1);

    // 查找请求行中GET方法所请求的URL
    start = request_line.find("?");
    if (start == string::npos) {
        url = request_line;
    } else {
        url = request_line.substr(0, start);
        string params_str = request_line.substr(start + 1);
        size_t pos = 0;
        while (pos != string::npos) {
            size_t end = params_str.find("&", pos);
            string param_str;
            if (end == string::npos) {
                param_str = params_str.substr(pos);
                pos = end;
            } else {
                param_str = params_str.substr(pos, end - pos);
                pos = end + 1;
            }
            size_t eq_pos = param_str.find("=");
            if (eq_pos != string::npos) {
                string key = param_str.substr(0, eq_pos);
                string value = param_str.substr(eq_pos + 1);
                params[key] = value;
            }
        }
    }
}

string handleMsg(string query)
{
    Json root;
    root["msgID"] = 2;
    root["msg"] = query;
    string msg = root.dump(4);
    return msg;
}

void EchoServer::onMessage(const TcpConnectionPtr &connPtr)
{
    // recv
    Json root;
    string http_request = connPtr->recv();
    cout << "start:" << http_request << endl;
    
    string url;
    unordered_map<string, string> params;    //输入信息
    get_url_and_params(http_request, url, params);
    string decode_url = UrlDecode(url);                //url decode解码
    cout << "decode_url: " << decode_url << endl;
    
    if(params.size() == 0)
    {
        return;
    }
    string decode_msg = UrlDecode(params.begin()->second);   //msg decode解码
    string msg = handleMsg(decode_msg);                 //将信息写为JSON格式
    /*
    cout << "URL: " << url << endl;
    cout << "Params: ";
    for (auto p : params) {
        cout << p.first << "=" << p.second << " ";
    }
    cout << endl;
    */
   
    cout << ">>recv msg from client: " << msg << endl;
    
    root = json::parse(msg); // 解析 _msg
    if (root.is_object())
    {
        size_t msgID = root["msgID"];
        //int fd = connPtr->getFd();                       //只要是查询请求都处理一下跨域请求
        //std::string response = root.dump();
         //handleResponse(fd, response);   
        // 处理其他请求
        MyTask task(msg, connPtr, _webPageSearcher, _recommander, _redis,decode_url);
        _pool.addTask(std::bind(&MyTask::process, task)); // 因此 ThreadPool ..> MyTask
        
    }
    
    /*
    // decode -> compute -> encode -> send
    MyTask task(msg, connPtr, _webPageSearcher, _recommander, _redis);
    //MyTask task(msg, connPtr, _webPageSearcher, _recommander);
    _pool.addTask(std::bind(&MyTask::process, task)); // 因此 ThreadPool ..> MyTask
    // _pool.addTask(std::bind(&MyTask::process, &task));
    */
}

void EchoServer::onClose(const TcpConnectionPtr &connPtr)
{
    LogInfo("\n\t%s disconnected", connPtr->show().c_str());
    cout << connPtr->show() << " disconnected!" << endl;
}
}//end of namespace wdcpp

