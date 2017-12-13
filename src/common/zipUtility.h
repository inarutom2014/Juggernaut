#pragma once
#include <stddef.h>
#include <stdint.h>

namespace jugg
{
namespace common
{
    //压缩文件头信息 兼容大文件压缩
    struct ZipInfoHeader
    {
        uint64_t orignalSize;   //原始文件总大小
        uint32_t blockSize;     //压缩块大小 为了做大文件压缩 建议控制在4M以内 压缩的过程会分配双倍的内存
        uint32_t blockNum;      //压缩块个数 待定 因为可以通过orignalSize/blockNum
    };
    //压缩块头信息
    struct ZipBlockInfoHeader
    {
        uint32_t orignalSize;   //压缩块的原始大小
        uint32_t compressSize;  //压缩块压缩后的尺寸
    };

class ZipUtility
{
public:
  enum ZipResult
  {
      ZipResult_FILE_NOT_FOUND = -2000,         //找不到文件
      ZipResult_READ_FAILURE,                   //读文件失败
      ZipResult_WRITE_FAILURE,                  //写文件失败
      ZipResult_CREATE_FILE_FAILURE,            //创建文件失败
      ZIPRESULT_COMPRESS_FAILURE,               //压缩失败
      ZIPRESULT_UNCOMPRESS_FAILURE,             //解压失败
      ZipResult_FILE_TOO_BIG,                   //文件太大(暂时不支持大文件压缩)
      ZipResult_Error = -1,                     //返回结果错误
      ZipResult_OK = 0,                         //返回结果正确
  };

public:
  static int Compress(unsigned char *dest, uint32_t *destLen, const unsigned char *source, uint32_t sourceLen, int level);
  static int Uncompress(unsigned char *dest, uint32_t *destLen, const unsigned char *source, uint32_t sourceLen);
  static int CompressFile(const char *sourceFilePath, const char *destFilePath, int compressLevel);
  static int UncompressFile(const char *sourceFilePath, const char *destFilePath);
  static int CopyFile(const char *sourceFilePath, const char *destFilePath);
};

}
}