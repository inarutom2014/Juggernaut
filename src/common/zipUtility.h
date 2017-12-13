#pragma once
#include <stddef.h>

namespace jugg
{
namespace common
{

class ZipUtility
{
public:
    enum ZipResult
    {
        ZipResult_FILE_NOT_FOUND = -2000,   //找不到文件
        ZipResult_READ_FAILURE,             //读文件失败
        ZipResult_WRITE_FAILURE,            //写文件失败
        ZipResult_CREATE_FILE_FAILURE,      //创建文件失败
        ZipResult_Compress_FAILURE,         //压缩失败
        ZipResult_FILE_TOO_BIG,             //文件太大(暂时不支持大文件压缩)
        ZipResult_Error = -1,               //错误

        ZipResult_OK = 0, //返回结果正确
    };

public:
    static int Compress(unsigned char *dest, size_t *destLen, const unsigned char *source, size_t sourceLen, int level);
    static int Uncompress(unsigned char *dest, size_t *destLen, const unsigned char *source, size_t sourceLen);
    static int CompressFile(const char *sourceFilePath, const char *destFilePath, int compressLevel);
    static int UncompressFile(const char *sourceFilePath, const char *destFilePath);
    static int CopyFile(const char *sourceFilePath, const char *destFilePath);
};

}
}