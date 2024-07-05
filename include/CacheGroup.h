#pragma once
#include "LRUCache.h"

namespace wdcpp
{
class CacheManager;
/*************************************************************
 *
 *  cache group 类（包含两块 cache）
 *
 *************************************************************/
class CacheGroup
{
    friend class CacheManager;

public:
    CacheGroup(size_t);

    string getRecord(const string &);
    void insertRecord(const string &, const string &);
    void load(const string &);
    void dump(const string &);
    void update(const CacheGroup &);

private:
    LRUCache _mainCache;          // 主 cache
    LRUCache _pendingUpdateCache; // 更新 cache（存放自上次更新以来新的记录，用于更新操作）
    bool _onlyRead;               // 只读标志（若只读则新记录不能插入到两 cache 中）
};
}; // namespace wdcpp
