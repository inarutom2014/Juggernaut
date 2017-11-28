#include "src/common/zipUtility.h"
#include <cstring>
#include <string>
#include <iostream>
#include <iomanip>

void outputCompressBuff(unsigned char*buff, size_t len)
{
    for (int i = 0; i < len; i++)
    {
        std::cout << std::hex
                  << std::setw(2) << std::setfill('0')
                  << static_cast<int>(buff[i]);
    }
    std::cout << std::endl;
}

void outputOrgBuff(unsigned char *buff, size_t len)
{
    for (int i = 0; i < len; i++)
    {
        std::cout << buff[i];
    }
    std::cout << std::endl;
}

int main()
{
    std::string content("The windowBits parameter is the base two logarithm of the window size (the size of the history buffer). It should be in the range 8..15 for this version of the library. Larger values of this parameter result in better compression at the expense of memory usage. The default value is 15 if deflateInit is used instead.");
    unsigned char compress[1024];
    size_t compressLen = 1024;
    unsigned char org[1024];
    size_t orgLen = 1024;
    int result = Jugg::common::ZipUtility::compress(compress, &compressLen, reinterpret_cast<const unsigned char *>(content.c_str()), content.length(), 8);
    if (result != 0)
    {
        std::cout << "compress fail" << std::endl;
        return 0;
    }
    outputCompressBuff(compress, compressLen);
    result = Jugg::common::ZipUtility::uncompress(org, &orgLen, compress, compressLen);
    if (result != 0)
    {
        std::cout << "uncompress fail" << std::endl;
        return 0;
    }
    outputOrgBuff(org, orgLen);
    return 0;
}