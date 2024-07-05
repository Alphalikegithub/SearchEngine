#include "../../../include/CompareSimhash.h"
#include "../../../include/WebPage.h"

namespace wdcpp
{
/**
 *	若剔除网页 page 则返回 true，否则返回 false
 *
 *  1. 输入 page 的引用
 *  2. 求得 page 的 hash 值
 *  3. 判断是否需要剔除该网页
 */
bool CompareSimhash::cut(const WebPage &page)
{
    uint64_t i = 0;
    _simhasher.make(page.getContent(), topN, i); // 求 page 的 64 位 hash 值

    uint64_t a = 0xff000000;
    uint64_t b = 0x00ff0000;
    uint64_t c = 0x0000ff00;
    uint64_t d = 0x000000ff;

    for (auto &list : keylist) // 依次遍历所有 list
    {
        if ((i & a) == list.key_A) // 命中 key_A
        {
            for (auto &hash : list.A) // 在 list.A 中查找
            {
                if (Simhasher::isEqual(i, hash))
                {
                    return true;
                }
            }
            list.A.push_back(i); // 当 i 未在 list.A 中命中，要将 i 加入 list.A
        }
        if ((i & b) == list.key_B) // 命中 key_B
        {
            for (auto &hash : list.B) // 在 list.B 中查找
            {
                if (Simhasher::isEqual(i, hash))
                {
                    return true;
                }
            }
            list.B.push_back(i); // 当 i 未在 list.B 中命中，要将 i 加入 list.B
        }
        if ((i & c) == list.key_C) // 命中 key_C
        {
            for (auto &hash : list.C) // 在 list.C 中查找
            {
                if (Simhasher::isEqual(i, hash))
                {
                    return true;
                }
            }
            list.C.push_back(i); // 当 i 未在 list.C 中命中，要将 i 加入 list.C
        }
        if ((i & d) == list.key_D) // 命中 key_D
        {
            for (auto &hash : list.D) // 在 list.D 中查找
            {
                if (Simhasher::isEqual(i, hash))
                {
                    return true;
                }
            }
            list.D.push_back(i); // 当 i 未在 list.D 中命中，要将 i 加入 list.d
        }
    }

    // 所有 list 的 Key 都未命中，将该 hash 值添加进 keylist 中作为新的 list
    hashkey h(i);
    keylist.push_back(h);
    return false;
}
}; // namespace wdcpp
