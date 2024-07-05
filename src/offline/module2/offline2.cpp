#include "../../../include/PageLib.h"
#include "../../../include/Configuration.h"
using namespace wdcpp;

#include <iostream>
using std::cin;
using std::cout;
using std::endl;

int main(int argc, char *argv[])
{
    PageLib lib(Configuration::getInstance()->getConfigMap()["pages"]);
    lib.create();
    lib.store();   

    return 0;
}
