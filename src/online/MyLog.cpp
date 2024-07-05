#include "../../include/MyLog.h"

namespace wdcpp
{
MyLog *MyLog::_pInstance = MyLog::getInstance();

MyLog::MyLog()
    : _root(Category::getRoot().getInstance("wd"))
{
    //日志格式
    auto ptnLayout1 = new PatternLayout();
    ptnLayout1->setConversionPattern("%d %c [%p] %m%n");

    auto ptnLayout2 = new PatternLayout();
    ptnLayout2->setConversionPattern("%d %c [%p] %m%n");

    //日志目的地
    OstreamAppender* ostreamAppender =
        new OstreamAppender("OstreamAppender", &cout);    
    ostreamAppender->setLayout(ptnLayout1);

    auto pFileapp = new FileAppender("fileapp", "../log/wd.log");
    pFileapp->setLayout(ptnLayout2);

    //日志过滤器
    _root.setPriority(Priority::DEBUG);
    _root.addAppender(ostreamAppender);
    _root.addAppender(pFileapp);
}

MyLog *MyLog::getInstance()
{
    if (nullptr == _pInstance)
    {
        _pInstance = new MyLog();
        ::atexit(destroy);
    }
    return _pInstance;
}

void MyLog::destroy()
{
    if (_pInstance)
    {
        delete _pInstance;
        _pInstance = nullptr;
    }
}

MyLog::~MyLog()
{
    Category::shutdown();
}
};
