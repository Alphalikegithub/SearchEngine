#include "../../../include/OffsetProcesser.h"
#include "../../../include/WebPage.h"

using std::make_pair;

namespace wdcpp
{
OffsetProcesser::OffsetProcesser(vector<WebPage> &pagelist, vector<pair<size_t, size_t>> &offsetlib)
    : _pagelist(pagelist),
      _offsetlib(offsetlib)
{
}

/**
 *  生成偏移库 _offsetlib
 */
void OffsetProcesser::process()
{
    size_t offset = 0;
    for (auto &page : _pagelist)
    {
        size_t textlength = page.getDoc().size();
        _offsetlib.push_back({offset, textlength});
        offset += textlength;
    }
}
}; // namespace wdcpp
