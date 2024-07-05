#include "../../include/Configuration.h"
#include "../../include/ERROR_CHECK.h"
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <sstream>
using std::ifstream;
using std::stringstream;

namespace wdcpp
{
Configuration *Configuration::_pInstance = Configuration::getInstance(); // 饿汉

Configuration::Configuration(const string &filepath)
    : _filepath(filepath)
{
    loadConf();
}

/**
 *  加载配置文件，并将配置信息存入 _configMap
 */
void Configuration::loadConf()
{
    ifstream ifs(_filepath, ifstream::in);
    if (!ifs.good())
    {
        ERROR_PRINT("can not open file");
    }

    string lines;
    string first, second;
    while (getline(ifs, lines))
    {
        stringstream ss(lines);
        ss >> first >> second;
        _configMap[first] = second;
    }

    std::cout << "[ _configMag is loading... ]\n"
              << std::endl;
    for (auto &item : _configMap)
    {
        std::cout << item.first << " " << item.second << std::endl;
    }
    std::cout << "\n[ _configMag finish loading ]\n"
              << std::endl;

    ifs.close();
}

Configuration *Configuration::getInstance()
{
    if (_pInstance == nullptr)
    {
        _pInstance = new Configuration("../conf/myconf.conf");
        atexit(destroy);
    }

    return _pInstance;
}

void Configuration::destroy()
{
    using namespace std;

    if (_pInstance)
    {
        delete _pInstance;
        _pInstance = nullptr;
    }
}

unordered_map<string, string> &Configuration::getConfigMap()
{
    return _configMap;
}
}; // namespace wdcpp
