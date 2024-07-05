#pragma once
#include <string>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>

using std::string;
using std::vector;
using std::pair;
using std::set;
using std::unordered_set;
using std::unordered_map;
using std::map;

namespace wdcpp
{

class SplitTool;

class DictProducer
{
public:
    DictProducer(const string &dir);
    DictProducer(const string &dir, SplitTool *splitTool);

    void buildEnDict();
    void buildCnDict();
    void buildIndex();
    void storeDict(const char * filepath);
    void storeIndex(const char *filepath);

private:
    void getFiles(string dir);//获取文件绝对路径
    size_t getByteNum_UTF8(const char byte);
    void showFiles() const;
    void showDict() const;
    void loadStopWord(string stopDictPath); 

private:
    vector<string> _files;
    vector<string> _enfiles;
    vector<pair<string, int>> _dict;//用来读
    unordered_map<string, int> _dict2;//用来写

    SplitTool * _splitTool;
    map<string, set<int>> _index;
    unordered_set<string> _stopWord;
    string _dir;
    string _endir;
};

};