#include "../../include/Dictionary.h"
#include "../../include/Configuration.h"

#include <iostream>
#include <sstream>
#include <fstream>

using std::cout;
using std::endl;
using std::ifstream;
using std::istringstream;

namespace wdcpp
{
Dictionary *Dictionary::_singletonDict = getInstance(); //饿汉模式
Dictionary *Dictionary::getInstance()
{
    if (_singletonDict == nullptr)
    {
        _singletonDict = new Dictionary();
        _singletonDict->initDict();
        _singletonDict->initIndex();
        _singletonDict->initEnDict();
        _singletonDict->initEnIndex();
        atexit(destory);
    }
    return _singletonDict;
}

void Dictionary::destory()
{
    if (_singletonDict)
    {
        delete _singletonDict;
        _singletonDict = nullptr;
    }
}

//将中文字典读入内存
void Dictionary::initDict()
{
    cout << "initialize dictionary" << endl;
    
    string dictPath = Configuration::getInstance()->getConfigMap()["dict"];
    ifstream ifs(dictPath);
    if (!ifs.good())
    {
        cout << "ifstream open file" << string(dictPath) << " error !" << endl;
        return;
    }
    string line;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        string word, freq;
        iss >> word >> freq;
        _dict.push_back(pair<string, int>(word, freq == "" ? 0 : (stoi(freq))));
    }
}

//将英文字典读入内存
void Dictionary::initEnDict()
{
    string dictPath = Configuration::getInstance()->getConfigMap()["enDict"];

    ifstream ifs(dictPath);
    if (!ifs.good())
    {
        cout << "ifstream open file" << string(dictPath) << " error !" << endl;
        return;
    }
    string line;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        string word, freq;
        iss >> word >> freq;
        _dict.push_back(pair<string, int>(word, freq == "" ? 0 : (stoi(freq))));
    }
}

//将中文索引读入内存
void Dictionary::initIndex() // key : set<id>
{
    cout << "initialize index" << endl;
    string dictIndexPath = Configuration::getInstance()->getConfigMap()["dicIndex"];

    ifstream ifs(dictIndexPath);
    if (!ifs.good())
    {
        cout << "ifstream open file" << string(dictIndexPath) << " error !" << endl;
        return;
    }
    string line;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        string word, locate;
        iss >> word;
        while (iss >> locate)
        {
            // _index[word].insert(stoi(locate));
            _index[word].insert(locate == "" ? 0 : stoi(locate));
        }
    }

 
}

//将英文索引读入内存
void Dictionary::initEnIndex()
{
    string enDictIndexPath = Configuration::getInstance()->getConfigMap()["enDicIndex"];

    ifstream ifs(enDictIndexPath);
    if (!ifs.good())
    {
        cout << "ifstream open file" << string(enDictIndexPath) << " error !" << endl;
        return;
    }
    string line;
    while (getline(ifs, line))
    {
        istringstream iss(line);
        string word, locate;
        iss >> word;
        while (iss >> locate)
        {
            // _index[word].insert(stoi(locate));
            _index[word].insert(locate == "" ? 0 : stoi(locate));
        }
    }
}

const vector<pair<string, int>> &Dictionary::getDict()
{
    return _dict;
}

const map<string, set<int>> &Dictionary::getIndexTable()
{
    return _index;
}

};
