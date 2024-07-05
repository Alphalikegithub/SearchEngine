#include "../../include/WebPageSearcher.h"
#include "../../include/Configuration.h"
#include "../../include/MyLog.h"
#include "../../include/MultiBytesCharacter.h"
#include "nlohmann/json.hpp"
#include "../../include/fifo_map.hpp"
#include "../../include/ERROR_CHECK.h"
using namespace nlohmann;
/* 以下为 nlohmann/json 库使用，保证插入顺序不变 */
template <class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, A>;
using my_json = basic_json<my_workaround_fifo_map>;
using Json = my_json;

#include <set>
#include <math.h>
using std::multiset;

namespace wdcpp
{
WebPageSearcher::WebPageSearcher()
{
    loadFromFile();
}

/**
 *  从磁盘中读入三个库（网页库，倒排索引库，停用词库）
 *
 *  1. 偏移库无需存入内存，仅在读入网页库时借用该库信息而已
 */
void WebPageSearcher::loadFromFile()
{
    // 读入停用词库
    ifstream stopWordsLib(Configuration::getInstance()->getConfigMap()["stopwords"]);
    if (!stopWordsLib)
    {
        ERROR_PRINT("can not open stop_words.utf8");
        exit(EXIT_FAILURE);
    }
    string word;
    while (getline(stopWordsLib, word))
    {
        _stopWords.push_back(word);
    }

    // 读入网页库
    ifstream offsetLib(Configuration::getInstance()->getConfigMap()["offset"]);
    if (!offsetLib)
    {
        ERROR_PRINT("can not open offset.dat");
        exit(EXIT_FAILURE);
    }
    ifstream ripepageLib(Configuration::getInstance()->getConfigMap()["ripepage"]);
    if (!ripepageLib)
    {
        ERROR_PRINT("can not open ripepage.dat");
        exit(EXIT_FAILURE);
    }
    string offsetLine;
    int docid;
    size_t beg, offset;
    char buf[65535] = {0};
    while (getline(offsetLib, offsetLine))
    {
        stringstream ss(offsetLine);
        ss >> docid >> beg >> offset;
        ripepageLib.read(buf, offset);
        string doc(buf);
        WebPage tmp(doc);
        _pageList.push_back(std::move(tmp));
    }

    // 读入倒排索引库
    ifstream invertIndexLib(Configuration::getInstance()->getConfigMap()["invertIndex"]);
    if (!invertIndexLib)
    {
        ERROR_PRINT("can not open invertIndex.dat");
        exit(EXIT_FAILURE);
    }
    string invertIndexLine;
    string keyWord;
    double weight;
    while (getline(invertIndexLib, invertIndexLine))
    {
        stringstream ss(invertIndexLine);

        ss >> keyWord;
        while (ss)
        {
            ss >> docid >> weight;
            _invertIndexTable[keyWord][docid] = weight;
        }
    }

    stopWordsLib.close();
    offsetLib.close();
    ripepageLib.close();
    invertIndexLib.close();
}

/**
 *  查询网页信息
 *
 *  1. msg 已经序列化后的，由客户发送的查询语句（如：王道在线科技）
 *  2. 返回所有网页信息，并且已经序列化为 json 格式
 */
string WebPageSearcher::doQuery(string msg)
{
    using namespace std;
    cout << "doQuery: " << msg << endl;

    WebPage pageX;
    pageX.setPageContent(msg);               // 将 msg 作为 content 创建网页 pageX
    pageX.splitWord(_splitTool, _stopWords); // 对 pageX 分词并统计词频

    unordered_map<string, double> vecX = getVectorX(pageX); // 获取向量 vecX

    string response;
    set<PageID> IDs = getIDs(pageX); // 获取候选文章的编号集合 IDs（取交集）

    if (IDs.empty())
    {
        LogInfo("\n\twebPageSearcher miss: %s", msg.c_str());
        response = serializeForNoting(); // 获取未找到网页的序列化信息
    }
    else
    {
        vector<PageID> sortedIDs = getSortedIDs(vecX, IDs); // 获取排序后的候选文章的编号集合 sortedIDs

        setSummarys(sortedIDs, pageX); // 为所有候选文章设置摘要信息
    
        response = serialize(sortedIDs); // 获取经过序列化后的所有网页信息
    }

    return response;
}

/**
 *  求网页 pageX 的向量 vexX
 */
unordered_map<string, double> WebPageSearcher::getVectorX(WebPage &pageX)
{
    unordered_map<string, double> vecX;                          // 向量 X
    unordered_map<string, int> &wordsMapX = pageX.getWordsMap(); // <word, freq>

    double sumWeight = 0.0; // pageX 中所有单词的权值平方和

    for (auto &wordPair : wordsMapX) // pair<string, int> wordPair
    {
        double TF = (double)wordPair.second / wordsMapX.size();
        int DF = _invertIndexTable[wordPair.first].size() + 1;
        int N = _pageList.size() + 1;
        double IDF = 0.0;
        if (N != DF)
            IDF = log10((double)N / (DF + 1));
        double w = TF * IDF;
        sumWeight += w * w; // 更新 sumWeight

        vecX.insert({wordPair.first, w}); // <word, w>
    }

    // if (sumWeight == 0.0)
    // {
    //     ERROR_PRINT("pageX's sumWeight equal 0.0\n");
    //     exit(EXIT_FAILURE);
    // }

    for (auto &wordPair : vecX) // pair<string, double> wordPair
    {
        if (sumWeight == 0)
            wordPair.second = 0;
        wordPair.second /= sqrt(sumWeight); // <word, w'>
    }

    return vecX;
}

/**
 *  获取候选文章的编号集合
 */
set<PageID> WebPageSearcher::getIDs(WebPage &pageX)
{
    unordered_map<string, int> &wordsMapX = pageX.getWordsMap(); // <word, freq>

    vector<set<PageID>> IDsArr;      // 存放所有单词所在网页 ID 的集合
    for (auto &wordPair : wordsMapX) // pair<string, int> wordPair
    {
        set<PageID> IDs;                                                 // 存放 wordPair.first 所在网页 ID 的集合
        for (auto &invertInvertPair : _invertIndexTable[wordPair.first]) // pair<PageID, double> invertInvertPair
        {
            IDs.insert(invertInvertPair.first);
        }
        IDsArr.push_back(std::move(IDs));
    }

    set<PageID> IDs;
    for (auto &item : IDsArr) // unordered_set<PageID> item
    {
        if (IDs.empty())
            IDs = item;
        else
        {
            set<PageID> tmp;
            set_intersection(item.begin(), item.end(), IDs.begin(), IDs.end(), inserter(tmp, tmp.begin())); // 这里第五个参数不能就设置为 IDs
            swap(IDs, tmp);
        }
    }

    return IDs;
}

/**
 *  根据余弦相似度算法，对候选文章的 ID 集合进行排序
 *
 *  1. vecX 为向量 X
 *  2. IDs 为候选文章的 ID 集合
 */
struct MyGreater
{
    bool operator()(const pair<double, PageID> &lhs, const pair<double, PageID> &rhs) const
    {
        if (lhs.first != rhs.first)
            return lhs.first > rhs.first;
        else
            return lhs.second < rhs.second;
    }
};
vector<PageID> WebPageSearcher::getSortedIDs(const unordered_map<string, double> &vecX, const set<PageID> &IDs)
{
    multiset<pair<double, PageID>, MyGreater> sortCos;

    for (auto &id : IDs)
    {
        unordered_map<string, double> vecY; // 向量 Yid
        for (auto it = vecX.begin(); it != vecX.end(); ++it)
        {
            double w = _invertIndexTable[it->first][id];
            vecY.insert(std::make_pair(it->first, w));
        }

        double innerProduct = 0, lengthXAbs = 0, lengthYAbs = 0;
        for (auto it = vecX.begin(); it != vecX.end(); ++it)
        {
            innerProduct += it->second * vecY[it->first];
            lengthXAbs += it->second * it->second;
            lengthYAbs += vecY[it->first] * vecY[it->first];
        }

        double Cos = innerProduct / ((sqrt(lengthXAbs) * sqrt(lengthYAbs)));
        sortCos.insert(std::make_pair(Cos, id));
    }

    vector<PageID> result;
    for (auto &id : sortCos)
    {
        result.push_back(id.second);
    }
    return result;
}

/**
 *  设置所有候选网页的摘要信息
 */
void WebPageSearcher::setSummarys(vector<PageID> &sortedIDs, WebPage &pageX)
{
    const size_t STEP = 40;                                      // 目标字符待往左/右偏移的字符数
    unordered_map<string, int> &wordsMapX = pageX.getWordsMap(); // <word, freq>
    vector<PageID> tmpIDs = sortedIDs;

    for (auto &ID : tmpIDs) // 依次设置所有网页的摘要
    {
        static int n = 0;
        ++n;
        if (n == 1548)
        {
            cout << "got it!" << endl;
        }

        const string content = _pageList[ID].getContent();

        size_t first_pos = SIZE_MAX;     // page 中第一次出现 wordsMapX 中的单词的位置
        for (auto &wordPair : wordsMapX) // pair<string, int> wordPair
        {
            string word = wordPair.first;
            size_t pos = content.find(word);
            // if (pos != string::npos && pos < first_pos)
            if (pos != content.npos && pos < first_pos)
                first_pos = pos;
        }

        if (first_pos == SIZE_MAX) // 这篇文章中的 content 部分，没有 wordsMapX 中的单词，而在 title 部分有 wordsMapX 中的单词。此时该篇文章应被剔除
        {
            remove(sortedIDs.begin(), sortedIDs.end(), ID);
            continue;
        }

        size_t first_to_end = content.substr(first_pos).size();                                             // 从 content[first_pos] 到字符串末尾所占字节数
        size_t right_pos = first_pos + howManyBytesWithNCharacter(&content[first_pos], first_to_end, STEP); // 从 content[first_pos] 到其后 STEP 个字符所占字节数（first_to_end 为上限）

        vector<size_t> ppc = getPosPerCharactor(content, first_pos);       // 从字符串前 first_pos 字节中所有字符的起始字节
        size_t left_pos = ppc.size() >= STEP ? ppc[ppc.size() - STEP] : 0; 
        // size_t left_pos = ppc.size() >= STEP ? ppc[ppc.size() - STEP] : ppc[0]; // ppc[0] 一定是 0

        string summary = "";
        if (left_pos != 0) // content[left_pos] 前还有字符
            summary += " ... ";
        summary += content.substr(left_pos, right_pos - left_pos);
        if (right_pos < content.size()) // content[right_pos] 后还有字符
            summary += " ... ";
        // summary += content.substr(first_pos);
        _pageList[ID].setPageSummary(summary);
    }
}

/**
 *  返回搜索失败后的 json 格式字符串
 */
string WebPageSearcher::serializeForNoting()
{
    Json root;
    root["msgID"] = 404;
    root["msg"] = json("未能找到相关文章");
    // root["msg"] = "未能找到相关文章"; // 一样的效果

    return root.dump(4);
}

/**
 *  返回使用 json 序列化后的所有网页信息
 */
string WebPageSearcher::serialize(const vector<PageID> &sortedIDs)
{
    const PageID N = stol(Configuration::getInstance()->getConfigMap()["maxpagenum"]);

    Json root;
    root["msgID"] = 200;

    Json msg;
    PageID cnt = 0;
    for (auto &ID : sortedIDs)
    {
        if (++cnt > N) // 第 N+1 次进入循环体则退出（保证只发送前 N 篇网页）
            break;
        WebPage &page = _pageList[ID];

        Json file;
        file["title"] = page.getTitle();
        file["url"] = page.getUrl();
        file["summary"] = page.getSummary();
        msg.push_back(file);
    }
    root["msg"] = msg;

    return root.dump(4);
}

}; // namespace wdcpp
