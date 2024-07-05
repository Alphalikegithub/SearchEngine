#pragma once
#include "../3rdparty/simhash-cppjieba/Simhasher.hpp"
using namespace simhash;

#include <vector>
using std::vector;

namespace wdcpp
{
class hashkey
{
    friend class CompareSimhash;

    explicit hashkey(uint64_t i)
    {
        key_A = i & (0xff000000);
        key_B = i & (0x00ff0000);
        key_C = i & (0x0000ff00);
        key_D = i & (0x000000ff);
        A.push_back(i);
        B.push_back(i);
        C.push_back(i);
        D.push_back(i);
    }

    uint64_t key_A;
    uint64_t key_B;
    uint64_t key_C;
    uint64_t key_D;
    vector<uint64_t> A; // 链表A
    vector<uint64_t> B; // 链表B
    vector<uint64_t> C; // 链表C
    vector<uint64_t> D; // 链表D
};

class WebPage;
/*************************************************************
 *
 *  网页比较类（去重）
 *
 *************************************************************/
class CompareSimhash
{
    const string DICT_PATH = "../3rdparty/simhash-cppjieba/dict/jieba.dict.utf8";
    const string MODEL_PATH = "../3rdparty/simhash-cppjieba/dict/hmm_model.utf8";
    const string IDF_PATH = "../3rdparty/simhash-cppjieba/dict/idf.utf8";
    const string STOP_WORDS_PATH = "../3rdparty/simhash-cppjieba/dict/stop_words.utf8";

public:
    CompareSimhash()
        : _simhasher(DICT_PATH, MODEL_PATH, IDF_PATH, STOP_WORDS_PATH) {}
    bool cut(const WebPage &);

private:
    vector<hashkey> keylist;
    const size_t topN = 5;
    Simhasher _simhasher;
};
}; // namespace wdcpp
