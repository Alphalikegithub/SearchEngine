#pragma once

#include <string>
#include <unordered_map>
#include <list>
#include <utility>
using std::list;
using std::pair;
using std::string;
using std::unordered_map;

namespace wdcpp
{
class CacheManager;
/*************************************************************
 *
 *  cache 类
 *
 *************************************************************/
class LRUCache
{
    friend class CacheManager;

    using Record = pair<string, string>;
    using iterator = list<Record>::iterator;

public:
    LRUCache(size_t);

    bool isHit(const string &);

    string getRecord(const string &);
    void insertRecord(const string &, const string &);
    void load(const string &);
    void dump(const string &);
    void update(const LRUCache &);
    void clear();
    size_t size() const;

private:
    list<Record> _resultList;
    unordered_map<string, iterator> _hashMap;
    size_t _capacity;
};
}; // namespace wdcpp
