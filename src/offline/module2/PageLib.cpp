#include "../../../include/PageLib.h"
#include "../../../include/Configuration.h"
#include <fstream>
using std::ofstream;

namespace wdcpp
{
PageLib::PageLib(const string &dirPath)
    :_dirScanner(dirPath),
     _pageProcesser(_dirScanner.getFilePathList(), _pageList),
     _invertIndexProcesser(_pageList, _invertIndexTable),
     _offsetProcesser(_pageList, _offsetTable)
{
}

void PageLib::create()
{
    _pageProcesser.process(); //生成网页库

    _invertIndexProcesser.process(); //生成倒排索引库

    _offsetProcesser.process(); //生成偏移库
}

void PageLib::store()
{
    string ripepagePath = Configuration::getInstance()->getConfigMap()["ripepage"];
    string offsetPath = Configuration::getInstance()->getConfigMap()["offset"];
    string InvertIndex = Configuration::getInstance()->getConfigMap()["invertIndex"];

    using namespace std;

    //写网页库
    ofstream ofs1(ripepagePath);
    if(!ofs1)
    {
        cout << "can not open ripepage.dat" << endl;
        exit(EXIT_FAILURE);
    }
    PageID ID = 0;
    for(auto &page : _pageList)
    {
        page.setPageID(ID++);
        page.setPageDoc();
        ofs1 << page.getDoc();
    }
    ofs1.close();

    //写倒排索引库
    ofstream ofs2(InvertIndex);
    if(!ofs2)
    {
        cout << "can not open InvertIndex.dat" << endl;
        exit(EXIT_FAILURE);
    }
    for(auto &wordPair : _invertIndexTable)
    {
        ofs2 << wordPair.first << " ";
        for(auto &pagePair : wordPair.second)
        {
            ofs2 << pagePair.first << " "
                 << pagePair.second << " ";
        }
        ofs2 << "\n";
    }
    ofs2.close();

    //写偏移库
    ofstream ofs3(offsetPath);
    if(!ofs3)
    {
        cout << "can not open offset.dat" << endl;
        exit(EXIT_FAILURE);
    }
    for(size_t idx = 0; idx < _offsetTable.size(); ++idx)
    {
        ofs3 << _pageList[idx].getDocId() << " "
             << _offsetTable[idx].first << " "
             << _offsetTable[idx].second << " "
             << "\n";
    }
    ofs3.close();

    cout << "store succeed!" << endl;
}
}
