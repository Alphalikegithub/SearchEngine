#pragma once

#include <iostream>
#include <vector>
#include <string>
using std::string;
using std::vector;

namespace wdcpp
{
/*************************************************************
 *
 *  目录扫描类
 *
 *************************************************************/
class DirScanner
{
public:
    explicit DirScanner(const string &);
    ~DirScanner()
    {
        //using namespace std;
        //cout << "~DirScanner()" << endl;
    }

    void traverse();
    vector<string> &getFilePathList();

private:
    void traverse(const string &);

private:
    vector<string> _filePathList; // _dirPath 目录下所有文件的路径
    string _dirPath;
};
}; // namespace wdcpp
