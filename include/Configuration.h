#pragma once

#include <stdlib.h>
#include <string>
#include <unordered_map>
using std::string;
using std::unordered_map;

namespace wdcpp
{
/*************************************************************
 *
 *  配置文件类
 *
 *************************************************************/
class Configuration
{
public:
    static Configuration *getInstance();
    unordered_map<string, string> &getConfigMap();

private:
    Configuration(const string &);
    ~Configuration(){};

    void loadConf();

    static void destroy();

private:
    string _filepath;
    unordered_map<string, string> _configMap; // <pages, ../yuliao/pages>
    static Configuration *_pInstance;
};
}; // namespace wdcpp
