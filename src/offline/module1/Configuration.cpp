#include "../../../include/Configuration.h"
#include <stdlib.h>
#include <iostream>
#include <iostream>
#include <sstream>
#include <fstream>
using std::ifstream;
using std::stringstream;
using std::cout;
using std::endl;

namespace  wdcpp
{
Configuration *Configuration::_pInstance = Configuration::getInstance();

Configuration::Configuration(const string &filepath)
        :_filepath(filepath)
    {
        loadConf();        
    }

void Configuration::loadConf()
{
    ifstream ifs(_filepath, ifstream::in);
    if(!ifs.good())
    {
        cout << "can not open file" << endl;
        return;
    }

    string lines;
    string first,second;
    while(getline(ifs, lines))
    {
        stringstream ss(lines);
        ss >> first >> second;
        _configMap[first] = second;
    }

    cout << "[ _configMap is loading... ]\n"
         << endl;
    for(auto &item : _configMap)
    {
        cout << item.first << " " << item.second << endl;
    }
    cout << "\n[ _configMap finish loading ]\n"
         << endl;
    ifs.close();
}

Configuration *Configuration::getInstance()
{
    if(_pInstance == nullptr)
    {
        _pInstance = new Configuration("../conf/myconf.conf");
        atexit(destroy);
    }

    return _pInstance;
}

void Configuration::destroy()
{
    if(_pInstance)
    {
        delete _pInstance;
        _pInstance = nullptr;
    }
}

unordered_map<string, string> &Configuration::getConfigMap()
{
    return _configMap;
}
}//end of namespace wdcpp

