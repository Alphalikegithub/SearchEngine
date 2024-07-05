#include "../../../include/SplitTool.h"
#include "../../../include/DictProducer.h"
#include "../../../include/Configuration.h" 

using namespace std;
using namespace wdcpp;

void test()
{
    string cnYuliaoPath = Configuration::getInstance()->getConfigMap()["cnDir"];
    string enYuliaoPath = Configuration::getInstance()->getConfigMap()["enDir"];

    SplitTool tool;
    DictProducer endict1(enYuliaoPath);
    DictProducer endict2(cnYuliaoPath, &tool);
}

int main()
{
    test();
    return 0;
}

