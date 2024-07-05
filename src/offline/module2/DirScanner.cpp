#include "../../../include/DirScanner.h"
#include "../../../include/ERROR_CHECK.h"

#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <iostream>

namespace wdcpp
{
DirScanner::DirScanner(const string &dirPath)
    : _dirPath(dirPath)
{
    // std::cout << "DirScanner()\n";
    traverse();
    //for (auto &item : _filePathList)
    //{
    //   std::cout << item << "\n";
    //}
}

/**
 *  打开 _dirPath 目录，递归获取其中所有文件的路径信息
 */
void DirScanner::traverse()
{
    // DIR *pDir = opendir(_dirPath.c_str());
    // ERROR_CHECK(pDir, nullptr, "opendir");

    // struct dirent *pDirent;
    // while ((pDirent = readdir(pDir)) != nullptr)
    // {
    //     if ((strcmp(pDirent->d_name, ".") == 0) || (strcmp(pDirent->d_name, "..") == 0))
    //         continue;
    //     // if (pDirent->d_type == DT_DIR) // 递归调用
    //     // {
    //     //     // ...
    //     // }
    //     _filePathList.push_back(_dirPath + "/" + pDirent->d_name);
    // }

    traverse(_dirPath);
}

void DirScanner::traverse(const string &dirName)
{
    DIR *fDir = opendir(dirName.c_str());
    ERROR_CHECK(fDir, nullptr, "opendir");

    struct dirent *pDirent;
    while (NULL != (pDirent = readdir(fDir)))
    {
        if (strcmp(pDirent->d_name, ".") == 0 || strcmp(pDirent->d_name, "..") == 0)
            continue;
        else if (pDirent->d_type == 8) //普通文件
        {
            string filePath;
            filePath = dirName + "/" + pDirent->d_name;
            _filePathList.push_back(filePath);
        }
        else if (pDirent->d_type == 4) //目录
        {
            string strNextdir = dirName + "/" + pDirent->d_name;
            traverse(strNextdir);
        }
    }

    closedir(fDir);
}

vector<string> &DirScanner::getFilePathList()
{
    return _filePathList;
}
}; // namespace wdcpp
