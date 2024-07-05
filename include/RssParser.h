#pragma once
#include "tinyxml2.h"
using namespace tinyxml2;

#include <string>
#include <vector>
#include <regex>
#include <iostream>
#include <fstream>
using std::cin;
using std::cout;
using std::endl;
using std::ofstream;
using std::regex;
using std::string;
using std::vector;

namespace wdcpp
{
struct RssItem // 存放一个item的内容
{
    string title;
    string link;
    string description;
    string content;

    /**
     *  返回 true 则表示该 item 无效
     */
    bool check()
    {
        if (title == "" || link == "" || description == "") // 存在空字段，不保留该 item
            return true;
        if (title == description) // titile 等于 description 不保留该 item
            return true;
        if (description.size() <= title.size()) // titile 长于 description 不保留该 item
            return true;
        for (auto &s : description)
        {
            if (s != ' ' || s != '\n' || s != '\t')
                return false; // 只要 description 中有非特殊字符，则保留该 item
        }
        return true;
    }
};

class RssPraser // 解析器
{
public:
    explicit RssPraser(const char *);

    void dump(const string &); // 转存到指定文件
    vector<RssItem> &getRssItems();
    // vector<string> getResultWithString(); // 以字符串的格式返回解析结果

private:
    void prase(const char *); // 解析
    string dissolve(string);  // 析出标签

private:
    vector<RssItem> _rss; // 存放解析后的所有内容（包括首页）
};
}; // namespace wdcpp
