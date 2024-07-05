#pragma once
#include <iostream>
#include <string>
#include <vector>

using std::vector;
using std::string;

namespace wdcpp
{
/**
 *  获取多字节字符所占字节数
 *
 *  1. ch 为多字节字符的第一个字节
 *  2. 返回该字符所占字节数
 */
inline size_t nBytesCode(const char ch)
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

/**
 *  获取从 p 开始往后 N 个字符的字符串所占字节数
 *
 *  1. limit 是能返回的最大字节数，若所求超过 limit 则直接返回 limit
 */
size_t howManyBytesWithNCharacter(const char *p, size_t limit, size_t N)
{
    size_t totalChar = 0;
    for (size_t i = 0; i < N; ++i)
    {
        int nBytes = nBytesCode(p[0]);
        p += nBytes;
        if (totalChar + nBytes > limit)
            return limit;
        totalChar += nBytes;
    }
    return totalChar;
}

/**
 *  获取 str 前 end 个字节中，所有字符的起始字节
 *
 *  1. 第 end 个字节一定是某个字符的起始字节
 */
vector<size_t> getPosPerCharactor(const string &str, size_t end)
{
    vector<size_t> res;
    int totalChar = 0;
    const char *p = str.c_str();
    for (size_t idx = 0; idx < end; ++idx)
    {
        int nBytes = nBytesCode(p[0]);
        p += nBytes;
        res.push_back(totalChar);
        totalChar += nBytes;
        idx += (nBytes - 1);
    }
    return res;
}

}; // namespace wdcpp
