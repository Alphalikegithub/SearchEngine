#include "tinyxml2.h"
#include <iostream>
#include <string.h>
#include <vector>
#include <fstream>
#include <regex>
using namespace std;
using namespace tinyxml2;

/*
    1. 看源码
    2. 看用到的函数
    3. 包括regex
*/

struct RssItem // 存放一个item的内容
{
    string title;
    string link;
    string description;
    string content;
};

class RssPraser // 解析器
{
public:
    void prase(const char *);  // 解析
    void dump(const string &); // 转存

private:
    string dissolve(string); // 析出标签

private:
    vector<RssItem> _rss; // 存放解析后的所有内容（包括首页）
};

void RssPraser::prase(const char *filename)
{
    XMLDocument doc;
    if (doc.LoadFile(filename))
    {
        doc.PrintError();
        exit(EXIT_FAILURE);
    }

    XMLElement *channel = doc.FirstChildElement("rss")->FirstChildElement("channel");
    RssItem home; // 首页内容
    home.title = channel->FirstChildElement("title")->GetText();
    home.link = channel->FirstChildElement("link")->GetText();
    home.description = channel->FirstChildElement("description")->GetText();
    _rss.push_back(home);

    XMLElement *item = channel->FirstChildElement("item");
    while (item)
    {
        RssItem page; // item内容
        page.title = item->FirstChildElement("title")->GetText();
        page.link = item->FirstChildElement("link")->GetText();
        page.description = item->FirstChildElement("description")->GetText();
        page.content = item->FirstChildElement("content:encoded")->GetText();

        // 析出标签
        page.description = dissolve(page.description);
        page.content = dissolve(page.content);

        _rss.push_back(page);

        item = item->NextSiblingElement("item");
    }
}

void RssPraser::dump(const string &filename)
{
    ofstream ofs(filename);
    if (!ofs)
    {
        cout << "open file failed." << endl;
        exit(EXIT_FAILURE);
    }

    for (size_t i = 0; i < _rss.size(); i++)
    {
        ofs << "<doc>" << endl
            << "\t<docid>" << i << "</docid>" << endl
            << "\t<title>" << _rss[i].title << "</title>" << endl
            << "\t<link>" << _rss[i].link << "</link>" << endl
            << "\t<description>" << _rss[i].description << "</description>" << endl
            << "\t<content>" << _rss[i].content << "</content>" << endl
            << "</doc>" << endl
            << endl;
    }
}

string RssPraser::dissolve(string text)
{
    string res = text;
    regex pattern("<(.[^>]*)>");
    res = regex_replace(res, pattern, "");
    return res;
}

int main()
{
    RssPraser pr;
    // pr.prase("codeshell.xml");
    pr.prase("coolshell.xml");
    pr.dump("pagelib.txt");
    return 0;
}