#pragma once
#include "DirScanner.h"
#include "WebPage.h"
#include "PageProcesser.h"
#include "InvertIndexProcesser.h"
#include "OffsetProcesser.h"

namespace wdcpp
{
/*************************************************************
 *
 *  网页库类
 *
 *  1. 包含三个网页库数据
 *  2. 包含三个网页库生成类对象
 *
 *************************************************************/
class PageLib
{
public:
    explicit PageLib(const string &);
    ~PageLib()
    {
        using namespace std;
        //cout << "~PageLib()" << endl;
    }

    void create();
    void store();

private:
    vector<WebPage> _pageList;                                              // 网页库
    vector<pair<size_t, size_t>> _offsetTable;                              // 网页偏移库
    unordered_map<string, unordered_map<PageID, double>> _invertIndexTable; // 倒排索引库
    DirScanner _dirScanner;
    PageProcesser _pageProcesser;
    InvertIndexProcesser _invertIndexProcesser;
    OffsetProcesser _offsetProcesser;
};
}; // namespace wdcpp
