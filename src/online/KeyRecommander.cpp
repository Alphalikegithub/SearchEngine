#include "../../include/KeyRecommander.h"
#include "../../include/Configuration.h"
#include "../../include/MyLog.h"
#include "nlohmann/json.hpp"
#include "../../include/fifo_map.hpp"
using namespace nlohmann;
/* 以下为 nlohmann/json 库使用，保证插入顺序不变 */
template <class K, class V, class dummy_compare, class A>
using my_workaround_fifo_map = fifo_map<K, V, fifo_map_compare<K>, A>;
using my_json = basic_json<my_workaround_fifo_map>;
using Json = my_json;

#include <algorithm>
#include <iostream>
using std::cout;
using std::endl;
using namespace wdcpp;

string KeyRecommander::doQuery(const string &queWord)
{
    Dictionary *pdict = Dictionary::getInstance();
    const map<string, set<int>> &indexTable = pdict->getIndexTable(); // 词典索引

    set<int> indexId; //获得的索引ID
    string index;
    for (size_t idx = 0; idx < queWord.length(); ++idx)
    {
        size_t nBytes = nBytesCode(queWord[idx]);
        index = queWord.substr(idx, nBytes);
        idx += (nBytes - 1);
        auto it = indexTable.find(index);
        if (it != indexTable.end())
        {
            indexId.insert(it->second.begin(), it->second.end());
        }
    }

    if (indexId.size() == 0)
    {
        LogInfo("\n\tkeyRecommender miss: %s", queWord.c_str());
        return serializeForNoting();
    }

    priority_queue<MyResult, vector<MyResult>, MyCompare> resultQue;
    statistic(queWord, indexId, resultQue);

    vector<string> result;
    const int candicateCount = stoi(Configuration::getInstance()->getConfigMap()["maxkeynum"]);
    for (int i = 0; i < candicateCount; ++i)
    {
        if (!resultQue.empty())
        {
#ifdef __DEBUG__
            cout << resultQue.top().getWord() << endl;
#endif
            result.push_back(resultQue.top().getWord());
            resultQue.pop();
        }
    }

    return serialize(result);
}

void KeyRecommander::statistic(const string &queWord,
                               set<int> &indexId,
                               priority_queue<MyResult, vector<MyResult>, MyCompare> &resultQue)
{
    Dictionary *pdict = Dictionary::getInstance();
    const vector<pair<string, int>> &dict = pdict->getDict(); // 词典

    for (auto it = indexId.begin(); it != indexId.end(); ++it)
    {
        MyResult result(dict[*it].first, dict[*it].second);
        int dist = distance(queWord, dict[*it].first);
        result.setDist(dist);
        resultQue.push(result);
    }
}
size_t KeyRecommander::nBytesCode(const char ch)
{
    if (ch & (1 << 7))
    {
        int nBytes = 1;
        for (int idx = 0; idx != 6; ++idx)
        {
            if (ch & (1 << (6 - idx)))
            {
                ++nBytes;
            }
            else
                break;
        }
        return nBytes;
    }
    return 1;
}
std::size_t KeyRecommander::length(const std::string &str)
{
    std::size_t ilen = 0;
    for (std::size_t idx = 0; idx != str.size(); ++idx)
    {
        int nBytes = nBytesCode(str[idx]);
        idx += (nBytes - 1);
        ++ilen;
    }
    return ilen;
}

int KeyRecommander::triple_min(const int &a, const int &b, const int &c)
{
    return a < b ? (a < c ? a : c) : (b < c ? b : c);
}

int KeyRecommander::distance(const string &lhs, const string &rhs)
{
    if (lhs == rhs) //相等返回0
        return 0;

    //计算最小编辑距离-包括处理中英文
    size_t lhs_len = length(lhs);
    size_t rhs_len = length(rhs);
    int editDist[lhs_len + 1][rhs_len + 1];
    for (size_t idx = 0; idx <= lhs_len; ++idx)
    {
        editDist[idx][0] = idx;
    }

    for (size_t idx = 0; idx <= rhs_len; ++idx)
    {
        editDist[0][idx] = idx;
    }

    string sublhs, subrhs;

    for (std::size_t dist_i = 1, lhs_idx = 0; dist_i <= lhs_len; ++dist_i, ++lhs_idx)
    {
        size_t nBytes = nBytesCode(lhs[lhs_idx]);
        sublhs = lhs.substr(lhs_idx, nBytes);
        lhs_idx += (nBytes - 1);

        for (std::size_t dist_j = 1, rhs_idx = 0; dist_j <= rhs_len; ++dist_j, ++rhs_idx)
        {
            nBytes = nBytesCode(rhs[rhs_idx]);
            subrhs = rhs.substr(rhs_idx, nBytes);
            rhs_idx += (nBytes - 1);
            if (sublhs == subrhs)
            {
                editDist[dist_i][dist_j] = editDist[dist_i - 1][dist_j - 1];
            }
            else
            {
                editDist[dist_i][dist_j] = triple_min(
                    editDist[dist_i][dist_j - 1] + 1,
                    editDist[dist_i - 1][dist_j] + 1,
                    editDist[dist_i - 1][dist_j - 1] + 1);
            }
        }
    }
    return editDist[lhs_len][rhs_len];
}

/**
 *  未找到结果，返回 404 序列化结果
 */
string KeyRecommander::serializeForNoting()
{
    Json root;
    root["msgID"] = 404;
    root["msg"] = json("未能找到相关关键词");
    // root["msg"] = "未能找到相关关键词"; // 一样的效果

    return root.dump(4);
}

/**
 *  找到结果，返回所有关键词的序列化结果
 */
string KeyRecommander::serialize(const vector<string> &result)
{
    Json root;
    root["msgID"] = 100;

    Json msg = result;
    root["suggestion"] = msg;

#ifdef __DEBUG__
    printf("\t(File:%s, Func:%s(), Line:%d)\n", __FILE__, __FUNCTION__, __LINE__);
    cout << root.dump(4) << endl;
#endif

    return root.dump(4);
}