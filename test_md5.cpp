#include "src/common/md5.h"
#include "src/config/configuration.h"
#include <iostream>
#include <cstdio>
using Jugg::common::Md5_Context;

bool testMd5(Md5_Context &md5Context, const std::string &buf, const std::string &expectMd5)
{
    md5Context.reset();
    md5Context.write(buf);
    md5Context.final();
    std::string md5str = md5Context.getMd5();
    if (md5str != expectMd5)
    {
        std::cout << "md5:" << buf << " error expect:" << expectMd5 << " now:" << md5str << std::endl;
        return false;
    }else{
        std::cout << "md5:" << buf << " success" << std::endl;
        return true;
    }
}

int main()
{
    Jugg::common::Md5_Context md5Context;
    testMd5(md5Context, "The quick brown fox jumps over the lazy dog", "9e107d9d372bb6826bd81d3542a419d6");
    testMd5(md5Context, "The quick brown fox jumps over the lazy cog", "1055d3e698d289f2af8663725127bd4b");
    testMd5(md5Context, "", "d41d8cd98f00b204e9800998ecf8427e");
    return 0;
}