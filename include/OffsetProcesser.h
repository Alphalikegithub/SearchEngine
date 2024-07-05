#pragma once

#include <iostream>
#include <map>
#include <vector>
#include <utility>
using std::map;
using std::pair;
using std::vector;

namespace wdcpp
{
class WebPage;
/*************************************************************
 *
 *  网页偏移库生成类
 *
 *************************************************************/
class OffsetProcesser
{
public:
    explicit OffsetProcesser(vector<WebPage> &, vector<pair<size_t, size_t>> &); // 参数传入一个网页库
    ~OffsetProcesser()
    {
        //using namespace std;
        //cout << "~OffsetProcesser()" << endl;
    }

    void process(); // 生成偏移库

private:
    vector<WebPage> &_pagelist;
    vector<pair<size_t, size_t>> &_offsetlib;
};
};
