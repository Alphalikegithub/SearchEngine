#include "../../include/TimerTask.h"
#include "../../include/CacheManager.h"

namespace wdcpp
{
void TimerTask::process()
{
    CacheManager::getInstance()->sync(); // 同步缓存
}
}; // namespace wdcpp

