#pragma once
#include "CompareSimhash.h"
#include "SplitTool.h"

#include <iostream>
#include <vector>
#include <string>
using std::string;
using std::vector;

namespace wdcpp
{
class WebPage;
/*************************************************************
 *
 *  网页库生成类
 *
 *************************************************************/
class PageProcesser
{
public:
    explicit PageProcesser(vector<string> &, vector<WebPage> &);
    ~PageProcesser()
    {
        //using namespace std;
        //cout << "~PageProcesser()" << endl;
    }

    void process();

    void printPageList();
    void printStopWords();

private:
    void loadStopWords();

    void loadPageFromXML();  // 加载网页
    void cutRedundantPage(); // 网页去重
    void countFrequence();   // 统计词频

private:
    vector<string> &_filePathList;
    vector<WebPage> &_nonRepetivepageList;
    vector<WebPage> _pageList;
    vector<string> _stopWords;
    // vector<bool> _isDelete;
    CompareSimhash _comparePages; // 网页比较器
    SplitTool _splitTool;         // 分词器
};
}; // namespace wdcpp
