#pragma once

#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <map>
#include <utility>
using std::map;
using std::pair;
using std::string;
using std::unordered_map;
using std::vector;

namespace wdcpp
{
class WebPage;
using PageID = long;
/*************************************************************
 *
 *  倒排索引库生成类
 *
 *************************************************************/
class InvertIndexProcesser
{
public:
    explicit InvertIndexProcesser(vector<WebPage> &, unordered_map<string, unordered_map<PageID, double>> &);
    ~InvertIndexProcesser()
    {
        using namespace std;
        //cout << "~InvertIndexProcesser()" << endl;
    }

    void process();

    void printInvertIndexTable();

private:
    vector<WebPage> &_pageList;
    unordered_map<string, unordered_map<PageID, double>> &_invertIndexTable;
    vector<double> _sumOfWeightsPerPage; // 每篇文章中所有单词的 w 的平方和
};
}; // namespace wdcpp
