#include "../../include/LRUCache.h"
#include "../../include/Thread.h"

#include <iostream>
#include <fstream>
#include <sstream>
using std::ifstream;
using std::istringstream;
using std::ofstream;

namespace wdcpp
{
extern __thread size_t __thread_id; // 工作线程的编号（0, 1, 2, ... , _workerNum-1）

LRUCache::LRUCache(size_t capacity)
    : _capacity(capacity)
{
}

bool LRUCache::isHit(const string &query)
{
    std::cout << "Entering isHit() function." << std::endl;
    std::cout << "Query is: " << query << std::endl;
    std::cout << "Map size is: " << _hashMap.size() << std::endl;

    bool result = _hashMap.count(query) > 0;
    std::cout << "result: " << result << std::endl;

    std::cout << "Leaving isHit() function." << std::endl;
    return _hashMap.count(query) > 0;
}

string LRUCache::getRecord(const string &query)
{
    std::cout << 3333333333 << std::endl;
    if (!isHit(query))
        return "";
    std::cout << 4444444444 << std::endl;
    std::cout << "No." << __thread_id << " cache hit!" << std::endl;
    // 将 query 的记录移至 _resultList 头部
    _resultList.splice(_resultList.begin(), _resultList, _hashMap[query]);
    return _hashMap[query]->second;
}

void LRUCache::insertRecord(const string &query, const string &result)
{
    if (isHit(query))
    {
        // 将 query 的记录移至 _resultList 头部
        _resultList.splice(_resultList.begin(), _resultList, _hashMap[query]);
    }
    else
    {
        _resultList.push_front({query, result}); // 将记录 <query, json> 插入 _resultList 头部
        _hashMap[query] = _resultList.begin();   // 更新 _hashMap
        if (_resultList.size() > _capacity)      // _resultList 已满
        {
            auto back_key = _resultList.back().first; // 暂存尾部记录的 query
            _resultList.pop_back();                   // 删除 _resultList 尾部记录
            _hashMap.erase(back_key);                 // 更新 _hashMap
        }
    }
}

// void LRUCache::load(const string &path)
// {
//     ifstream ifs(path);
//     if (!ifs)
//     {
//         std::cout << "LRU cache file open failed" << std::endl;
//     }

//     string line;
//     while (getline(ifs, line))
//     {
//         istringstream iss(line);
//         string key, value;
//         iss >> key >> value;
//         insertRecord(key, value);
//     }

//     ifs.close();
// }

// void LRUCache::dump(const string &path)
// {
//     ofstream ofs(path);
//     if (!ofs)
//     {
//         std::cout << "LRU cache file open failed" << std::endl;
//     }

//     auto it = _resultList.rbegin();
//     int count = 0;
//     for (; it != _resultList.rend(); ++it)
//     {
//         ofs << it->first << " " << it->second << std::endl;
//     }

//     ofs.close();
// }

void LRUCache::clear()
{
    _resultList.clear();
    _hashMap.clear();
}

void LRUCache::update(const LRUCache &cache)
{
    for (auto it = cache._resultList.rbegin(); it != cache._resultList.rend(); ++it)
    {
        insertRecord(it->first, it->second);
    }
}

size_t LRUCache::size() const
{
    return _resultList.size();
}
}; // namespace wdcpp
