#include "../../include/CacheGroup.h"
#include <iostream>

namespace wdcpp
{
CacheGroup::CacheGroup(size_t capacity)
    : _mainCache(capacity),
      _pendingUpdateCache(capacity),
      _onlyRead(false)
{
}

string CacheGroup::getRecord(const string &query)
{
    return _mainCache.getRecord(query);
}

void CacheGroup::insertRecord(const string &query, const string &result)
{
    _mainCache.insertRecord(query, result);
    if (!_onlyRead) // 若可写
        _pendingUpdateCache.insertRecord(query, result);
}

// void CacheGroup::load(const string &path)
// {
// }
// void CacheGroup::dump(const string &path)
// {
// }

void CacheGroup::update(const CacheGroup &group)
{
    _mainCache.update(group._mainCache);
    _pendingUpdateCache.update(group._pendingUpdateCache); // 传进来的 group 的 _pendingUpdateCache 是空的
}
}; // namespace wdcpp
