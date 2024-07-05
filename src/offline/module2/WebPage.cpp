#include "../../../include/WebPage.h"
#include "../../../include/RssParser.h"
#include "../../../include/SplitTool.h"

#include <sstream>
using std::stringstream;

namespace wdcpp
{
/**
 *  根据 Item 对象构建网页
 *
 *  1. 构建网页库时调用
 */
WebPage::WebPage(const RssItem &item)
    : _docID(0)
{
    _docTitle = item.title;
    _docURL = item.link;
    _docContent = item.description;

    // stringstream ss(str);
    // getline(ss, _docTitle);
    // getline(ss, _docURL);
    // getline(ss, _docContent);
}

/**
 *  根据 <doc>...</doc> 字符串构建网页
 *
 *  1. 加载网页库时调用
 */
WebPage::WebPage(const string &doc)
    : _doc(doc)
{
    size_t beg = 0, end = 0;

    beg = doc.find("<docid>"); // 注意：<> 前后有一个空格
    end = doc.find("</docid>");
    istringstream tmp(doc.substr(beg + 8, end - beg - 7));
    tmp >> _docID;

    beg = doc.find("<title>");
    end = doc.find("</title>");
    _docTitle = doc.substr(beg + 8, end - beg - 7);

    beg = doc.find("<url>");
    end = doc.find("</url>");
    _docURL = doc.substr(beg + 6, end - beg - 5);

    beg = doc.find("<content>");
    end = doc.find("</content>");
    _docContent = doc.substr(beg + 10, end - beg - 9);

    // regex reg("<[^>]*>");
    // string result = regex_replace(doc, reg, "");

    // istringstream iss(result);
    // iss >> _docID >> _docTitle >> _docURL;
    // string linefeed;
    // getline(iss, linefeed);    // 读取 URL 所在行的换行符
    // getline(iss, _docContent); // 这里会读入 Content 所在行，行首的 \t
}

WebPage::WebPage()
    : _docID(0)
{
}

string WebPage::getDoc() const
{
    return _doc;
}

PageID WebPage::getDocId() const
{
    return _docID;
}

string WebPage::getTitle() const
{
    return _docTitle;
}

string WebPage::getUrl() const
{
    return _docURL;
}

string WebPage::getContent() const
{
    return _docContent;
}

string WebPage::getSummary() const
{
    return _docSummary;
}

unordered_map<string, int> &WebPage::getWordsMap()
{
    return _wordsMap;
}

void WebPage::setPageID(PageID ID)
{
    _docID = ID;
}

void WebPage::setPageDoc()
{
    _doc = "<doc>\n\t<docid> " + to_string(_docID) +
           " </docid>\n\t<title> " + _docTitle +
           " </title>\n\t<url> " + _docURL +
           " </url>\n\t<content> " + _docContent +
           " </content>\n</doc>\n";
}

void WebPage::setPageContent(const string &content)
{
    _docContent = content;
}

void WebPage::setPageSummary(const string &summary)
{
    _docSummary = summary;
}

/**
 *  对 _docTitle 和 _docContent 分词并统计词频
 */
void WebPage::splitWord(SplitTool &tool, const vector<string> &stopWords)
{
    auto words = tool.cut(_docTitle + _docContent); // 分词
    for (auto &word : words)                        // 去重并统计词频
    {
        if (word != " " && find(stopWords.begin(), stopWords.end(), word) == stopWords.end()) // 若不是停用词就加入 _wordsMap
            ++_wordsMap[word];
    }

    // printWordsMap();
}

void WebPage::printWordsMap() const
{
    using namespace std;
    cout << "words num of No." << _docID << " = " << _wordsMap.size() << endl;
    for (auto &pair : _wordsMap)
    {
        cout << pair.first << " " << pair.second << endl;
    }
}
}; // namespace wdcpp
