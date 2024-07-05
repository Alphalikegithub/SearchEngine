#include "../../include/EchoServer.h"
#include "../../include/Configuration.h"
#include "../../include/CacheManager.h"

#include <iostream>
#include <unistd.h>
using std::cout;
using std::endl;
using namespace wdcpp;

void EchoServer::handleResponse(int fd, const std::string& response)
{
    const std::string header = "HTTP/1.1 200 OK\r\nAccess-Control-Allow-Origin: *\r\nContent-Type: text/plain\r\nContent-Length: " + std::to_string(response.size()) + "\r\n\r\n";
    std::string data = header + response;
    cout << "data: " << data << endl;

    int ret = ::send(fd, data.c_str(), data.size(), 0);
    if (ret == -1) {
        perror("send");
        close(fd);
    }
}

void EchoServer::handleRequest(int fd, const char* buf, ssize_t count)
{
    std::string response(buf, count);
    handleResponse(fd, response);
}

void test1()
{
    // 192.168.196.135 1234
    // 192.168.4.104 1234
    const string ip = Configuration::getInstance()->getConfigMap()["ip"];
    const unsigned short port = (unsigned short)stoul(Configuration::getInstance()->getConfigMap()["port"]);

    EchoServer server(ip, port);
    server.start();
}

int main()
{
    test1();
    return 0;
}
