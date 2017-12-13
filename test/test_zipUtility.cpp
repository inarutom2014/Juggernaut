#include "../src/common/zipUtility.h"
#include "../src/common/logging.h"
#include <string.h>
#include <string>
#include <iostream>
#include <iomanip>

void outputCompressBuff(unsigned char *buff, size_t len)
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

int testCompress()
{
    std::string content("The windowBits parameter is the base two logarithm of the window size (the size of the history buffer). It should be in the range 8..15 for this version of the library. Larger values of this parameter result in better compression at the expense of memory usage. The default value is 15 if deflateInit is used instead.");
    unsigned char compress[1024];
    uint32_t compressLen = 1024;
    unsigned char org[1024];
    uint32_t orgLen = 1024;
    int result = jugg::common::ZipUtility::Compress(compress, &compressLen, reinterpret_cast<const unsigned char *>(content.c_str()), content.length(), 8);
    if (result != 0)
    {
        std::cout << "compress False" << std::endl;
        return 0;
    }
    outputCompressBuff(compress, compressLen);
    result = jugg::common::ZipUtility::Uncompress(org, &orgLen, compress, compressLen);
    if (result != 0)
    {
        std::cout << "uncompress False" << std::endl;
        return 0;
    }
    outputOrgBuff(org, orgLen);
    return 0;
}

void testCopyFile()
{
    int result = jugg::common::ZipUtility::CopyFile("/Users/lishuai/Desktop/IMG_1085.JPG", "/Users/lishuai/Desktop/IMG_1085_Copy.JPG");
    if (result != jugg::common::ZipUtility::ZipResult_OK)
    {
        std::cout << "Copy False" << std::endl;
    }
    else
    {
        std::cout << "Copy True" << std::endl;
    }
}

void testCompressFile()
{
    int result = jugg::common::ZipUtility::CompressFile("/Users/lishuai/Desktop/libiPhone-lib.a", "/Users/lishuai/Desktop/libiPhone-lib_compress.a", 8);
    if (result != jugg::common::ZipUtility::ZipResult_OK)
    {
        std::cout << "CompressFile False" << std::endl;
    }
    else
    {
        std::cout << "CompressFile True" << std::endl;
    }
}

void testUncompressFile()
{
    int result = jugg::common::ZipUtility::UncompressFile("/Users/lishuai/Desktop/libiPhone-lib_compress.a", "/Users/lishuai/Desktop/libiPhone-lib_org.a");
    if (result != jugg::common::ZipUtility::ZipResult_OK)
    {
        std::cout << "UncompressFile False" << std::endl;
    }
    else
    {
        std::cout << "UncompressFile True" << std::endl;
    }
}

int main()
{
    // testCopyFile();
    testCompressFile();
    testUncompressFile();
    return 0;
}