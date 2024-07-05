#pragma once
#include "WebPage.h"
#include "SplitTool.h"

#include <unordered_map>
using std::unordered_map;

namespace wdcpp
{
/*************************************************************
 *
 *
 *
 *************************************************************/
class WebPageSearcher
{
public:
    WebPageSearcher();
    ~WebPageSearcher() = default;

    string doQuery(string);

private:
    void loadFromFile();

    unordered_map<string, double> getVectorX(WebPage &);
    set<PageID> getIDs(WebPage &);
    vector<PageID> getSortedIDs(const unordered_map<string, double> &, const set<PageID> &);

    void setSummarys(vector<PageID> &, WebPage &);

    string serializeForNoting();
    string serialize(const vector<PageID> &);

private:
    vector<WebPage> _pageList;
    // unordered_map<int, pair<int, int>> _offsetTable; // 感觉不需要一直存放在内存
    unordered_map<string, unordered_map<PageID, double>> _invertIndexTable;

    SplitTool _splitTool;
    vector<string> _stopWords;
};
}; // namespace wdcpp
