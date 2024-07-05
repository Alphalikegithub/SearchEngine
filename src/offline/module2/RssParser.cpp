#include "../../../include/RssParser.h"

namespace wdcpp
{
RssPraser::RssPraser(const char *filePath)
{
    prase(filePath);
}

void RssPraser::prase(const char *filename)
{
    cout << "RssPraser is prasing " << filename << endl;

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
    if (!home.check()) // 检查 item，若返回 false 则将其保留，否则忽略
        _rss.push_back(home);

    XMLElement *item = channel->FirstChildElement("item");
    while (item)
    {
        RssItem page; // 存放 item 的内容
        page.title = item->FirstChildElement("title")->GetText();
        //cout << page.title << endl;
        page.link = item->FirstChildElement("link")->GetText();
        //cout << page.link << endl;
        page.description = item->FirstChildElement("description")->GetText();
        // page.content = item->FirstChildElement("content")->GetText();  
        // 析出标签
        page.description = dissolve(page.description);
        // page.content = dissolve(page.content);

        if (!page.check())        // 检查 item，若返回 false 则将其保留，否则忽略
            _rss.push_back(page); // 插入 _rss

        // 获取下一个 item
        item = item->NextSiblingElement("item");
    }
    //cout << 555555 << endl;
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

vector<RssItem> &RssPraser::getRssItems()
{
    return _rss;
}

// vector<string> RssPraser::getResultWithString()
// {
//     vector<string> result;
//     for (auto item = _rss.begin() + 1; item != _rss.end(); ++item) // xml 文件中包含的首页信息不予返回
//     {
//         result.push_back(item->title + "\n" + item->link + "\n" + item->description);
//     }
//     return result;
// }

string RssPraser::dissolve(string text)
{
    string res = text;
    {
        regex pattern("<(.[^>]*)>");
        res = regex_replace(res, pattern, "");
        //cout << 1111111 << endl;
    }
    //{
    //可以匹配以showPlayer({开头，以});结尾，并且中间可以有任意字符的字符串。std::regex::egrep是一个标志，表示使用扩展的grep语法。
        //regex pattern1("showPlayer\\(\\{.*\\}\\);", std::regex::egrep);
        //res = regex_replace(res, pattern1, "");
       // cout << 2222222 << endl;
    //}
    {   
        //可以匹配HTML中表示空格的字符。
        regex pattern("&nbsp;");
        res = regex_replace(res, pattern, "");
        //cout << 3333333 << endl;
    }
    return res;
}
}; // namespace wdcpp
