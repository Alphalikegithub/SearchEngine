#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
using std::string;
using std::unordered_map;
using std::vector;

namespace wdcpp
{
using PageID = long; // 便于全 wdcpp 空间可访问

class RssItem;
class SplitTool;
/*************************************************************
 *
 *  网页类
 *
 *************************************************************/
class WebPage
{
public:
    explicit WebPage(const string &);
    explicit WebPage(const RssItem &);
    WebPage();
    ~WebPage()
    {
        using namespace std;
        // cout << "~WebPage()" << endl;
    }

    string getDoc() const;
    PageID getDocId() const;
    string getTitle() const;
    string getUrl() const;
    string getContent() const;
    string getSummary() const;
    unordered_map<string, int> &getWordsMap();

    void setPageID(PageID);
    void setPageDoc();
    void setPageContent(const string &);
    void setPageSummary(const string &);

    void splitWord(SplitTool &, const vector<string> &);

    void printWordsMap() const;

private:
    string _doc; // <doc>...</doc>
    PageID _docID;
    string _docTitle;
    string _docURL;
    string _docContent;
    string _docSummary;                   // 摘要
    unordered_map<string, int> _wordsMap; // 词频集合 <word, freq>
};
}; // namespace wdcpp
