#include "../../../include/SplitTool.h"

namespace wdcpp
{
SplitTool::SplitTool()
    :_jieba(DICT_PATH,
            HMM_PATH,
            USER_DICT_PATH)
{
}

vector<string> SplitTool::cut(const string &sentence)
{
    vector<string> result;
    _jieba.CutForSearch(sentence, result);
    return result;
}
}//end of namespace wdcpp
