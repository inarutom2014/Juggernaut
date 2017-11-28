#include "src/common/sha1.h"
#include "src/config/configuration.h"
#include <iostream>
#include <cstdio>
using Jugg::common::SHA_Context;

bool testSha1(SHA_Context &sha1Context, const std::string &buf, const std::string &expectsha1)
{
    sha1Context.reset();
    sha1Context.write(buf);
    sha1Context.final();
    std::string sha1_str = sha1Context.getSHA1();
    if (sha1_str != expectsha1)
    {
        std::cout << "sha1:" << buf << " error expect:" << expectsha1 << " now:" << sha1_str << std::endl;
        return false;
    }
    else
    {
        std::cout << "md5:" << buf << " success" << std::endl;
        return true;
    }
}

int main()
{
    Jugg::common::SHA_Context shaContent;
    testSha1(shaContent, "This is a simple of sha1", "52609d401c66541f9fc52cc2bc19b30a37edc8ca");
    testSha1(shaContent, "hello world", "2aae6c35c94fcfb415dbe95f408b9ce91ee846ed");
    testSha1(shaContent, "This is b simple of sha1", "495da0f010915389c0810f11e89a89b837e974ea");
    return 0;
}