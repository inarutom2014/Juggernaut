#include <iostream>
#include <cstdio>
#include "src/common/sha1.h"
    int main()
{
    Jugg::common::SHA_Context shaContent;
    const unsigned char hello[] = "123";
    shaContent.write(hello, 3);
    shaContent.final();
    std::cout << shaContent.getSHA1() << std::endl;
    return 0;
}